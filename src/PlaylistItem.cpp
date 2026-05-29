#include "PlaylistItem.h"

#include <qcoreevent.h>
#include <QVBoxLayout>
#include <QLabel>

#include "Log.hpp"

PlaylistItem::PlaylistItem(QString coverPath, QString description, QWidget* parent) : QWidget(parent),
    _coverPath(std::move(coverPath)),
    _description(std::move(description)) {
    constexpr int coverLength = 120, coverHeight = 150;
    this->setFixedSize(coverLength, coverHeight);

    const auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 4);
    layout->setSpacing(4);

    _button = new QPushButton(this);
    _button->setFixedSize(coverLength - 16, coverLength - 16);
    _button->setIcon(QIcon(_coverPath));
    _button->setIconSize(QSize(coverLength - 16, coverLength - 16));
    _button->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    background: transparent;"
        "    border-radius: 8px;"
        "}"
    );
    // 透明
    _button->setFlat(true);
    // 安装时间过滤器
    _button->installEventFilter(this);

    const auto descriptionLabel = new QLabel(_description, this);
    descriptionLabel->setFixedSize(coverLength - 16, coverHeight - coverLength - 4);
    descriptionLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(_button, 0, Qt::AlignHCenter);
    layout->addWidget(descriptionLabel, 0, Qt::AlignHCenter);
}

bool PlaylistItem::eventFilter(QObject* watched, QEvent* event) {
    if (watched == _button) {
        if (event->type() == QEvent::Enter) {
            _button->move(_button->x(), _button->y() - 4);
            return true;
        }

        if (event->type() == QEvent::Leave) {
            _button->move(_button->x(), _button->y() + 4);
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}
