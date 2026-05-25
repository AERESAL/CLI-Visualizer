#include "graphics.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "../config/config.h"
#include "../utils/utils.h"

namespace {

std::array<std::string, 7> glyph_for_char(char c) {
    switch (c) {
        case 'A': return {"01110", "10001", "10001", "11111", "10001", "10001", "10001"};
        case 'B': return {"11110", "10001", "10001", "11110", "10001", "10001", "11110"};
        case 'C': return {"01110", "10001", "10000", "10000", "10000", "10001", "01110"};
        case 'D': return {"11110", "10001", "10001", "10001", "10001", "10001", "11110"};
        case 'E': return {"11111", "10000", "10000", "11110", "10000", "10000", "11111"};
        case 'F': return {"11111", "10000", "10000", "11110", "10000", "10000", "10000"};
        case 'G': return {"01110", "10001", "10000", "10011", "10001", "10001", "01110"};
        case 'H': return {"10001", "10001", "10001", "11111", "10001", "10001", "10001"};
        case 'I': return {"01110", "00100", "00100", "00100", "00100", "00100", "01110"};
        case 'J': return {"00111", "00010", "00010", "00010", "10010", "10010", "01100"};
        case 'K': return {"10001", "10010", "10100", "11000", "10100", "10010", "10001"};
        case 'L': return {"10000", "10000", "10000", "10000", "10000", "10000", "11111"};
        case 'M': return {"10001", "11011", "10101", "10101", "10001", "10001", "10001"};
        case 'N': return {"10001", "11001", "10101", "10011", "10001", "10001", "10001"};
        case 'O': return {"01110", "10001", "10001", "10001", "10001", "10001", "01110"};
        case 'P': return {"11110", "10001", "10001", "11110", "10000", "10000", "10000"};
        case 'Q': return {"01110", "10001", "10001", "10001", "10101", "10010", "01101"};
        case 'R': return {"11110", "10001", "10001", "11110", "10100", "10010", "10001"};
        case 'S': return {"01111", "10000", "10000", "01110", "00001", "00001", "11110"};
        case 'T': return {"11111", "00100", "00100", "00100", "00100", "00100", "00100"};
        case 'U': return {"10001", "10001", "10001", "10001", "10001", "10001", "01110"};
        case 'V': return {"10001", "10001", "10001", "10001", "01010", "01010", "00100"};
        case 'W': return {"10001", "10001", "10001", "10101", "10101", "10101", "01010"};
        case 'X': return {"10001", "01010", "01010", "00100", "01010", "01010", "10001"};
        case 'Y': return {"10001", "01010", "00100", "00100", "00100", "00100", "00100"};
        case 'Z': return {"11111", "00010", "00100", "00100", "01000", "10000", "11111"};
        case '0': return {"01110", "10001", "10011", "10101", "11001", "10001", "01110"};
        case '1': return {"00100", "01100", "00100", "00100", "00100", "00100", "01110"};
        case '2': return {"01110", "10001", "00001", "00010", "00100", "01000", "11111"};
        case '3': return {"11110", "00001", "00001", "01110", "00001", "00001", "11110"};
        case '4': return {"00010", "00110", "01010", "10010", "11111", "00010", "00010"};
        case '5': return {"11111", "10000", "10000", "11110", "00001", "00001", "11110"};
        case '6': return {"01110", "10000", "10000", "11110", "10001", "10001", "01110"};
        case '7': return {"11111", "00001", "00010", "00100", "01000", "01000", "01000"};
        case '8': return {"01110", "10001", "10001", "01110", "10001", "10001", "01110"};
        case '9': return {"01110", "10001", "10001", "01111", "00001", "00001", "01110"};
        case ':': return {"00000", "00100", "00100", "00000", "00100", "00100", "00000"};
        case '-': return {"00000", "00000", "00000", "11111", "00000", "00000", "00000"};
        case '/': return {"00001", "00010", "00100", "01000", "10000", "00000", "00000"};
        case '.': return {"00000", "00000", "00000", "00000", "00000", "00100", "00100"};
        case ',': return {"00000", "00000", "00000", "00000", "00100", "00100", "01000"};
        case '&': return {"00110", "01001", "00110", "01010", "10010", "10010", "01101"};
        case '(': return {"00010", "00100", "01000", "01000", "01000", "00100", "00010"};
        case ')': return {"01000", "00100", "00010", "00010", "00010", "00100", "01000"};
        case '\'': return {"00100", "00100", "00000", "00000", "00000", "00000", "00000"};
        case '!': return {"00100", "00100", "00100", "00100", "00100", "00000", "00100"};
        case '?': return {"01110", "10001", "00001", "00010", "00100", "00000", "00100"};
        case ' ': return {"00000", "00000", "00000", "00000", "00000", "00000", "00000"};
        default:  return {"01110", "10001", "00010", "00100", "00100", "00000", "00100"};
    }
}

void draw_glyph(SDL_Renderer* renderer, int x, int y, char c, int scale, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    char glyph_char = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    std::array<std::string, 7> glyph = glyph_for_char(glyph_char);

    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (glyph[row][col] != '1') {
                continue;
            }

            SDL_Rect pixel = {x + col * scale, y + row * scale, scale, scale};
            SDL_RenderFillRect(renderer, &pixel);
        }
    }
}

