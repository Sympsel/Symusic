#include "entity/Song.h"

#include <QTimer>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaMetaData>

void Song::parseMusicMeta() {
    // 检查URL是否有效
    if (!_url.isValid()) {
        return;
    }

    // 创建媒体播放对象（使用堆分配，避免立即销毁）
    auto* player = new QMediaPlayer();
    player->setSource(_url);

    // 使用事件循环等待媒体可用，设置超时
    QEventLoop loop;
    bool isReady = false;

    // 连接信号，当播放器状态改变时退出循环
    QObject::connect(player, &QMediaPlayer::mediaStatusChanged,
                     [&, player]() {
                         if (player && player->isAvailable()) {
                             isReady = true;
                             loop.quit();
                         }
                     });

    // 设置5秒超时
    QTimer::singleShot(5000, [&]() {
        if (!isReady) {
            loop.quit();
        }
    });

    // 执行事件循环（最多等待5秒）
    loop.exec();

    // 如果成功获取到元数据，则读取
    if (isReady && player->isAvailable()) {
        const auto metaData = player->metaData();

        // 读取标题
        if (const auto title = metaData.value(QMediaMetaData::Title); title.isValid()) {
            _name = title.toString();
        }
        // 读取艺术家
        if (const auto artist = metaData.value(QMediaMetaData::Author); artist.isValid()) {
            _artist = artist.toString();
        }
        // 读取专辑
        if (const auto album = metaData.value(QMediaMetaData::AlbumTitle); album.isValid()) {
            _album = album.toString();
        }
        // 读取时长（毫秒）
        if (const auto duration = metaData.value(QMediaMetaData::Duration); duration.isValid()) {
            _duration = duration.toLongLong();
        }
    }

    // 清理播放器 - 使用 deleteLater 确保信号处理完成后再删除
    player->deleteLater();
}

Song::Song(const QUrl& url, const bool isLiked)
    : _id(QUuid::createUuid().toString())
      , _duration(0)
      , _tagsFlag(0)
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
