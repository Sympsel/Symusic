#include "ui/ListItem.h"

#include <QMouseEvent>

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
    const auto nameLabel = new QLabel(_song.getName());
    Sync::appendStyleSheet(nameLabel, "color: white;");
    leftLayout->addWidget(nameLabel);
    const auto tags = _song.getTags();
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
    const auto artistLabel = new QLabel(_song.getArtist());
    Sync::appendStyleSheet(artistLabel, "color: white;");
    centralLayout->addWidget(artistLabel);
    centralLayout->addStretch(1);

    // [专辑]
    const auto rightLayout = new QHBoxLayout(rightWidget);
    Sync::clearWidgetMargins(rightWidget);
    Sync::clearLayoutMargins(rightLayout);

    auto* albumLabel = new QLabel(_song.getAlbum());
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

ListItem::ListItem(Song song, const bool isLiked) : _isLiked(isLiked)
                                                    , _likeButton(new QPushButton)
                                                    , _song(std::move(song)) {
    _likeButton->setFixedSize(24, 24);
    _likeButton->setFocusPolicy(Qt::NoFocus);
    if (_isLiked) {
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

    connect(_likeButton, &QPushButton::clicked, this, [this]() {
        _isLiked = !_isLiked;
        updateIconStatus();
        auto& likedList = SongManager::getInstance().getLikedList();
        if (_isLiked) {
            SongManager::append(likedList, _song);
            LOG_DEBUG() << "添加到喜欢列表: " << _song;
        } else {
            if (const auto it = std::ranges::find(likedList, _song);
                it != likedList.end()) {
                likedList.erase(it);
                LOG_DEBUG() << "从喜欢列表删除了: " << _song;
            }
        }
        // 发射信号更新界面
        emit likeStatusUpdated();
    });
}

void ListItem::updateIconStatus() const {
    if (_isLiked) {
        _likeButton->setIcon(QPixmap(prefix::normalImages + "赞_选中.png"));
    } else {
        _likeButton->setIcon(QPixmap(prefix::normalImages + "赞.png"));
    }
}

void ListItem::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(_song);
    }
    QWidget::mouseDoubleClickEvent(event);
}
