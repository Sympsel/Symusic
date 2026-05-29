#include "../include/HeadWidget.h"

#include <QLineEdit>

#include "Sync.hpp"
#include "Create.hpp"
#include "Log.hpp"

HeadWidget::HeadWidget(QWidget* parent) {
    this->setFixedHeight(68);
    const auto layout = new QHBoxLayout(this);

    // todo remove or finish it
    const auto leftWidget = new QWidget(this);

    const auto avatarLabel = new QLabel();
    avatarLabel->setFixedSize(QSize(50, 50));
    avatarLabel->setStyleSheet(R"(
    QLabel {
        border-radius: 25px;
    }
)");
    avatarLabel->setScaledContents(true);
    avatarLabel->setPixmap(Create::circularPixmap(":/images/Sympsel.png", 50));

    const auto searchLineEdit = new QLineEdit(parent);
    searchLineEdit->setPlaceholderText("搜索");

    const auto ensureButton = new QPushButton("确认", this);

    QWidget* functionWidget = createFunctionWidget(this);

    // todo two style to select
    Sync::widgetToLayout(layout, {leftWidget, avatarLabel, searchLineEdit, ensureButton, functionWidget});
    // Sync::widgetToLayout(layout, {leftWidget, avatarLabel});
    // layout->addStretch();
    // Sync::widgetToLayout(layout, {searchLineEdit, ensureButton, functionWidget});
}

QWidget* HeadWidget::createFunctionWidget(QWidget* parent) {
    const auto functionWidget = new QWidget(parent);

    const auto layout = new QHBoxLayout(functionWidget);

    const auto settingsButton = new QPushButton(QIcon(":/images/设置.png"), "", functionWidget);
    const auto minimizeButton = new QPushButton(QIcon(":/images/最小化.png"), "", functionWidget);
    connect(minimizeButton, &QPushButton::clicked, this, [this]() {
        if (this->parent()) {
            emit minimizeRequested();
        } else {
            LOG_INFO() << "HeadWidget控件 最大化";
        }
    });
    const auto maximizeButton = new QPushButton(QIcon(":/images/最大化.png"), "", functionWidget);
    connect(maximizeButton, &QPushButton::clicked, this, [this]() {
        if (this->parent()) {
            emit maximizeRequested();
        } else {
            LOG_INFO() << "HeadWidget控件 最小化";
        }
    });
    const auto closeButton = new QPushButton(QIcon(":/images/关闭.png"), "", functionWidget);
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        if (this->parent()) {
            emit closeRequested();
        } else {
            LOG_INFO() << "HeadWidget控件 退出";
            this->close();
        }
    });
    const auto buttons = {settingsButton, minimizeButton, maximizeButton, closeButton};
    this->syncButtonBackground(buttons);
    Sync::buttonFixedSize(QSize(30, 30), buttons);

    layout->addStretch(1);
    Sync::buttonToLayout(layout, buttons);

    return functionWidget;
}

void HeadWidget::syncButtonBackground(const std::initializer_list<QPushButton*>& buttons) const {
    Sync::buttonBackground(buttons, _color.background, _color.hoverOn, _color.pressed);
}
