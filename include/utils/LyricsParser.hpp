#pragma once

#include <vector>
#include <print>
#include <fstream>
#include <regex>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2frame.h>


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
            std::regex reg{"\\[(\\d+):(\\d+)\\.(\\d+)\\](.+)"};
            std::smatch matches;
            std::regex_match(line, matches, reg);
            if (matches.size() == 5) {
                try {
                    const LL min = std::stoll(matches[1]);
                    const LL snd = std::stoll(matches[2]);
                    const LL mcs = std::stoll(matches[3]);
                    pos = (min * 60 + snd) * 1'000 + mcs;
                } catch (const std::exception& _) {
                    LOG_WARN() << std::format("Invalid lrc line: {}", line);
                }
                text = matches[4];
            } else {
                LOG_WARN() << std::format("Invalid lrc line: {}", line);
                continue;
            }

            lrcs.emplace_back(pos, text);
        }
        return lrcs;
    }

     static auto parseString(const std::string& lrcContent) -> std::vector<LyricItem> {
        std::vector<LyricItem> lrcs;
        std::istringstream iss{lrcContent};
        std::string line;
        std::regex reg{"\\[(\\d+):(\\d+)\\.(\\d+)\\](.+)"};
        while (std::getline(iss, line)) {
            if (line.empty()) continue;
            LL pos{};
            std::string text;
            std::smatch matches;
            std::regex_match(line, matches, reg);
            if (matches.size() == 5) {
                try {
                    const LL min = std::stoll(matches[1]);
                    const LL snd = std::stoll(matches[2]);
                    const LL mcs = std::stoll(matches[3]);
                    pos = (min * 60 + snd) * 1'000 + mcs;
                } catch (...) {
                    continue;
                }
                text = matches[4];
                lrcs.emplace_back(pos, text);
            }
        }
        return lrcs;
    }

    static auto extractLyricsFromMP3(const QString& filePath) -> std::vector<LyricItem> {
        TagLib::MPEG::File mpegFile(filePath.toStdWString().c_str());
        if (!mpegFile.isOpen() || !mpegFile.ID3v2Tag()) {
            return {};
        }

        const auto* id3v2Tag = mpegFile.ID3v2Tag();
        const auto frameList = id3v2Tag->frameListMap()["USLT"];
        if (frameList.isEmpty()) {
            return {};
        }

        const TagLib::ByteVector data = frameList.front()->render();

        // Frame header: 4(FrameID) + 4(Size) + 2(Flags) = 10 bytes
        if (data.size() <= 14) return {};

        const auto encoding = static_cast<unsigned char>(data[10]);

        // Skip: encoding(1) + language(3)
        // Then skip content descriptor (null-terminated) to find lyrics start
        int offset = 14;
        if (encoding == 0 || encoding == 3) {
            // Latin-1 / UTF-8: single-byte null terminator
            while (offset < data.size() && data[offset] != '\0') {
                ++offset;
            }
            ++offset; // skip null terminator
        } else if (encoding == 1) {
            // UTF-16 with BOM: double-byte null terminator
            while (offset + 1 < data.size() && !(data[offset] == '\0' && data[offset + 1] == '\0')) {
                offset += 2;
            }
            offset += 2; // skip double null terminator
        }

        if (offset >= data.size()) return {};

        QString lyricsText;
        if (encoding == 3) {
            // UTF-8
            lyricsText = QString::fromUtf8(data.data() + offset, data.size() - offset);
        } else {
            // Fallback: Latin-1
            lyricsText = QString::fromLatin1(data.data() + offset, data.size() - offset);
        }

        return parseString(lyricsText.toStdString());
    }
};