void draw_text(SDL_Renderer* renderer, int x, int y, const std::string& text, int scale, SDL_Color color) {
    int cursor_x = x;
    for (char c : text) {
        draw_glyph(renderer, cursor_x, y, c, scale, color);
        cursor_x += 6 * scale;
    }
}

void draw_music_overlay(SDL_Renderer* renderer, int width, const TrackInfo& track, float fps) {
    const int scale = 2;
    const int left = 20;
    const int top = 20;
    const int line_height = 8 * scale;
    const int panel_width = std::max(360, std::min(std::max(width - 40, 0), 980));
    const int panel_height = 6 * line_height + 36;
    const int char_width = 6 * scale;
    const size_t max_chars = std::max<size_t>(24, static_cast<size_t>((panel_width - 28) / char_width));

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 10, 12, 18, 180);
    SDL_Rect panel = {left, top, panel_width, panel_height};
    SDL_RenderFillRect(renderer, &panel);

    SDL_SetRenderDrawColor(renderer, 80, 220, 255, 255);
    SDL_Rect accent = {left, top, panel_width, 4};
    SDL_RenderFillRect(renderer, &accent);

    SDL_Color title_color = {230, 245, 255, 255};
    SDL_Color label_color = {120, 210, 255, 255};
    SDL_Color muted_color = {180, 190, 200, 255};
    SDL_Color fps_color = {255, 210, 120, 255};

    std::string header = track.available ? "NOW PLAYING" : "NO MEDIA METADATA";
    std::string title_line = "TITLE: " + (track.title.empty() ? std::string("UNKNOWN") : track.title);
    std::string artist_line = "ARTIST: " + (track.artist.empty() ? std::string("UNKNOWN") : track.artist);
    std::string album_line = "ALBUM: " + (track.album.empty() ? std::string("UNKNOWN") : track.album);
    std::string player_line = "PLAYER: " + (track.player.empty() ? std::string("UNKNOWN") : track.player);
    char fps_buffer[32];
    std::snprintf(fps_buffer, sizeof(fps_buffer), "FPS: %.1f", fps);
    std::string fps_line = fps_buffer;

    header = truncate_copy(to_upper_copy(header), max_chars);
    title_line = truncate_copy(to_upper_copy(title_line), max_chars);
    artist_line = truncate_copy(to_upper_copy(artist_line), max_chars);
    album_line = truncate_copy(to_upper_copy(album_line), max_chars);
    player_line = truncate_copy(to_upper_copy(player_line), max_chars);
    fps_line = truncate_copy(to_upper_copy(fps_line), max_chars);

    draw_text(renderer, left + 12, top + 10, header, scale + 1, label_color);
    draw_text(renderer, left + 12, top + 10 + line_height, title_line, scale, title_color);
    draw_text(renderer, left + 12, top + 10 + line_height * 2, artist_line, scale, title_color);
    draw_text(renderer, left + 12, top + 10 + line_height * 3, album_line, scale, muted_color);
    draw_text(renderer, left + 12, top + 10 + line_height * 4, player_line, scale, muted_color);
    draw_text(renderer, left + 12, top + 10 + line_height * 5, fps_line, scale, fps_color);
}

