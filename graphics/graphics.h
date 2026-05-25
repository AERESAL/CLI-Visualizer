#pragma once

#include <SDL2/SDL.h>

#include "../types/config.h"
#include "../types/track_info.h"

void draw_frequency_visualizer(SDL_Renderer* renderer, int width, int height, float t, const Config& cfg, const TrackInfo& track, float fps);
void draw_polar_frequency_visualizer(SDL_Renderer* renderer, int width, int height, float t, const Config& cfg, const TrackInfo& track, float fps);
void draw_radial_bars_visualizer(SDL_Renderer* renderer, int width, int height, float t, const Config& cfg, const TrackInfo& track, float fps);