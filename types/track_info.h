#pragma once

#include <string>

struct TrackInfo {
    std::string title;
    std::string artist;
    std::string album;
    std::string player;
    bool available = false;
};