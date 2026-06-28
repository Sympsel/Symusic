#include "ui/NavigationButton.h"

#include "entity/Common.hpp"

NavigationButton::NavigationButton(const QString& iconPath, const QString& text, QWidget* parent)
    : QToolButton(parent) {
    // UI 组件初始化
    this->setIcon(QIcon(prefix::normalImages + iconPath));
    this->setText(text);
    this->setIconSize(QSize(18, 18));
    this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->setMinimumHeight(36);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 设置默认样式
    setupDefaultStyle();
}

void NavigationButton::setupDefaultStyle() {
    const Color color;
    setStyleSheet(
        QString("QToolButton {"
            "    background-color: transparent;"
            "    border-left: 3px solid transparent;"
            "    color: rgb(%1);"
            "}"
            "QToolButton:hover {"
            "    background-color: rgb(%2);"
            "}"
            "QToolButton:pressed {"
            "    background-color: rgb(%3);"
            "}"
        ).arg(
            color.navButtonText,
            color.hoverOn,
            color.pressed
        )
    );
}

void NavigationButton::setupSelectedStyle(const Color& color) {
    setStyleSheet(
        QString("QToolButton {"
            "    background-color: rgb(%1);"
            "    border-left: 3px solid rgb(%2);"
            "    color: rgb(%3);"
            "}"
            "QToolButton:hover {"
            "    background-color: rgb(%4);"
            "}"
            "QToolButton:pressed {"
            "    background-color: rgb(%5);"
            "}")
        .arg(color.navButtonSelected,
             color.navButtonHighlight,
             color.navButtonTextSelected,
             color.navButtonSelectedHover,
             color.navButtonSelectedPressed)
    );
}

void NavigationButton::resetToDefaultStyle(const QString& originStyle, const Color& color) {
    if (originStyle.isEmpty()) {
        setupDefaultStyle();
    } else {
        setStyleSheet(originStyle);
    }
}

void NavigationButton::setSelected(const bool selected, const QString& originStyle, const Color& color) {
    if (_selected == selected) {
        return;
    }
    _selected = selected;
    if (selected) {
        setupSelectedStyle(color);
    } else {
        resetToDefaultStyle(originStyle, color);
    }
}
