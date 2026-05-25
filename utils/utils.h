#pragma once

#include <cstddef>
#include <string>

std::string run_command(const std::string& command);
std::string trim_copy(const std::string& value);
std::string to_upper_copy(std::string value);
std::string truncate_copy(const std::string& value, size_t max_chars);