void draw_frequency_chart(SDL_Renderer* renderer, int width, int height, const std::vector<FrequencyLine>& lines) {
    if (lines.empty()) {
        return;
    }

    const int left = 20;
    const int bottom = 20;
    const int chart_width = 320;
    const int row_height = 14;
    const int chart_height = std::min(24 + static_cast<int>(lines.size()) * row_height, height - 40);
    const int top = std::max(20, height - bottom - chart_height);
    const int label_width = 74;
    const int bar_x = left + label_width + 10;
    const int bar_width = chart_width - label_width - 20;
    const int char_scale = 1;
    const size_t max_label_chars = 6;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 10, 12, 18, 180);
    SDL_Rect panel = {left, top, chart_width, chart_height};
    SDL_RenderFillRect(renderer, &panel);

    SDL_SetRenderDrawColor(renderer, 80, 220, 255, 255);
    SDL_Rect accent = {left, top, chart_width, 3};
    SDL_RenderFillRect(renderer, &accent);

    SDL_Color title_color = {120, 210, 255, 255};
    SDL_Color label_color = {225, 235, 245, 255};
    SDL_Color bar_bg_color = {40, 45, 58, 255};
    SDL_Color value_color = {140, 235, 255, 255};

    draw_text(renderer, left + 10, top + 8, "FREQ / LVL", 1, title_color);

    int usable_rows = std::max(1, chart_height - 24);
    int max_visible_rows = std::max(1, usable_rows / row_height);
    int visible_rows = std::min<int>(static_cast<int>(lines.size()), max_visible_rows);

    for (int i = 0; i < visible_rows; ++i) {
        const FrequencyLine& line = lines[static_cast<size_t>(i)];
        int row_y = top + 22 + i * row_height;
        float normalized_level = std::clamp(line.level, 0.0f, 1.0f);
        int filled_width = static_cast<int>(bar_width * normalized_level);

        SDL_SetRenderDrawColor(renderer, bar_bg_color.r, bar_bg_color.g, bar_bg_color.b, 220);
        SDL_Rect bar_background = {bar_x, row_y + 1, bar_width, 8};
        SDL_RenderFillRect(renderer, &bar_background);

        SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, 255);
        SDL_Rect bar_fill = {bar_x, row_y + 1, std::max(1, filled_width), 8};
        SDL_RenderFillRect(renderer, &bar_fill);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 90);
        SDL_RenderDrawRect(renderer, &bar_background);

        std::string freq_label = truncate_copy(std::to_string(static_cast<int>(line.freq)) + "HZ", max_label_chars);
        std::string level_label = truncate_copy(std::to_string(static_cast<int>(normalized_level * 100.0f)) + "%", 4);
        draw_text(renderer, left + 10, row_y, freq_label, char_scale, label_color);
        draw_text(renderer, left + chart_width - 42, row_y, level_label, char_scale, value_color);
    }
}

}  // namespace

void draw_polar_frequency_visualizer(SDL_Renderer* renderer, int width, int height, float t, const Config& cfg, const TrackInfo& track, float fps) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    std::vector<FrequencyLine> lines = cfg.frequency_lines;
    if (lines.empty()) {
        lines = default_frequency_lines();
    }

    const float center_x = static_cast<float>(width) * 0.5f;
    const float center_y = static_cast<float>(height) * 0.5f;
    const float base_radius = std::min(width, height) * 0.18f;
    const float ring_radius = std::min(width, height) * 0.30f;
    const float beat = 0.5f + 0.5f * std::sin(t * 1.2f);
    const size_t line_count = std::max<size_t>(1, lines.size());
    float max_level = 0.0f;

    for (const FrequencyLine& line : lines) {
        max_level = std::max(max_level, line.level);
    }

    float polar_idle_scale = max_level < 0.02f ? 0.02f : 1.0f;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
    for (int ring = 1; ring <= 3; ++ring) {
        int radius = static_cast<int>(ring_radius * (static_cast<float>(ring) / 3.0f));
        for (int deg = 0; deg < 360; deg += 2) {
            float a1 = static_cast<float>(deg) * static_cast<float>(M_PI) / 180.0f;
            float a2 = static_cast<float>(deg + 2) * static_cast<float>(M_PI) / 180.0f;
            SDL_RenderDrawLine(renderer,
                               static_cast<int>(center_x + std::cos(a1) * radius),
                               static_cast<int>(center_y + std::sin(a1) * radius),
                               static_cast<int>(center_x + std::cos(a2) * radius),
                               static_cast<int>(center_y + std::sin(a2) * radius));
        }
    }

    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    for (int deg = 0; deg < 360; deg += 12) {
        float angle = static_cast<float>(deg) * static_cast<float>(M_PI) / 180.0f;
        SDL_RenderDrawLine(renderer,
                           static_cast<int>(center_x + std::cos(angle) * (base_radius - 14)),
                           static_cast<int>(center_y + std::sin(angle) * (base_radius - 14)),
                           static_cast<int>(center_x + std::cos(angle) * (base_radius + 14)),
                           static_cast<int>(center_y + std::sin(angle) * (base_radius + 14)));
    }

    SDL_SetRenderDrawColor(renderer, 120, 220, 255, 255);
    SDL_Rect center_dot = {static_cast<int>(center_x - 4), static_cast<int>(center_y - 4), 8, 8};
    SDL_RenderFillRect(renderer, &center_dot);

    for (size_t i = 0; i < line_count; ++i) {
        const FrequencyLine& line = lines[i];
        int harmonic = std::max(1, static_cast<int>(std::round(line.freq / 100.0f)));
        float base_level = (0.006f + line.level * cfg.audio_multiplier * 0.08f) * polar_idle_scale;
        float radius_offset = line.y_offset;
        float base_line_radius = base_radius + radius_offset;
        int thickness = std::max(1, line.weight);
        int steps = 240;

        SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
        int previous_x = 0;
        int previous_y = 0;
        int first_x = 0;
        int first_y = 0;
        bool has_previous = false;

        for (int step = 0; step < steps; ++step) {
            float theta = (static_cast<float>(step) / static_cast<float>(steps)) * 2.0f * static_cast<float>(M_PI);
            float radial_wave = std::sin(theta * static_cast<float>(harmonic));
            float radius = base_line_radius + radial_wave * ring_radius * base_level * (0.75f + 0.25f * beat);
            float minimum_radius = base_line_radius + std::max(1.0f, ring_radius * 0.008f);
            radius = std::max(radius, minimum_radius);

            int x = static_cast<int>(center_x + std::cos(theta) * radius);
            int y = static_cast<int>(center_y + std::sin(theta) * radius);

            if (has_previous) {
                for (int offset = 0; offset < thickness; ++offset) {
                    int thickness_offset = offset - thickness / 2;
                    SDL_RenderDrawLine(renderer, previous_x, previous_y + thickness_offset, x, y + thickness_offset);
                }
            } else {
                first_x = x;
                first_y = y;
            }

            previous_x = x;
            previous_y = y;
            has_previous = true;
        }

        for (int offset = 0; offset < thickness; ++offset) {
            int thickness_offset = offset - thickness / 2;
            SDL_RenderDrawLine(renderer, previous_x, previous_y + thickness_offset, first_x, first_y + thickness_offset);
        }
    }

    draw_music_overlay(renderer, width, track, fps);
    draw_frequency_chart(renderer, width, height, lines);
    SDL_RenderPresent(renderer);
}

