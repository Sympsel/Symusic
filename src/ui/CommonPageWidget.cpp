#include "ui/CommonPageWidget.h"

#include <QKeyEvent>

#include "entity/PlayManager.hpp"
#include "entity/SongManager.h"
#include "entity/StatusManager.hpp"

QWidget* CommonPageWidget::createHeadWidget(const QString& coverFileWithoutPath, const QString& description) const {
    const auto headWidget = new QWidget();
    headWidget->setFixedHeight(120);
    const auto coverLabel = new QLabel();
    coverLabel->setScaledContents(true);
    coverLabel->setPixmap(QPixmap(prefix::normalImages + coverFileWithoutPath));
    coverLabel->setFixedSize(QSize(120, 120));

    const auto hLayout = new QHBoxLayout(headWidget);

    const auto rightWidget = new QWidget(headWidget);
    Sync::clearWidgetMargins(rightWidget);
    const auto vLayout = new QVBoxLayout(rightWidget);

    const auto descriptionLabel = new QLabel(description);
    vLayout->addWidget(descriptionLabel);
    vLayout->addWidget(_playAllButton);
    vLayout->addWidget(descriptionLabel);
    vLayout->addStretch(1);
    vLayout->addWidget(_playAllButton);

    Sync::clearLayoutVMargins({vLayout});
    _playAllButton->setFixedWidth(150);

    Sync::widgetToLayout(hLayout, {
                             coverLabel, rightWidget
                         });

    return headWidget;
}

QWidget* CommonPageWidget::createMiddleWidget() {
    const Color& color = ColorTheme::getInstance().getColor();

    const auto middleWidget = new QWidget();
    const auto layout = new QHBoxLayout(middleWidget);
    const auto musicLabel = new QLabel("音乐");
    const auto singerLabel = new QLabel("歌手");
    const auto albumLabel = new QLabel("专辑");

    const QString labelStyle = QString(
        "QLabel {"
        "   background-color: transparent;"
        "   padding: 4px 8px;"
        "}"
        "QLabel:hover {"
        "   background-color: rgb(%1);"
        "}"
    ).arg(color.hoverOn);

    musicLabel->setAttribute(Qt::WA_Hover, true);
    musicLabel->setStyleSheet(labelStyle);
    singerLabel->setAttribute(Qt::WA_Hover, true);
    singerLabel->setStyleSheet(labelStyle);
    albumLabel->setAttribute(Qt::WA_Hover, true);
    albumLabel->setStyleSheet(labelStyle);


    middleWidget->setFixedHeight(40);
    Sync::widgetToLayout(layout, {
                             // musicLabel, singerLabel, albumLabel
                             {musicLabel, 4},
                             {singerLabel, 3},
                             {albumLabel, 3}
                         });
    return middleWidget;
}

void CommonPageWidget::initData(const SongList& songList) {
    _songList = const_cast<SongList*>(&songList);

    // 调用特化回调，方便对各个页面进行微调
    if (_specializationCb) {
        _specializationCb();
    }
    reloadData(songList);
}

void CommonPageWidget::reloadData(const SongList& songList) {
    _playlist->clear();
    for (const auto& song : songList) {
        const auto item = new QListWidgetItem(_playlist);
        item->setSizeHint(QSize(0, 40));
        if (!_songList) {
            LOG_ERROR() << "未注册播放列表";
            return;
        }
        const auto listItem = new ListItem({song, *_songList});
        _playlist->setItemWidget(item, listItem);
        connect(listItem, &ListItem::doubleClicked, this, [this, song]() {
            emit songItemDoubleClicked(song);
        });
    }
}

void CommonPageWidget::reloadData() {
    if (_reloadCb) {
        _reloadCb(this);
    } else {
        LOG_ERROR() << "未注册页面刷新回调";
    }
}

void CommonPageWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        //if (const auto currItem = _playlist->currentItem()) {
        //    if (const auto listItemWidget = qobject_cast<ListItem*>(_playlist->itemWidget(currItem))) {
        //        const auto song = listItemWidget->getSong();
        //        emit songItemDoubleClicked(song);
        //    }
        //}
        //event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

bool CommonPageWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == _playlist && event->type() == QEvent::KeyPress) {
        if (const auto keyEvent = dynamic_cast<QKeyEvent*>(event); keyEvent->key() == Qt::Key_Space) {
            if (const auto currItem = _playlist->currentItem()) {
                if (const auto listItemWidget = qobject_cast<ListItem*>(_playlist->itemWidget(currItem))) {
                    const auto song = listItemWidget->getSong();
                    emit songItemDoubleClicked(song);
                }
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}


CommonPageWidget::CommonPageWidget(QString pageName, const QString& coverFileWithoutPath, const QString& description,
                                   QWidget* parent) : QWidget(parent)
                                                      , _pageName(std::move(pageName))
                                                      , _playAllButton(new QPushButton("播放全部"))
                                                      , _playlist(new QListWidget())
                                                      , _songList(nullptr) {
    const Color& color = ColorTheme::getInstance().getColor();
    _playlist->setStyleSheet(QString(
        "QListWidget {"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   padding: 0px;"
        "   border: none;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: rgb(%1);"
        "}"
        "QListWidget::item:hover {"
        "   background-color: rgb(%2);"
        "}"
    ).arg(color.hoverOn, color.pressed));

    _playlist->installEventFilter(this);

    const auto mainLayout = new QVBoxLayout(this);

    const auto headWidget = createHeadWidget(coverFileWithoutPath, description);
    const auto middleLabel = createMiddleWidget();

    Sync::widgetToLayout(mainLayout, {
                             headWidget, middleLabel, _playlist
                         });

    connect(_playAllButton, &QPushButton::clicked, this, [this]() {
        if (!_songList || _songList->empty()) {
            StatusManager::getInstance().showMessage("当前列表为空，无法播放", 2000);
            return;
        }


        auto& playManager = PlayManager::getInstance();
        const auto mode = playManager.getPlayMode();

        SongPtr firstSong;
        switch (mode) {
        case PlayMode::RANDOMED:
            {
            static std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<size_t> dis(0, _songList->size() - 1);
            firstSong = (*_songList)[dis(gen)];
            StatusManager::getInstance().showMessage(
                std::format("随机播放: 从 {} 首中随机选择", _songList->size())
            );
            }
            break;
        case PlayMode::ORDERED:
        case PlayMode::SINGLE_LOOPING:
            firstSong = _songList->front();
            StatusManager::getInstance().showMessage(
                std::format("顺序播放全部: {} 首歌曲", _songList->size())
            );
            break;
        }
        playManager.play({firstSong, *_songList});
        connect(&SongManager::getInstance(), &SongManager::updated, this, [this](const SongList& updatedList) {
            if (_songList == &updatedList) {
                emit needUpdate();
            }
        });
    });
}
