#include "ui/SongInfoPage.h"

#include <QHBoxLayout>
#include <QWidget>
#include <QMouseEvent>

#include "entity/Common.hpp"
#include "entity/SongManager.h"
#include "utils/Log.hpp"
#include "utils/Sync.hpp"

SongInfoPage::SongInfoPage(const SongPtr& song, QWidget* parent)
    : QWidget(parent)
      , _song(song)
      , _originLiked(song->isLiked())
      , _willLike(song->isLiked())
      , _coverLabel(new QLabel())
      , _nameLabel(new QLabel())
      , _artistLabel(new QLabel())
      , _albumLabel(new QLabel())
      , _durationLabel(new QLabel())
      , _tagsLabel(new QLabel())
      , _listsLabel(new QLabel())
      , _likeButton(new QPushButton())
      , _playButton(new QPushButton("  播放"))
      , _closeButton(new QPushButton()) {
    this->setFixedSize(600, 450);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);

    setupUI();
    applyStyles();
    updateSong(song);
}

void SongInfoPage::updateSong(const SongPtr& song) {
    _song = song;

    if (const QPixmap coverPixmap = song->getCover().scaled(
        200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation
    ); !coverPixmap.isNull()) {
        _coverLabel->setPixmap(coverPixmap);
    } else {
        LOG_WARN() << "歌曲封面图片为空";
    }

    _nameLabel->setText(song->getName());
    _artistLabel->setText(QString("歌手：%1").arg(song->getArtist()));
    _albumLabel->setText(QString("专辑：%1").arg(song->getAlbum()));
    _durationLabel->setText(QString("时长：%1").arg(song->getFormattedDuration()));

    const auto tags = song->getTags();
    QString tagsStr;
    for (const auto& tag : tags) {
        tagsStr.append(tag + " ");
    }
    if (!tagsStr.isEmpty() && tagsStr.back() == ' ') {
        tagsStr.removeLast();
    }
    _tagsLabel->setText(tagsStr.isEmpty() ? "标签：无" : "标签：" + tagsStr);

    // 所在列表信息展示
    const auto belongStatus = song->getBelongStatus();
    QStringList listNames;
    for (const auto& allLists = ListMappingManager::getInstance().getAllLists();
         const auto& listInfo : allLists) {
        if (belongStatus & static_cast<int>(listInfo.existIn)) {
            listNames.append(listInfo.name);
        }
    }
    const QString listsStr = listNames.isEmpty() ? "所在列表：无" : "所在列表：" + listNames.join("、");
    _listsLabel->setText(listsStr);
}

