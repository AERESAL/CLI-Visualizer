#include "config.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

bool parse_float_after_key(const std::string& text, const std::string& key, float& value) {
    size_t pos = text.find(key);
    if (pos == std::string::npos) {
        return false;
    }

    pos = text.find(':', pos);
    if (pos == std::string::npos) {
        return false;
    }

    char* end = nullptr;
    const char* start = text.c_str() + pos + 1;
    value = std::strtof(start, &end);
    return end != start;
}

bool parse_int_after_key(const std::string& text, const std::string& key, int& value) {
    float temp = 0.0f;
    if (!parse_float_after_key(text, key, temp)) {
        return false;
    }

    value = static_cast<int>(temp);
    return true;
}

bool parse_string_after_key(const std::string& text, const std::string& key, std::string& value) {
    size_t pos = text.find(key);
    if (pos == std::string::npos) {
        return false;
    }

    pos = text.find(':', pos);
    if (pos == std::string::npos) {
        return false;
    }

    pos = text.find('"', pos);
    if (pos == std::string::npos) {
        return false;
    }

    size_t end = text.find('"', pos + 1);
    if (end == std::string::npos || end <= pos + 1) {
        return false;
    }

    value = text.substr(pos + 1, end - pos - 1);
    return true;
}

bool parse_color_after_key(const std::string& text, const std::string& key, SDL_Color& color) {
    size_t pos = text.find(key);
    if (pos == std::string::npos) {
        return false;
    }

    pos = text.find('[', pos);
    if (pos == std::string::npos) {
        return false;
    }

    const char* start = text.c_str() + pos + 1;
    char* end = nullptr;

    long r = std::strtol(start, &end, 10);
    if (end == start) {
        return false;
    }

    long g = std::strtol(end + 1, &end, 10);
    if (end == nullptr) {
        return false;
    }

    long b = std::strtol(end + 1, &end, 10);
    if (end == nullptr) {
        return false;
    }

    color.r = static_cast<uint8_t>(std::clamp<long>(r, 0, 255));
    color.g = static_cast<uint8_t>(std::clamp<long>(g, 0, 255));
    color.b = static_cast<uint8_t>(std::clamp<long>(b, 0, 255));
    color.a = 255;
    return true;
}

std::vector<FrequencyLine> parse_frequency_lines(const std::string& text) {
    std::vector<FrequencyLine> lines;
    size_t lines_pos = text.find("lines");
    if (lines_pos == std::string::npos) {
        return lines;
    }

    size_t array_start = text.find('[', lines_pos);
    if (array_start == std::string::npos) {
        return lines;
    }

    size_t array_end = std::string::npos;
    int depth = 0;
    for (size_t i = array_start; i < text.size(); ++i) {
        if (text[i] == '[') {
            depth++;
        } else if (text[i] == ']') {
            depth--;
            if (depth == 0) {
                array_end = i;
                break;
            }
        }
    }

    if (array_end == std::string::npos || array_end <= array_start) {
        return lines;
    }

    std::string array_text = text.substr(array_start + 1, array_end - array_start - 1);
    size_t search_pos = 0;

    while (true) {
        size_t object_start = array_text.find('{', search_pos);
        if (object_start == std::string::npos) {
            break;
        }

        size_t object_end = array_text.find('}', object_start + 1);
        if (object_end == std::string::npos) {
            break;
        }

        std::string object_text = array_text.substr(object_start, object_end - object_start + 1);
        FrequencyLine line;
        bool has_freq = parse_float_after_key(object_text, "freq", line.freq);
        parse_int_after_key(object_text, "weight", line.weight);
        parse_color_after_key(object_text, "color", line.color);
        parse_float_after_key(object_text, "y_offset", line.y_offset);

        if (has_freq) {
            lines.push_back(line);
        }

        search_pos = object_end + 1;
    }

    return lines;
}

}  // namespace

std::vector<FrequencyLine> default_frequency_lines() {
    std::vector<FrequencyLine> lines;
    for (float freq = 20.0f; freq <= 1500.0f; freq += 100.0f) {
        FrequencyLine line;
        line.freq = freq;
        line.weight = 2;
        line.y_offset = 0.0f;
        float ratio = (freq - 20.0f) / 1480.0f;
        line.color = {
            static_cast<uint8_t>(40 + ratio * 40),
            static_cast<uint8_t>(180 + ratio * 40),
            static_cast<uint8_t>(255 - ratio * 120),
            255
        };
        lines.push_back(line);
    }

    if (lines.empty() || lines.back().freq < 1500.0f) {
        FrequencyLine last;
        last.freq = 1500.0f;
        last.weight = 2;
        last.color = {255, 120, 180, 255};
        lines.push_back(last);
    }

    return lines;
}

Config load_config(const std::string& filename) {
    Config cfg = {0.1f, 2, 2.0f, 50, true, 8.0f, GraphType::Rectangular, {}};

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: config.json not found, using defaults" << std::endl;
        cfg.frequency_lines = default_frequency_lines();
        return cfg;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();

    parse_float_after_key(text, "amplitude_percent", cfg.amplitude_percent);
    parse_int_after_key(text, "line_weight", cfg.line_weight);
    parse_float_after_key(text, "speed", cfg.speed);
    parse_int_after_key(text, "grid_spacing", cfg.grid_spacing);
    {
        int val = cfg.audio_enabled ? 1 : 0;
        if (parse_int_after_key(text, "audio_enabled", val)) {
            cfg.audio_enabled = val != 0;
        }
    }
    parse_float_after_key(text, "audio_multiplier", cfg.audio_multiplier);
    {
        std::string graph_type_text;
        if (parse_string_after_key(text, "graph_type", graph_type_text)) {
            std::transform(graph_type_text.begin(), graph_type_text.end(), graph_type_text.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            if (graph_type_text == "polar") {
                cfg.graph_type = GraphType::Polar;
            } else if (graph_type_text == "radial_bars") {
                cfg.graph_type = GraphType::RadialBars;
            } else {
                cfg.graph_type = GraphType::Rectangular;
            }
        }
    }

    cfg.frequency_lines = parse_frequency_lines(text);
    if (cfg.frequency_lines.empty()) {
        cfg.frequency_lines = default_frequency_lines();
    }

    std::cout << "Config loaded" << std::endl;
    return cfg;
}