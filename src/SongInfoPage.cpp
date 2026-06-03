#include "SongInfoPage.h"

#include <qevent.h>

#include "PathMaganger.hpp"
#include "Sync.hpp"

SongInfoPage::SongInfoPage(const Song& song, QWidget* parent)
    : QWidget(parent)
    , _song(song)
    , _coverLabel(new QLabel())
    , _nameLabel(new QLabel())
    , _artistLabel(new QLabel())
    , _albumLabel(new QLabel())
    , _durationLabel(new QLabel())
    , _tagsLabel(new QLabel())
    , _likeButton(new QPushButton())
    , _playButton(new QPushButton("  播放"))
    , _closeButton(new QPushButton()) {

    setFixedSize(600, 450);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);

    setupUI();
    applyStyles();
    updateSong(song);
}

void SongInfoPage::updateSong(const Song& song) {
    _song = song;

    _coverLabel->setPixmap(song.getCover().scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    _nameLabel->setText(song.getName());
    _artistLabel->setText(QString("歌手：%1").arg(song.getArtist()));
    _albumLabel->setText(QString("专辑：%1").arg(song.getAlbum()));
    _durationLabel->setText(QString("时长：%1").arg(song.getFormattedDuration()));

    const auto tags = song.getTags();
    QString tagsStr;
    for (const auto& tag : tags) {
        tagsStr += "[" + tag + "] ";
    }
    _tagsLabel->setText(tagsStr.isEmpty() ? "标签：无" : "标签：" + tagsStr);

    _likeButton->setIcon(song.isLiked() ? QIcon(prefix::normalImages + "赞_选中.png") : QIcon(":/images/赞.png"));
}

void SongInfoPage::setupUI() {
    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    const auto headerWidget = new QWidget();
    headerWidget->setFixedHeight(50);
    const auto headerLayout = new QHBoxLayout(headerWidget);
    Sync::clearWidgetMargins(headerWidget);
    Sync::clearLayoutMargins(headerLayout);

    const auto titleLabel = new QLabel("歌曲详情");
    titleLabel->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));

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
    // _nameLabel->setFont(QFont("Microsoft YaHei", 18, QFont::Bold));

    // _artistLabel->setFont(QFont("Microsoft YaHei", 12));
    // _albumLabel->setFont(QFont("Microsoft YaHei", 12));
    // _durationLabel->setFont(QFont("Microsoft YaHei", 11));
    // _tagsLabel->setFont(QFont("Microsoft YaHei", 10));

    Sync::widgetToLayout(infoLayout, {
        _nameLabel, _artistLabel, _albumLabel, _durationLabel, _tagsLabel
    });
    infoLayout->addStretch(1);

    const auto buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    _likeButton->setFixedSize(40, 40);
    _likeButton->setIconSize(QSize(24, 24));
    _likeButton->setToolTip("添加到喜欢");

    _playButton->setFixedHeight(40);
    _playButton->setMinimumWidth(120);
    _playButton->setIcon(QIcon(prefix::normalImages + "播放.png"));
    _playButton->setIconSize(QSize(20, 20));

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
        "QWidget {"
        "   background-color: rgb(%1);"
        "}"
        "#songName {"
        "   color: white;"
        "}"
        "QLabel {"
        "   color: rgb(200, 200, 200);"
        "   background-color: transparent;"
        "}"
        "QPushButton {"
        "   background-color: rgb(%2);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgb(%3);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgb(%4);"
        "}"
    ).arg(color.background, color.hoverOn, color.pressed, color.border));

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
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 100, 100, 80);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgba(255, 100, 100, 120);"
        "}"
    ));

    connect(_closeButton, &QPushButton::clicked, this, [this]() {
        this->close();
    });

    connect(_likeButton, &QPushButton::clicked, this, [this]() {
        _song.setLiked(!_song.isLiked());
        _likeButton->setIcon(_song.isLiked() ? QIcon(prefix::normalImages + "赞_选中.png") : QIcon(":/images/赞.png"));
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
