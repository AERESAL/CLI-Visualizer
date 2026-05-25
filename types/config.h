#pragma once

#include <vector>

#include "frequency_line.h"
#include "graph_type.h"

struct Config {
    float amplitude_percent = 0.1f;
    int line_weight = 2;
    float speed = 2.0f;
    int grid_spacing = 50;
    bool audio_enabled = true;
    float audio_multiplier = 8.0f;
    GraphType graph_type = GraphType::Rectangular;
    std::vector<FrequencyLine> frequency_lines;
};