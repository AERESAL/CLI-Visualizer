#include "audio.h"

#include <algorithm>
#include <cmath>
#include <fcntl.h>
#include <iostream>
#include <sstream>

#include "../utils/utils.h"

AudioAnalyzer::AudioAnalyzer(int buffer_size) : audio_buffer(buffer_size, 0) {}

std::string AudioAnalyzer::choose_output_target() {
    std::string output = run_command("wpctl status 2>/dev/null");
    std::istringstream stream(output);
    std::string line;

    bool in_sinks = false;

    while (std::getline(stream, line)) {
        if (line.find("Sinks:") != std::string::npos) {
            in_sinks = true;
            continue;
        }

        if (in_sinks && line.find("Sources:") != std::string::npos) {
            break;
        }

        if (!in_sinks) {
            continue;
        }

        if (line.find("Speaker") == std::string::npos && line.find('*') == std::string::npos) {
            continue;
        }

        size_t dot_pos = line.find('.');
        if (dot_pos == std::string::npos) {
            continue;
        }

        size_t start = line.find_first_of("0123456789");
        if (start == std::string::npos || start >= dot_pos) {
            continue;
        }

        std::string target = line.substr(start, dot_pos - start);
        if (!target.empty()) {
            return target;
        }
    }

    return {};
}

void AudioAnalyzer::init() {
    output_target = choose_output_target();
    if (output_target.empty()) {
        std::cerr << "Failed to find an output sink for playback capture" << std::endl;
        return;
    }

    std::string command =
        "pw-cat -r --target " + output_target +
        " --format s16 --channels 2 --rate 44100 -a -";

    audio_pipe = popen(command.c_str(), "r");
    if (!audio_pipe) {
        std::cerr << "Failed to open pw-cat capture stream" << std::endl;
        return;
    }

    int pipe_fd = fileno(audio_pipe);
    if (pipe_fd >= 0) {
        int flags = fcntl(pipe_fd, F_GETFL, 0);
        if (flags >= 0) {
            fcntl(pipe_fd, F_SETFL, flags | O_NONBLOCK);
        }
    }

    sample_rate = 44100;
    std::cout << "Audio input active: " << sample_rate << "Hz using output sink " << output_target << std::endl;
}

void AudioAnalyzer::update(std::vector<FrequencyLine>& lines) {
    if (!audio_pipe) {
        return;
    }

    Uint32 now = SDL_GetTicks();
    if (now - last_analysis_ticks < 16) {
        return;
    }

    uint8_t buf[4096];
    size_t len = fread(buf, 1, sizeof(buf), audio_pipe);

    if (len > 0) {
        int16_t* samples = reinterpret_cast<int16_t*>(buf);
        int count = static_cast<int>(len / sizeof(int16_t));

        for (int i = 0; i < count; i++) {
            audio_buffer[buffer_pos++] = samples[i];
            if (buffer_pos >= static_cast<int>(audio_buffer.size())) {
                buffer_pos = 0;
            }
        }

        analyze_frequency_lines(lines);
        last_analysis_ticks = now;
    }
}

std::vector<int16_t> AudioAnalyzer::ordered_samples() const {
    std::vector<int16_t> samples;
    samples.reserve(audio_buffer.size());

    for (size_t i = 0; i < audio_buffer.size(); ++i) {
        size_t index = (buffer_pos + i) % audio_buffer.size();
        samples.push_back(audio_buffer[index]);
    }

    return samples;
}

float AudioAnalyzer::analyze_frequency(const std::vector<int16_t>& samples, float target_freq) const {
    int n = static_cast<int>(samples.size());
    if (n < 2 || sample_rate <= 0) {
        return 0.0f;
    }

    double k = 0.5 + ((static_cast<double>(n) * target_freq) / static_cast<double>(sample_rate));
    double omega = (2.0 * M_PI * k) / static_cast<double>(n);
    double coeff = 2.0 * std::cos(omega);
    double q0 = 0.0;
    double q1 = 0.0;
    double q2 = 0.0;

    for (int i = 0; i < n; ++i) {
        double sample = static_cast<double>(samples[i]) / 32768.0;
        q0 = coeff * q1 - q2 + sample;
        q2 = q1;
        q1 = q0;
    }

    double power = q1 * q1 + q2 * q2 - coeff * q1 * q2;
    double level = std::log1p(std::max(0.0, power)) * 0.12;
    return static_cast<float>(std::min(1.0, level));
}

void AudioAnalyzer::analyze_frequency_lines(std::vector<FrequencyLine>& lines) {
    std::vector<int16_t> samples = ordered_samples();
    for (FrequencyLine& line : lines) {
        float new_level = analyze_frequency(samples, line.freq);
        line.level = line.level * 0.7f + new_level * 0.3f;
    }
}

AudioAnalyzer::~AudioAnalyzer() {
    if (audio_pipe) {
        pclose(audio_pipe);
    }
}