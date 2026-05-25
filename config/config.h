#pragma once

#include <string>
#include <vector>

#include "../types/config.h"

std::vector<FrequencyLine> default_frequency_lines();
Config load_config(const std::string& filename);