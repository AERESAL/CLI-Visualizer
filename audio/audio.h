#pragma once

#include <SDL2/SDL.h>

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "../types/frequency_line.h"

class AudioAnalyzer {
public:
    int sample_rate = 44100;
    std::vector<int16_t> audio_buffer;
    int buffer_pos = 0;
    Uint32 last_analysis_ticks = 0;
    FILE* audio_pipe = nullptr;
    std::string output_target;

    explicit AudioAnalyzer(int buffer_size = 1024);

    static std::string choose_output_target();
    void init();
    void update(std::vector<FrequencyLine>& lines);
    ~AudioAnalyzer();

private:
    std::vector<int16_t> ordered_samples() const;
    float analyze_frequency(const std::vector<int16_t>& samples, float target_freq) const;
    void analyze_frequency_lines(std::vector<FrequencyLine>& lines);
};