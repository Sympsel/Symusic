module symusic.component.navigation_button;

import symusic.common;

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
    const auto& [hover, selected, selectedAndHover,
            selectedAndPressed, leftHighlight,
            text, textSelected]
        = ColorTheme::getInstance().getNavigationColor();
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
            text, hover, selectedAndPressed
        )
    );
}

void NavigationButton::setupSelectedStyle() {
    const auto& [hover, selected, selectedAndHover,
            selectedAndPressed, leftHighlight,
            text, textSelected]
        = ColorTheme::getInstance().getNavigationColor();
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
        .arg(selected,
             leftHighlight,
             textSelected,
             selectedAndHover,
             selectedAndPressed)
    );
}

void NavigationButton::resetToDefaultStyle(const QString& originStyle) {
    if (originStyle.isEmpty()) {
        setupDefaultStyle();
    } else {
        this->setStyleSheet(originStyle);
    }
}

void NavigationButton::setSelected(const bool selected, const QString& originStyle) {
    if (_selected == selected) {
        return;
    }
    _selected = selected;
    if (selected) {
        setupSelectedStyle();
    } else {
        resetToDefaultStyle(originStyle);
    }
}