void draw_radial_bars_visualizer(SDL_Renderer* renderer, int width, int height, float t, const Config& cfg, const TrackInfo& track, float fps) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    std::vector<FrequencyLine> lines = cfg.frequency_lines;
    if (lines.empty()) {
        lines = default_frequency_lines();
    }

    const float center_x = static_cast<float>(width) * 0.5f;
    const float center_y = static_cast<float>(height) * 0.5f;
    const float base_radius = std::min(width, height) * 0.16f;
    const float bar_span = std::min(width, height) * 0.34f;
    const size_t line_count = std::max<size_t>(1, lines.size());

    float max_level = 0.0f;
    for (const FrequencyLine& line : lines) {
        max_level = std::max(max_level, line.level);
    }

    float idle_scale = max_level < 0.02f ? 0.02f : 1.0f;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
    for (int ring = 1; ring <= 4; ++ring) {
        int radius = static_cast<int>(bar_span * (static_cast<float>(ring) / 4.0f));
        for (int deg = 0; deg < 360; deg += 3) {
            float a1 = static_cast<float>(deg) * static_cast<float>(M_PI) / 180.0f;
            float a2 = static_cast<float>(deg + 3) * static_cast<float>(M_PI) / 180.0f;
            SDL_RenderDrawLine(renderer,
                               static_cast<int>(center_x + std::cos(a1) * radius),
                               static_cast<int>(center_y + std::sin(a1) * radius),
                               static_cast<int>(center_x + std::cos(a2) * radius),
                               static_cast<int>(center_y + std::sin(a2) * radius));
        }
    }

    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_RenderDrawLine(renderer,
                       static_cast<int>(center_x - base_radius),
                       static_cast<int>(center_y),
                       static_cast<int>(center_x + base_radius),
                       static_cast<int>(center_y));

    SDL_SetRenderDrawColor(renderer, 120, 220, 255, 255);
    SDL_Rect center_dot = {static_cast<int>(center_x - 4), static_cast<int>(center_y - 4), 8, 8};
    SDL_RenderFillRect(renderer, &center_dot);

    for (size_t i = 0; i < line_count; ++i) {
        const FrequencyLine& line = lines[i];
        float angle_center = (static_cast<float>(i) / static_cast<float>(line_count)) * 2.0f * static_cast<float>(M_PI) - t * 0.02f;
        float bar_base_radius = base_radius + line.y_offset;
        float bar_length = std::max(1.0f, bar_span * (0.10f + line.level * cfg.audio_multiplier * 0.10f) * idle_scale);
        float minimum_length = bar_span * 0.03f;
        bar_length = std::max(bar_length, minimum_length);

        float spacing_radius = bar_base_radius + bar_length;
        float arc_spacing = (2.0f * static_cast<float>(M_PI) * spacing_radius) / static_cast<float>(line_count);
        int max_bar_thickness = std::max(1, static_cast<int>(arc_spacing - 5.0f));
        int bar_thickness = std::min(std::max(5, line.weight * 4), max_bar_thickness);
        SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);

        float cos_theta = std::cos(angle_center);
        float sin_theta = std::sin(angle_center);
        float perp_x = -sin_theta;
        float perp_y = cos_theta;
        int x1 = static_cast<int>(center_x + cos_theta * bar_base_radius);
        int y1 = static_cast<int>(center_y + sin_theta * bar_base_radius);
        int x2 = static_cast<int>(center_x + cos_theta * (bar_base_radius + bar_length));
        int y2 = static_cast<int>(center_y + sin_theta * (bar_base_radius + bar_length));

        for (int thickness_step = 0; thickness_step < bar_thickness; ++thickness_step) {
            float thickness_offset = static_cast<float>(thickness_step) - static_cast<float>(bar_thickness - 1) * 0.5f;
            int offset_x = static_cast<int>(perp_x * thickness_offset);
            int offset_y = static_cast<int>(perp_y * thickness_offset);
            SDL_RenderDrawLine(renderer, x1 + offset_x, y1 + offset_y, x2 + offset_x, y2 + offset_y);
        }
    }

    draw_music_overlay(renderer, width, track, fps);
    draw_frequency_chart(renderer, width, height, lines);
    SDL_RenderPresent(renderer);
}