void SongInfoPage::setupUI() {
    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(10, 0, 5, 0);

    const auto headerWidget = new QWidget();
    headerWidget->setFixedHeight(50);
    const auto headerLayout = new QHBoxLayout(headerWidget);
    Sync::clearWidgetMargins(headerWidget);
    Sync::clearLayoutMargins(headerLayout);

    const auto titleLabel = new QLabel("歌曲详情");
    auto font = titleLabel->font();
    font.setBold(true);
    titleLabel->setFont(font);

    _closeButton->setFixedSize(40, 40);
    _closeButton->setIcon(QIcon(prefix::normalImages + "关闭.png"));
    _closeButton->setIconSize(QSize(20, 20));
    _closeButton->setCursor(Qt::PointingHandCursor);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(1);
    headerLayout->addWidget(_closeButton);

    const auto contentWidget = new QWidget();
    const auto contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(40, 20, 40, 40);

    const auto topWidget = new QWidget();
    const auto topLayout = new QHBoxLayout(topWidget);
    topLayout->setSpacing(30);

    _coverLabel->setFixedSize(200, 200);
    _coverLabel->setScaledContents(true);
    _coverLabel->setAlignment(Qt::AlignCenter);

    const auto infoWidget = new QWidget();
    const auto infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setSpacing(15);

    _nameLabel->setObjectName("songName");

    Sync::widgetToLayout(
        infoLayout, {
            _nameLabel, _artistLabel, _albumLabel,
            _durationLabel, _tagsLabel, _listsLabel
        });
    infoLayout->addStretch(1);

    const auto buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    _likeButton->setFixedSize(40, 40);
    _likeButton->setIconSize(QSize(24, 24));

    if (_song->isLiked()) {
        _likeButton->setIcon(QIcon(prefix::normalImages + "赞_选中.png"));
        _likeButton->setToolTip("取消喜欢");
    } else {
        _likeButton->setIcon(QIcon(prefix::normalImages + "赞.png"));
        _likeButton->setToolTip("添加喜欢");
    }

    _playButton->setFixedHeight(40);
    _playButton->setMinimumWidth(120);
    _playButton->setIcon(QIcon(prefix::normalImages + "播放.png"));
    _playButton->setIconSize(QSize(20, 20));
    connect(_playButton, &QPushButton::clicked, this, [this]() {
        SongManager::getInstance().play(_song);
    });

    buttonLayout->addWidget(_likeButton);
    buttonLayout->addWidget(_playButton);
    buttonLayout->addStretch(1);

    infoLayout->addLayout(buttonLayout);

    Sync::widgetToLayout(topLayout, {_coverLabel, infoWidget});
    topLayout->addStretch(1);

    contentLayout->addWidget(topWidget);
    contentLayout->addStretch(1);

    Sync::widgetToLayout(mainLayout, {headerWidget, contentWidget});
}

void SongInfoPage::applyStyles() {
    const Color& color = ColorTheme::getInstance().getColor();

    setStyleSheet(QString(
        "#SongInfoPageWidget {"
        "   background-color: rgb(%1);"
        "   border: 2px solid rgb(%2);"
        "}"
        "#songName {"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QLabel {"
        "   color: rgb(200, 200, 200);"
        "   background-color: transparent;"
        "}"
    ).arg(color.background, color.border));

    _likeButton->setStyleSheet(QString(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 30);"
        "}"
    ));

    _closeButton->setStyleSheet(QString(
        "QPushButton {"
        "   background-color: rgb(%1);"
        "   border: none;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgb(%2);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgb(%3);"
        "}"
    ).arg(color.background, color.hoverOn, color.pressed));

    connect(_closeButton, &QPushButton::clicked, this, [this]() {
        this->close();
    });

    connect(_likeButton, &QPushButton::clicked, this, [this]() {
        _willLike = !_willLike;
        if (_willLike) {
            _likeButton->setIcon(QIcon(prefix::normalImages + "赞_选中.png"));
            _likeButton->setToolTip("取消喜欢");
        } else {
            _likeButton->setIcon(QIcon(prefix::normalImages + "赞.png"));
            _likeButton->setToolTip("添加喜欢");
        }
        // 这里将添加或移除到喜欢列表推迟到当前界面关闭
    });
}

void SongInfoPage::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        // 标记事件已处理，即消费了该事件
        this->close();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void SongInfoPage::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - _dragPos);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void SongInfoPage::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        _dragPos = event->globalPosition().toPoint() - geometry().topLeft();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void SongInfoPage::closeEvent(QCloseEvent* event) {
    LOG_DEBUG() << std::format("closeEvent: _originLiked={}, _willLike={}, belongStatus={}",
                               _originLiked, _willLike, _song->getBelongStatus());

    auto& songManager = SongManager::getInstance();
    auto& likedList = songManager.getLikedList();

    if (!_originLiked && _willLike) {
        const bool result = SongManager::append(likedList, _song);
        LOG_DEBUG() << std::format("添加到喜欢列表, append={}, likedList数量: {}", result, likedList.size());
        emit likeStatusChanged();
    } else if (_originLiked && !_willLike) {
        const bool result = SongManager::remove(likedList, _song->getId());
        LOG_DEBUG() << std::format("从喜欢列表移除, remove={}, likedList数量: {}", result, likedList.size());
        emit likeStatusChanged();
    }
    QWidget::closeEvent(event);
}
