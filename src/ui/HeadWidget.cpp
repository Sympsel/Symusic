#include "ui/HeadWidget.h"

#include <QLineEdit>
#include <QTimer>

#include "entity/PathManager.hpp"
#include "utils/Create.hpp"
#include "utils/Log.hpp"
#include "utils/Sync.hpp"

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
    avatarLabel->setPixmap(Create::circularPixmap(prefix::normalImages + "Sympsel.png", 50));

    const auto searchLineEdit = new QLineEdit(parent);
    searchLineEdit->setPlaceholderText("搜索");

    const auto ensureButton = new QPushButton("确认", this);

    QWidget* functionWidget = createFunctionWidget(this);

    Sync::widgetToLayout(layout, {leftWidget, avatarLabel, searchLineEdit, ensureButton, functionWidget});
}

QWidget* HeadWidget::createFunctionWidget(QWidget* parent) {
    const auto functionWidget = new QWidget(parent);

    const auto layout = new QHBoxLayout(functionWidget);

    const auto settingsButton = Create::buttonOnlyIcon("设置.png", functionWidget);
    const auto minimizeButton = Create::buttonOnlyIcon("最小化.png", functionWidget);
    connect(minimizeButton, &QPushButton::clicked, this, [this, minimizeButton]() {
        if (this->parent()) {
            emit minimizeRequested();
        } else {
            minimizeButton->setIcon(QPixmap(""));
            LOG_INFO() << "HeadWidget控件 最大化";
        }
    });
    const auto maximizeButton = Create::buttonOnlyIcon("最大化_1.png", functionWidget);
    connect(maximizeButton, &QPushButton::clicked, this, [this, maximizeButton]() {
        if (this->parent()) {
            emit maximizeRequested();
            QTimer::singleShot(50, this, [this, maximizeButton]() {
                // this->window() 返回顶层窗口的指针，即 MainWidget
                if (this->window()->isMaximized()) {
                    maximizeButton->setIcon(QIcon(prefix::normalImages + "最大化_1.png"));
                    maximizeButton->setToolTip("还原");
                } else {
                    maximizeButton->setIcon(QIcon(prefix::normalImages + "最大化_2.png"));
                    maximizeButton->setToolTip("最大化");
                }
            });
        }
    });

    const auto closeButton = Create::buttonOnlyIcon("关闭.png", functionWidget);
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
