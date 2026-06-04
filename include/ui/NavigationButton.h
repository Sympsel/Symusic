#pragma once

#include <QToolButton>

#include "entity/Color.hpp"

class NavigationButton final : public QToolButton {
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
