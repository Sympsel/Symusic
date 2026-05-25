#include "HeadWidget.h"

#include <QLineEdit>

#include "Sync.hpp"
#include "Create.hpp"
#include "Log.hpp"

HeadWidget::HeadWidget(QWidget* parent) {
    this->setFixedHeight(68);
    const auto layout = new QHBoxLayout(this);

    const auto headLeftWidget = new QWidget(this);
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

    Sync::widgetToLayout(layout, {headLeftWidget, avatarLabel, searchLineEdit, ensureButton, functionWidget});
}

QWidget* HeadWidget::createFunctionWidget(QWidget* parent) {
    const auto functionWidget = new QWidget(parent);

    const auto layout = new QHBoxLayout(functionWidget);

    const auto settingsButton = new QPushButton(QIcon(":/images/设置.png"), "", functionWidget);
    const auto minimizeButton = new QPushButton(QIcon(":/images/最小化.png"), "", functionWidget);
    const auto maximizeButton = new QPushButton(QIcon(":/images/最大化.png"), "", functionWidget);
    const auto closeButton = new QPushButton(QIcon(":/images/关闭.png"), "", functionWidget);
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        if (this->parent()) {
            emit closeRequested();
        } else {
            LOG_INFO() << "HeadWidget控件退出";
            this->close();
        }
    });
    const auto buttons = {settingsButton, minimizeButton, maximizeButton, closeButton};
    this->syncButtonBackground(buttons);
    Sync::buttonSize(QSize(30, 30), buttons);

    layout->addStretch(1);
    Sync::buttonToLayout(layout, buttons);

    return functionWidget;
}

void HeadWidget::syncButtonBackground(const std::initializer_list<QPushButton*>& buttons) const {
    Sync::buttonBackground(buttons, _color.background, _color.hoverOn, _color.pressed);
}