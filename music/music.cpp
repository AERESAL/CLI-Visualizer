#include "music.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "../utils/utils.h"

namespace {

std::vector<std::string> list_mpris_players() {
    std::vector<std::string> players;
    std::string output = run_command("busctl --user list --no-pager 2>/dev/null");
    std::istringstream stream(output);
    std::string token;

    while (stream >> token) {
        if (token.rfind("org.mpris.MediaPlayer2.", 0) == 0) {
            if (std::find(players.begin(), players.end(), token) == players.end()) {
                players.push_back(token);
            }
        }
    }

    return players;
}

std::string extract_busctl_string_after(const std::string& output, const std::string& marker) {
    size_t pos = output.find(marker);
    if (pos == std::string::npos) {
        return {};
    }

    pos += marker.size();
    std::string value;
    bool escaping = false;

    while (pos < output.size()) {
        char c = output[pos++];
        if (escaping) {
            value.push_back(c);
            escaping = false;
            continue;
        }

        if (c == '\\') {
            escaping = true;
            continue;
        }

        if (c == '"') {
            break;
        }

        value.push_back(c);
    }

    return trim_copy(value);
}

std::string extract_first_artist(const std::string& output) {
    size_t key_pos = output.find("\"xesam:artist\" as ");
    if (key_pos == std::string::npos) {
        return {};
    }

    size_t pos = key_pos + std::string("\"xesam:artist\" as ").size();
    while (pos < output.size() && std::isdigit(static_cast<unsigned char>(output[pos]))) {
        pos++;
    }

    while (pos < output.size() && std::isspace(static_cast<unsigned char>(output[pos]))) {
        pos++;
    }

    if (pos >= output.size() || output[pos] != '"') {
        return {};
    }

    pos++;
    std::string artist;
    bool escaping = false;

    while (pos < output.size()) {
        char c = output[pos++];
        if (escaping) {
            artist.push_back(c);
            escaping = false;
            continue;
        }

        if (c == '\\') {
            escaping = true;
            continue;
        }

        if (c == '"') {
            break;
        }

        artist.push_back(c);
    }

    return trim_copy(artist);
}

bool is_playing_player(const std::string& player_name) {
    std::string output = run_command(
        "busctl --user get-property " + player_name +
        " /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player PlaybackStatus 2>/dev/null"
    );

    return output.find("\"Playing\"") != std::string::npos;
}

}  // namespace

TrackInfo fetch_music_metadata() {
    TrackInfo info;
    std::vector<std::string> players = list_mpris_players();
    if (players.empty()) {
        return info;
    }

    std::string chosen_player = players.front();
    for (const std::string& player : players) {
        if (is_playing_player(player)) {
            chosen_player = player;
            break;
        }
    }

    info.player = chosen_player;
    std::string output = run_command(
        "busctl --user get-property " + chosen_player +
        " /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player Metadata 2>/dev/null"
    );

    info.title = extract_busctl_string_after(output, "\"xesam:title\" s \"");
    info.album = extract_busctl_string_after(output, "\"xesam:album\" s \"");
    info.artist = extract_first_artist(output);
    info.available = !info.title.empty() || !info.artist.empty() || !info.album.empty();
    return info;
}