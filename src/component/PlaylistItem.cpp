module symusic.component.playlist_item;

#include <utility>
#include <QVBoxLayout>
#include <QLabel>

PlaylistItem::PlaylistItem(const PlayContext& songCtx, QString description, QWidget* parent)
    : QWidget(parent)
      , _songCtx(songCtx)
      , _description(std::move(description)) {
    auto& [song, songList] = _songCtx;
    constexpr int coverLength = 120, coverHeight = 150;
    this->setFixedSize(coverLength, coverHeight);

    const auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 4);
    layout->setSpacing(4);

    _button = new QPushButton(this);
    _button->setFocusPolicy(Qt::NoFocus);
    _button->setFixedSize(coverLength - 16, coverLength - 16);
    _button->setIcon(song->getCover());
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
    // 安装事件过滤器
    _button->installEventFilter(this);
    connect(_button, &QPushButton::clicked, this, [this]() {
        PlayManager::getInstance().play(_songCtx);
    });

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
