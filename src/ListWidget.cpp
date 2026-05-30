#include "../include/ListWidget.h"

#include <QVBoxLayout>

#include "../include/NavigationButton.h"
#include "../include/Sync.hpp"

ListWidget::ListWidget(QWidget* parent, const QString& path, const QString& groupName, const std::initializer_list<NavigationButton*>& buttons) {
    this->setParent(parent);
    if (parent == nullptr) {
        this->setWindowFlag(Qt::FramelessWindowHint);
    }

    // 组名行
    _groupHead = new QWidget(this);
    const auto labelLayout = new QHBoxLayout(_groupHead);

    // 让图标贴左边
    labelLayout->setContentsMargins(2, 4, 2, 4);
    labelLayout->setSpacing(8);

    const QIcon icon(path);
    const auto iconLabel = new QLabel(_groupHead);
    iconLabel->setPixmap(icon.pixmap(QSize(16, 16)));
    iconLabel->setFixedSize(16, 16);

    const auto groupNameLabel = new QLabel(groupName, _groupHead);

    labelLayout->addWidget(iconLabel);
    labelLayout->addWidget(groupNameLabel);
    labelLayout->addStretch(1);

    // ===== 按钮列表 =====
    const auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addWidget(_groupHead);

    for (const auto& button : buttons) {
        layout->addWidget(button);
        if (button && button->parentWidget() == nullptr) {
            button->setParent(this);
        }
    }
}