#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Config {
    float amplitude_percent;
    int line_weight;
    float speed;
    int grid_spacing;
    bool audio_enabled;
    float audio_multiplier;
    bool glow_enabled;
    int glow_size;
};

Config load_config(const std::string& filename) {
    Config cfg = {0.1f, 2, 2.0f, 50, true, 8.0f, true, 15};
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: config.json not found, using defaults" << std::endl;
        return cfg;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("amplitude_percent") != std::string::npos) {
            sscanf(line.c_str(), "%*[^:]:%f", &cfg.amplitude_percent);
        } else if (line.find("line_weight") != std::string::npos) {
            sscanf(line.c_str(), "%*[^:]:%d", &cfg.line_weight);
        } else if (line.find("speed") != std::string::npos) {
            sscanf(line.c_str(), "%*[^:]:%f", &cfg.speed);
        } else if (line.find("grid_spacing") != std::string::npos) {
            sscanf(line.c_str(), "%*[^:]:%d", &cfg.grid_spacing);
        } else if (line.find("audio_enabled") != std::string::npos) {
            int val;
            sscanf(line.c_str(), "%*[^:]:%d", &val);
            cfg.audio_enabled = val;
        } else if (line.find("audio_multiplier") != std::string::npos) {
            sscanf(line.c_str(), "%*[^:]:%f", &cfg.audio_multiplier);
        } else if (line.find("glow_enabled") != std::string::npos) {
            int val;
            sscanf(line.c_str(), "%*[^:]:%d", &val);
            cfg.glow_enabled = val;
        } else if (line.find("glow_size") != std::string::npos) {
            sscanf(line.c_str(), "%*[^:]:%d", &cfg.glow_size);
        }
    }
    
    std::cout << "Config loaded" << std::endl;
    return cfg;
}

class AudioAnalyzer {
public:
    float low_freq = 0.0f;
    float high_freq = 0.0f;
    SDL_AudioDeviceID device = 0;
    std::vector<int16_t> audio_buffer;
    int buffer_pos = 0;
    
    AudioAnalyzer(int buffer_size = 512) : audio_buffer(buffer_size, 0) {}
    
    void init() {
        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = 44100;
        want.format = AUDIO_S16;
        want.channels = 1;
        want.samples = 256;
        want.callback = NULL;
        
        device = SDL_OpenAudioDevice(NULL, 1, &want, &have, 0);
        if (device == 0) {
            std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
            return;
        }
        
        std::cout << "Audio input active: " << have.freq << "Hz" << std::endl;
        SDL_PauseAudioDevice(device, 0);
    }
    
    void update() {
        if (device == 0) return;
        
        uint8_t buf[2048];
        int len = SDL_DequeueAudio(device, buf, sizeof(buf));
        
        if (len > 0) {
            int16_t* samples = (int16_t*)buf;
            int count = len / sizeof(int16_t);
            
            for (int i = 0; i < count; i++) {
                audio_buffer[buffer_pos++] = samples[i];
                if (buffer_pos >= (int)audio_buffer.size()) {
                    buffer_pos = 0;
                }
            }
            
            analyze_frequencies();
        }
    }
    
    void analyze_frequencies() {
        int n = audio_buffer.size();
        
        float low_sum = 0.0f, high_sum = 0.0f;
        
        for (int i = 0; i < n; i += 4) {
            low_sum += std::abs(audio_buffer[i]) / 32768.0f;
        }
        low_freq = std::min(1.0f, low_sum / (n / 4.0f) * 2.0f);
        
        for (int i = 1; i < n - 1; i++) {
            float diff = std::abs((float)audio_buffer[i+1] - audio_buffer[i-1]);
            high_sum += diff / 65536.0f;
        }
        high_freq = std::min(1.0f, high_sum / (n - 2.0f) * 2.0f);
    }
    
    ~AudioAnalyzer() {
        if (device != 0) {
            SDL_CloseAudioDevice(device);
        }
    }
};

void draw_glow_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness, int glow_size) {
    // Draw glow layers
    for (int g = glow_size; g > 0; g--) {
        uint8_t alpha = (uint8_t)(200 * (1.0f - (float)g / glow_size) * (1.0f - (float)g / glow_size));
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, alpha);
        
        for (int i = 0; i < thickness + g; i++) {
            int offset = i - (thickness + g) / 2;
            SDL_RenderDrawLine(renderer, x1, y1 + offset, x2, y2 + offset);
        }
    }
    
    // Draw bright core
    SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < thickness; i++) {
        int offset = i - thickness / 2;
        SDL_RenderDrawLine(renderer, x1, y1 + offset, x2, y2 + offset);
    }
}

void draw_sine_wave(SDL_Renderer* renderer, int width, int height, float t, const Config& cfg, float low_freq, float high_freq) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
    for (int i = 0; i < width; i += cfg.grid_spacing) {
        SDL_RenderDrawLine(renderer, i, 0, i, height);
    }
    for (int i = 0; i < height; i += cfg.grid_spacing) {
        SDL_RenderDrawLine(renderer, 0, i, width, i);
    }

    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_RenderDrawLine(renderer, 0, height / 2, width, height / 2);

    float base_amplitude = height * (cfg.amplitude_percent / 100.0f);

    for (int x = 0; x < width - 1; x++) {
        float x1_norm = (float)x / 100.0f;
        float x2_norm = (float)(x + 1) / 100.0f;
        
        float x_pos_norm = (float)x / width;
        float envelope = 0.5f + low_freq * cfg.audio_multiplier * (1.0f - std::abs(x_pos_norm * 2.0f - 1.0f));
        float local_mult = 0.5f + high_freq * cfg.audio_multiplier * std::sin(x_pos_norm * M_PI);
        float total_mult = envelope * local_mult;
        float amplitude = base_amplitude * total_mult;
        
        float y1 = std::sin(x1_norm - t) * amplitude + height / 2;
        float y2 = std::sin(x2_norm - t) * amplitude + height / 2;

        if (cfg.glow_enabled) {
            draw_glow_line(renderer, x, (int)y1, x + 1, (int)y2, cfg.line_weight, cfg.glow_size);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
            for (int i = 0; i < cfg.line_weight; i++) {
                int offset = i - cfg.line_weight / 2;
                SDL_RenderDrawLine(renderer, x, (int)y1 + offset, x + 1, (int)y2 + offset);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

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

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        if (cfg.audio_enabled) {
            audio.update();
        }

        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        draw_sine_wave(renderer, width, height, t, cfg, audio.low_freq, audio.high_freq);
        t += cfg.speed * 0.016f;
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
