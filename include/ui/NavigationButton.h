#pragma once

#include <QToolButton>

#include "entity/Common.hpp"

class NavigationButton final : public QToolButton {
    Q_OBJECT
private:
    // 样式设置
    void setupDefaultStyle();
    void setupSelectedStyle();
    void resetToDefaultStyle(const QString& originStyle);

public:
    explicit NavigationButton(const QString& iconPath, const QString& text, QWidget* parent = nullptr);

    void setSelected(bool selected, const QString& originStyle);

    [[nodiscard]] bool isSelected() const {
        return _selected;
    }

private:
    bool _selected{};
};
