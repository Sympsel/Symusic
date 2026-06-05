#include "ui/NavigationButton.h"

#include "entity/PathManager.hpp"

NavigationButton::NavigationButton(const QString& iconPath, const QString& text, QWidget* parent)
    : QToolButton(parent) {
    this->setIcon(QIcon(prefix::normalImages + iconPath));
    this->setText(text);
    this->setIconSize(QSize(18, 18));
    this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->setMinimumHeight(36);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 设置默认样式（未选中状态）
    Color defaultColor;
    this->setStyleSheet(
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
            defaultColor.navButtonText,
            defaultColor.hoverOn,
            defaultColor.pressed
        )
    );
}

void NavigationButton::setSelected(const bool selected, const QString& originStyle, const Color& color) {
    if (_selected == selected) {
        return;
    }

    _selected = selected;

    if (selected) {
        // 选中状态：添加高亮背景
        this->setStyleSheet(
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
    } else {
        // 未选中状态：恢复原始样式
        if (originStyle.isEmpty()) {
            this->setStyleSheet(
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
                    "}")
                .arg(color.navButtonText,
                     color.hoverOn,
                     color.pressed)
            );
        } else {
            this->setStyleSheet(originStyle);
        }
    }
}
