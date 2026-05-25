#include "utils.h"

#include <cctype>
#include <cstdio>

std::string run_command(const std::string& command) {
    std::string output;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return output;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }

    pclose(pipe);
    return output;
}

std::string trim_copy(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        start++;
    }

    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        end--;
    }

    return value.substr(start, end - start);
}

std::string to_upper_copy(std::string value) {
    for (char& c : value) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return value;
}

std::string truncate_copy(const std::string& value, size_t max_chars) {
    if (value.size() <= max_chars) {
        return value;
    }

    if (max_chars <= 3) {
        return value.substr(0, max_chars);
    }

    return value.substr(0, max_chars - 3) + "...";
}