#pragma once

#include <SDL2/SDL.h>

struct FrequencyLine {
    float freq = 20.0f;
    int weight = 2;
    SDL_Color color = {0, 200, 255, 255};
    float level = 0.0f;
    float y_offset = 0.0f;
};