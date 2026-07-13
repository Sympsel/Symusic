#pragma once

#include <vector>
#include <print>
#include <fstream>
#include <regex>

struct LyricItem {
    LL pos;
    std::string text;

    bool operator<(const LyricItem& other) const {
        return pos < other.pos;
    }
};

class LyricsParser {
public:
    static auto parse(const QString& lrcFile) -> std::vector<LyricItem> {
        std::vector<LyricItem> lrcs;
        std::ifstream ifs{lrcFile.toStdString(), std::ios::in};
        std::string line;
        while (std::getline(ifs, line)) {
            LL pos{};
            std::string text;
            std::regex reg{"[(\\d+):(\\d+)\\.(\\d+)](.+)"};
            std::smatch matches;
            std::regex_match(line, matches, reg);
            if (matches.size() == 4) {
                try {
                    const LL min = std::stoll(matches[0]);
                    const LL snd = std::stoll(matches[1]);
                    const LL mcs = std::stoll(matches[2]);
                    pos = (min * 60 + snd) * 1'000 + mcs;
                } catch (const std::exception& e) {
                    LOG_WARN() << std::format("Invalid lrc line: {}", line);
                }
                text = matches[3];
            } else {
                LOG_WARN() << std::format("Invalid lrc line: {}", line);
            }

            lrcs.emplace_back(pos, text);
        }
        return lrcs;
    }
};
