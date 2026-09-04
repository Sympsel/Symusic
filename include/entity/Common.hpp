#pragma once

#include <QSet>
#include <QString>
#include <QStandardPaths>

namespace Color {
    struct Base {
        // 边框
        QString bd = "62,62,66";
    };

    struct Nav {
        QString hover = "60,60,60";
        QString selected = "60,60,60";
        QString selectedAndHover = "70,70,70";
        QString selectedAndPressed = "80,80,80";
        QString leftHighlight = "0,125,215";
        QString text = "200,200,200";
        QString textSelected = "255,255,255";
    };

    struct ControlButton {
        QString hover = "60,60,60";
        QString pressed = "100,100,100";
    };

    struct ArrowButton {
        QString hover = "80,80,80";
        QString pressed = "100,100,100";
    };

    struct PlayItem {
        QString hover = "40,40,40";
        QString pressed = "100,100,100";
        QString playing = "80,80,80";
    };

    struct FixedButton {
        QString activate = "60,60,60";
    };

    struct Label {
        QString text = "200,200,200";
        QString hover = "60,60,60";
    };
}

class ColorTheme {
public:
    ColorTheme(const ColorTheme&) = delete;
    ColorTheme(ColorTheme&&) = delete;
    ColorTheme& operator=(const ColorTheme&) = delete;
    ColorTheme& operator=(ColorTheme&&) = delete;

    static ColorTheme& getInstance() {
        static ColorTheme theme;
        return theme;
    }

    [[nodiscard]] const Color::Base& getBaseColor() const {
        return _base;
    }

    [[nodiscard]] const Color::Nav& getNavigationColor() const {
        return _nav;
    }

    [[nodiscard]] const Color::ControlButton& getControlButtonColor() const {
        return _ctl;
    }

    [[nodiscard]] const Color::ArrowButton& getArrowButtonColor() const {
        return _arrow;
    }

    [[nodiscard]] const Color::PlayItem& getPlayItemColor() const {
        return _pi;
    }

    [[nodiscard]] const Color::FixedButton& getFixedButtonColor() const {
        return _fixed;
    }

    [[nodiscard]] const QString& getGlobalBGColor() const {
        return _bg;
    }

    [[nodiscard]] const Color::Label& getLabelColor() const {
        return _label;
    }

private:
    ColorTheme() = default;
    QString _bg = "30,30,30";
    Color::Base _base;
    Color::Nav _nav;
    Color::ControlButton _ctl;
    Color::ArrowButton _arrow;
    Color::PlayItem _pi;
    Color::FixedButton _fixed;
    Color::Label _label;
};

using SongSupportedList = QSet<QString>;

struct SupportSongType {
private:
    /**
     *
     * audio/mpeg - MP3
     * audio/flac - 无损
     * audio/wav
     */
    explicit SupportSongType() : _supportedList{"audio/mpeg", "audio/flac", "audio/wav"} {
    }

public:
    SupportSongType(const SupportSongType&) = delete;
    SupportSongType(SupportSongType&&) = delete;
    SupportSongType& operator=(const SupportSongType&) = delete;
    SupportSongType& operator=(SupportSongType&&) = delete;

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
    inline QString cacheCoverDir() {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/cache/cover/";
    }

    inline QString normalImages = ":/resource/images/";
    inline QString itemImages = ":/resource/images/items/";

    inline QString songsFile = ":/resource/songs/";
    inline QString songsFileDownload = ":/resource/songs/download";

    inline QString styleFiles = ":/resource/styles/";
}

enum class PlayMode {
    ORDERED,
    RANDOMED,
    SINGLE_LOOPING
};
