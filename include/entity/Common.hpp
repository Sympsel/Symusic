#pragma once

#include <QSet>
#include <QString>

struct Color {
    // 基础颜色
    QString background = "30, 30, 30";
    QString hoverOn = "60, 60, 60";
    QString pressed = "100, 100, 100";
    QString border = "62, 62, 66";

    // 导航按钮颜色
    QString navButtonSelected = "60, 60, 60"; // 选中背景
    QString navButtonSelectedHover = "70, 70, 70"; // 选中悬停
    QString navButtonSelectedPressed = "80, 80, 80"; // 选中按下
    QString navButtonHighlight = "0, 120, 215"; // 高亮边框（蓝色）
    QString navButtonText = "200, 200, 200"; // 未选中文字颜色
    QString navButtonTextSelected = "255, 255, 255"; // 选中文字颜色

    // 推荐页面箭头按钮颜色
    QString arrowButtonHover = "80, 80, 80";
    QString arrowButtonPressed = "0, 122, 204";
    Color() = default;

    // 置顶按钮颜色
    QString fixedActivate = "60, 60, 60";
};

class ColorTheme {
public:
    static ColorTheme& getInstance() {
        static ColorTheme theme;
        return theme;
    }

    [[nodiscard]] const Color& getColor() const {
        return _color;
    }

private:
    ColorTheme() = default;
    Color _color;
};

using SongSupportedList = QSet<QString>;

struct SupportSongType {
private:
    /**
     *
     * audio/mepg - MP3
     * audio/flac - 无损
     * audio/wav
     */
    explicit SupportSongType() : _supportedList{"audio/mepg", "audio/flac", "audio/wav"} {
    }

public:
    [[nodiscard]] static SupportSongType& getInstance() {
        static SupportSongType instance;
        return instance;
    }

    [[nodiscard]] const SongSupportedList& getList() {
        return _supportedList;
    }

private:
    SongSupportedList _supportedList;
};

namespace prefix {
    inline QString normalImages = ":/resource/images/";
    inline QString itemImages = ":/resource/images/items/";

    inline QString songsFile = ":/resource/songs/";
    inline QString songsFileDownload = ":/resource/songs/download";

    inline QString styleFiles = ":/resource/styles/";
}