void draw_frequency_visualizer(SDL_Renderer* renderer, int width, int height, float t, const Config& cfg, const TrackInfo& track, float fps) {
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

    const std::vector<FrequencyLine>& lines_ref = cfg.frequency_lines;
    std::vector<FrequencyLine> fallback_lines;
    const std::vector<FrequencyLine>* lines_ptr = &lines_ref;
    if (lines_ref.empty()) {
        fallback_lines = default_frequency_lines();
        lines_ptr = &fallback_lines;
    }
    const std::vector<FrequencyLine>& lines = *lines_ptr;

    const float center_y = static_cast<float>(height) * 0.5f;
    const float base_amplitude = static_cast<float>(height) * (cfg.amplitude_percent / 100.0f);
    float max_level = 0.0f;
    for (const FrequencyLine& line : lines) {
        max_level = std::max(max_level, line.level);
    }

    float idle_scale = max_level < 0.02f ? 0.04f : 1.0f;
    int draw_step = lines.size() > 8 ? 3 : 2;

    for (size_t index = 0; index < lines.size(); ++index) {
        const FrequencyLine& line = lines[index];
        float amplitude = std::max(0.5f, base_amplitude * 0.35f) * idle_scale * (0.06f + line.level * cfg.audio_multiplier);
        float wave_speed = 0.65f + line.freq / 1200.0f;
        float wave_frequency = 1.0f + line.freq / 220.0f;
        int thickness = line.weight > 0 ? line.weight : cfg.line_weight;
        float line_center_y = center_y + line.y_offset;

        for (int x = 0; x < width - 1; x += draw_step) {
            float x1_norm = static_cast<float>(x) / static_cast<float>(width);
            int next_x = std::min(x + draw_step, width - 1);
            float x2_norm = static_cast<float>(next_x) / static_cast<float>(width);

            float phase1 = x1_norm * wave_frequency * 2.0f * static_cast<float>(M_PI) - t * wave_speed;
            float phase2 = x2_norm * wave_frequency * 2.0f * static_cast<float>(M_PI) - t * wave_speed;

            float y1 = line_center_y + std::sin(phase1) * amplitude;
            float y2 = line_center_y + std::sin(phase2) * amplitude;

            SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
            for (int i = 0; i < thickness; i++) {
                int offset = i - thickness / 2;
                SDL_RenderDrawLine(renderer, x, static_cast<int>(y1) + offset, next_x, static_cast<int>(y2) + offset);
            }
        }
    }

    draw_music_overlay(renderer, width, track, fps);
    draw_frequency_chart(renderer, width, height, lines);
    SDL_RenderPresent(renderer);
}