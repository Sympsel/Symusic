#include "entity/Song.h"

#include <QCoreApplication>
#include <QTimer>
#include <QPushButton>
#include <utility>
#include <QDir>
#include <QtMultimedia/QMediaMetaData>
#include <QtMultimedia/QMediaPlayer>

#include <taglib/attachedpictureframe.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>

#include "utils/Log.hpp"
#include "entity/DatabaseManager.h"

/**
 * @brief 获取歌曲元数据，使用 Qt6 的 QMediaPlayer 读取有 bug，故使用了 TagLib 的库，QMediaPlayer 作为备用读取方式
 */
void Song::parseMusicMeta() {
    const QString coverDir = prefix::cacheCoverDir();
    if (!QDir().mkpath(coverDir)) {
        LOG_WARN() << "无法创建缓存目录: " << coverDir;
    }
    if (!_url.isValid()) {
        LOG_WARN() << "无效的URL：" << _url.path();
        return;
    }

    // 先设置默认封面
    _coverPath = prefix::itemImages + "Sympsel.png";

    const QString filePath = _url.toLocalFile();

    // 使用 TagLib 读取元数据
    TagLib::MPEG::File mpegFile(filePath.toStdWString().c_str());

    if (!mpegFile.isOpen()) {
        LOG_WARN() << "无法打开文件: " << filePath.toStdString();
        return;
    }

    // 读取 ID3v2 标签
    if (mpegFile.ID3v2Tag()) {
        const auto id3v2Tag = mpegFile.ID3v2Tag();

        // 读取封面
        if (const auto frameList = id3v2Tag->frameListMap()["APIC"]; !frameList.isEmpty()) {
            if (const auto picFrame = dynamic_cast<const TagLib::ID3v2::AttachedPictureFrame*>(frameList.front())) {
                const QByteArray imageData(
                    (
                        picFrame->picture().data()), static_cast<int>(picFrame->picture().size()
                    ));
                if (QImage image; image.loadFromData(imageData)) {
                    const QString coverPath = coverDir + _id + ".png";
                    if (QFile::exists(coverPath)) {
                        _coverPath = coverPath;
                        LOG_DEBUG() << "封面已存在，跳过保存: " << coverPath.toStdString();
                    } else if (!image.save(coverPath, "PNG")) {
                        LOG_WARN() << "无法保存封面图片到: " << coverPath.toStdString();
                    } else {
                        _coverPath = coverPath;
                        LOG_DEBUG() << "成功保存封面图片到: " << coverPath.toStdString();
                    }
                }
            }
        }

        // 读取标题
        if (!id3v2Tag->title().to8Bit(true).empty()) {
            _name = QString::fromWCharArray(id3v2Tag->title().toCWString());
        }

        // 读取艺术家
        if (!id3v2Tag->artist().to8Bit(true).empty()) {
            _artist = QString::fromWCharArray(id3v2Tag->artist().toCWString());
        }

        // 读取专辑
        if (!id3v2Tag->album().to8Bit(true).empty()) {
            _album = QString::fromWCharArray(id3v2Tag->album().toCWString());
        }
    }

    // 使用 QMediaPlayer 读取时长（TagLib 的时长读取可能不准确）
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

    while (!finished) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    timer.stop();

    if (loaded && !hasError) {
        const auto metaData = player.metaData();

        // 如果 TagLib 没有读取到某些字段，使用 QMediaPlayer 补充
        if (_name.isEmpty()) {
            if (const auto title = metaData.value(QMediaMetaData::Title); title.isValid()) {
                _name = title.toString();
            } else {
                LOG_WARN() << std::format("{} 读取出错", "歌曲名");
                _name = "出错";
            }
        }

        if (_artist.isEmpty()) {
            if (const auto artist = metaData.value(QMediaMetaData::Author); artist.isValid()) {
                _artist = artist.toString();
            } else {
                LOG_WARN() << std::format("{} 读取出错", "作曲家");
                _artist = "未知";
            }
        }

        if (_album.isEmpty()) {
            if (const auto album = metaData.value(QMediaMetaData::AlbumTitle); album.isValid()) {
                _album = album.toString();
            } else {
                LOG_WARN() << std::format("{} 读取出错", "所在专辑");
                _album = "未知";
            }
        }

        // 读取时长
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

Song::Song(QUrl url, const bool isLiked)
    : _id(QUuid::createUuid().toString())
      , _duration()
      , _tagsFlag()
      , _belongingList(isLiked ? static_cast<int>(ExistIn::LIKED_LIST) : 0)
      , _url(std::move(url)) {
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
