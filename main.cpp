#include <SDL2/SDL.h>

#include <iostream>
#include <string>

#include "audio/audio.h"
#include "config/config.h"
#include "graphics/graphics.h"
#include "music/music.h"

int main() {
    Config cfg = load_config("config.json");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL initialization failed!" << std::endl;
        return 1;
    }

    AudioAnalyzer audio;
    if (cfg.audio_enabled) {
        audio.init();
    }

    SDL_Window* window = SDL_CreateWindow(
        "y = sin(x - t) [Audio Reactive]",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1200, 600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "Window creation failed!" << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed!" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    float t = 0;
    SDL_Event event;
    TrackInfo track_info;
    Uint32 last_metadata_refresh = 0;
    Uint32 last_frame_ticks = SDL_GetTicks();
    float fps = 0.0f;
    std::string base_window_title = "y = sin(x - t) [Audio Reactive]";

    track_info = fetch_music_metadata();
    if (track_info.available) {
        SDL_SetWindowTitle(window, (base_window_title + " | " + track_info.title + " - " + track_info.artist).c_str());
    } else {
        SDL_SetWindowTitle(window, base_window_title.c_str());
    }

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        Uint32 now = SDL_GetTicks();
        if (now - last_metadata_refresh > 3000) {
            track_info = fetch_music_metadata();
            if (track_info.available) {
                SDL_SetWindowTitle(window, (base_window_title + " | " + track_info.title + " - " + track_info.artist).c_str());
            } else {
                SDL_SetWindowTitle(window, base_window_title.c_str());
            }
            last_metadata_refresh = now;
        }

        if (cfg.audio_enabled) {
            audio.update(cfg.frequency_lines);
        }

        Uint32 frame_now = SDL_GetTicks();
        Uint32 frame_delta = frame_now - last_frame_ticks;
        if (frame_delta > 0) {
            float instant_fps = 1000.0f / static_cast<float>(frame_delta);
            fps = fps * 0.9f + instant_fps * 0.1f;
        }
        last_frame_ticks = frame_now;

        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        if (cfg.graph_type == GraphType::Polar) {
            draw_polar_frequency_visualizer(renderer, width, height, t, cfg, track_info, fps);
        } else if (cfg.graph_type == GraphType::RadialBars) {
            draw_radial_bars_visualizer(renderer, width, height, t, cfg, track_info, fps);
        } else {
            draw_frequency_visualizer(renderer, width, height, t, cfg, track_info, fps);
        }

        t += cfg.speed * 0.016f;
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
