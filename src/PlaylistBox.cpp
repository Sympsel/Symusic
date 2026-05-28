#include "PlaylistBox.h"

#include <QPixmap>
#include <QVBoxLayout>
#include <QLabel>

PlaylistBox::PlaylistBox(QString coverPath, QString description, QWidget* parent): QWidget(parent),
    _coverPath(std::move(coverPath)),
    _description(std::move(description)) {
    constexpr int coverLength = 120, coverHeight = 150;
    this->setFixedSize(coverLength, coverHeight);

    const auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    _button = new QPushButton(this);
    _button->setFixedSize(coverLength, coverLength);

    const auto coverLabel = new QLabel(this);
    coverLabel->setPixmap(QPixmap(_coverPath).scaled(
        coverLength, coverLength,
        Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    coverLabel->setFixedSize(coverLength, coverLength);

    const auto descriptionLabel = new QLabel(_description, this);
    descriptionLabel->setFixedSize(coverLength, coverHeight - coverLength);
    descriptionLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(_button);
    layout->addWidget(descriptionLabel);
}
