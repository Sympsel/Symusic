module;

#include <QString>

export module symusic.common.color;

export namespace Color {
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

export class ColorTheme {
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
