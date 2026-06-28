#pragma once

#include <QToolButton>

#include "entity/Common.hpp"

class NavigationButton final : public QToolButton {
private:
    // 样式设置
    void setupDefaultStyle();
    void setupSelectedStyle(const Color& color);
    void resetToDefaultStyle(const QString& originStyle, const Color& color);

public:
    explicit NavigationButton(const QString& iconPath, const QString& text, QWidget* parent = nullptr);

    void setSelected(bool selected, const QString& originStyle, const Color& color);

    [[nodiscard]] bool isSelected() const {
        return _selected;
    }

private:
    bool _selected{};
    Color _color;
};
