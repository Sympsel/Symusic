#include "entity/Song.h"

#include <QCoreApplication>
#include <QMimeDatabase>
#include <QTimer>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaMetaData>

#include "utils/Log.hpp"

void Song::parseMusicMeta() {
    if (!_url.isValid()) {
        LOG_WARN() << "无效的URL";
        return;
    }

    QMediaPlayer player;

    bool loaded = false;
    bool hasError = false;
    bool finished = false;

    QObject::connect(&player, &QMediaPlayer::mediaStatusChanged,
                     [&](const QMediaPlayer::MediaStatus status) {
                         if (finished) return;
                         if (status == QMediaPlayer::LoadedMedia ||
                             status == QMediaPlayer::BufferedMedia) {
                             loaded = true;
                             finished = true;
                         } else if (status == QMediaPlayer::InvalidMedia) {
                             hasError = true;
                             finished = true;
                         }
                     });

    QObject::connect(&player, &QMediaPlayer::errorOccurred,
                     [&](QMediaPlayer::Error, const QString& errorString) {
                         LOG_WARN() << "播放器错误:" << errorString;
                         hasError = true;
                         finished = true;
                     });

    player.setSource(_url);

    // 使用 processEvents 等待，但用 finished 标志判断是否完成
    QTimer timer;
    int elapsed = 0;
    timer.setInterval(50);
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        elapsed += 50;
        if (elapsed >= 10000) {
            LOG_WARN() << "读取媒体元数据超时";
            finished = true;
        }
    });
    timer.start();

    // 轮询等待媒体加载
    while (!finished) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    timer.stop();

    if (loaded && !hasError) {
        const auto metaData = player.metaData();

        if (const auto title = metaData.value(QMediaMetaData::Title); title.isValid()) {
            _name = title.toString();
        } else {
            LOG_WARN() << std::format("{} 读取出错", "歌曲名");
            _name = "出错";
        }

        if (const auto artist = metaData.value(QMediaMetaData::Author); artist.isValid()) {
            _artist = artist.toString();
        } else {
            // todo 读取失败时，从文件名中提取作曲家
            LOG_WARN() << std::format("{} 读取出错", "作曲家");
            _artist = "未知";
        }
        if (const auto album = metaData.value(QMediaMetaData::AlbumTitle); album.isValid()) {
            _album = album.toString();
        } else {
            LOG_WARN() << std::format("{} 读取出错", "所在专辑");
            _album = "未知";
        }
        if (const auto duration = metaData.value(QMediaMetaData::Duration);
            duration.isValid() || duration.toLongLong() > 0) {
            _duration = duration.toLongLong();
        } else {
            LOG_WARN() << std::format("{} 读取出错", "时长");
            _duration = 0;
        }
        LOG_DEBUG() << "歌曲对象创建成功: " << *this;
    } else {
        LOG_WARN() << "无法加载媒体元数据: " << _url.toLocalFile().toStdString();
    }
}

Song::Song(const QUrl& url, const bool isLiked)
    : _id(QUuid::createUuid().toString())
      , _duration()
      , _tagsFlag()
      , _belongingList(isLiked ? static_cast<int>(ExistIn::LIKED_LIST) : 0)
      , _url(url) {
    parseMusicMeta();
}

Song::TagList Song::getTags(const int flag) {
    TagList tags;
    if (flag & VIP) {
        tags.emplace_back("VIP");
    }
    if (flag & SQ) {
        tags.emplace_back("SQ");
    }
    return tags;
}
