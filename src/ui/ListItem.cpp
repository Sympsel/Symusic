#include "ui/ListItem.h"

#include <QApplication>
#include <qdatetime.h>
#include <QMouseEvent>
#include <QTimer>
#include <utility>

#include "entity/PlayManager.hpp"
#include "utils/Log.hpp"
#include "entity/SongManager.h"

void ListItem::setupUI() {
    this->setFixedHeight(40);

    const auto mainLayout = new QHBoxLayout(this);
    Sync::clearWidgetMargins(this);
    Sync::clearLayoutMargins(mainLayout);
    mainLayout->setSpacing(8);

    const auto leftWidget = new QWidget();
    const auto centralWidget = new QWidget();
    const auto rightWidget = new QWidget();
    Sync::enabledWidgetStyledBackground({
                                            this, leftWidget, centralWidget, rightWidget
                                        }, true);

    // [收藏按钮 歌曲名 标签]
    const auto leftLayout = new QHBoxLayout(leftWidget);
    Sync::clearWidgetMargins(leftWidget);
    Sync::clearLayoutMargins(leftLayout);
    leftLayout->setSpacing(4);

    leftLayout->addWidget(_likeButton);
    const auto nameLabel = new QLabel(_song->getName());
    Sync::appendStyleSheet(nameLabel, "color: white;");
    leftLayout->addWidget(nameLabel);
    const auto tags = _song->getTags();
    QString tagsStr = "[";
    for (const auto& tag : tags) {
        tagsStr.append(tag + ", ");
    }
    if (!tags.empty()) {
        tagsStr.chop(2);
    }
    tagsStr.append("]");
    const auto tagsLabel = new QLabel(tagsStr);
    Sync::appendStyleSheet(tagsLabel, "color: gray;");
    leftLayout->addWidget(tagsLabel);
    leftLayout->addStretch(1);

    // [歌手]
    const auto centralLayout = new QHBoxLayout(centralWidget);
    Sync::clearWidgetMargins(centralWidget);
    Sync::clearLayoutMargins(centralLayout);
    const auto artistLabel = new QLabel(_song->getArtist());
    Sync::appendStyleSheet(artistLabel, "color: white;");
    centralLayout->addWidget(artistLabel);
    centralLayout->addStretch(1);

    // [专辑]
    const auto rightLayout = new QHBoxLayout(rightWidget);
    Sync::clearWidgetMargins(rightWidget);
    Sync::clearLayoutMargins(rightLayout);

    auto* albumLabel = new QLabel(_song->getAlbum());
    Sync::appendStyleSheet(albumLabel, "color: white;");
    rightLayout->addWidget(albumLabel);

    Sync::widgetTransparentBackground({
        this,
        nameLabel, tagsLabel, artistLabel, albumLabel,
        leftWidget, rightWidget, centralWidget
    });

    Sync::widgetToLayout(mainLayout, {
                             {leftWidget, 3},
                             {centralWidget, 2},
                             {rightWidget, 2}
                         });
}

ListItem::ListItem(SongPtr song, SongList& songList)
    : _clickTimer(new QTimer(this))
      , _likeButton(new QPushButton)
      , _song(std::move(song))
      , _songList(&songList) {
    _likeButton->setFixedSize(24, 24);
    _likeButton->setFocusPolicy(Qt::NoFocus);

    if (_song->isLiked()) {
        _likeButton->setIcon(QPixmap(prefix::normalImages + "赞_选中.png"));
    } else {
        _likeButton->setIcon(QPixmap(prefix::normalImages + "赞.png"));
    }
    _likeButton->setStyleSheet(
        "QPushButton {"
        "   padding: 0px;"
        "   min-height: 0px;"
        "   min-width: 0px;"
        "   max-height: 24px;"
        "   max-width: 24px;"
        "   background-color: transparent;"
        "   border: none;"
        "}"
    );
    setupUI();

    // 设置为单次触发模式，超时自动停止
    _clickTimer->setSingleShot(true);
    _clickTimer->setInterval(QApplication::doubleClickInterval());
    connect(_clickTimer, &QTimer::timeout, this, [this]() {
        if (_pendingSingleClick) {
            _pendingSingleClick = false;
            PlayManager::getInstance().play(_song, *_songList);
            LOG_DEBUG() << "单击播放: " << _song->getName();
        }
    });

    connect(_likeButton, &QPushButton::clicked, this, [this]() {
        auto& likedList = SongManager::getInstance().getLikedList();
        if (!_song->isLiked()) {
            SongManager::append(likedList, _song);
            LOG_DEBUG() << "添加到喜欢列表: " << _song->getName();
        } else {
            SongManager::remove(likedList, _song->getId());
            LOG_DEBUG() << "从喜欢列表移除: " << _song->getName();
        }
        updateIconStatus();
        emit likeStatusUpdated();
    });
}

void ListItem::updateIconStatus() const {
    if (_song->isLiked()) {
        _likeButton->setIcon(QPixmap(prefix::normalImages + "赞_选中.png"));
    } else {
        _likeButton->setIcon(QPixmap(prefix::normalImages + "赞.png"));
    }
}

void ListItem::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (_skipNextPress) {
            // 跳过双击中的第二次 press，即跳过对第二次按压的判别
            _skipNextPress = false;
            return;
        }
        // 当第二次按下时结束等待，判断是否超时，如果没有超时，取消单击逻辑
        LOG_DEBUG() << "mousePressEvent, pending=" << _pendingSingleClick;
        if (_pendingSingleClick) {
            _clickTimer->stop();
            _pendingSingleClick = false;
            return;
        }
        // 当第一次按下时开始等待
        _pendingSingleClick = true;
        _clickTimer->start();
    }
    QWidget::mousePressEvent(event);
}

void ListItem::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        LOG_DEBUG() << "mouseDoubleClickEvent 触发";
        _clickTimer->stop();
        _pendingSingleClick = false;
        _skipNextPress = true;
        emit doubleClicked(_song);
    }
    QWidget::mouseDoubleClickEvent(event);
}
