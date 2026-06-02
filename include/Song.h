#pragma once

#include <QPixmap>
#include <PathMaganger.hpp>

class Song {
public:
    using TagList = std::vector<QString>;

    enum tag {
        NORMAL = 0,
        VIP = 1,
        SQ = 2
    };

    explicit Song(QString name,
                  QString artist,
                  QString album,
                  const QString& filePath,
                  const QString& coverPath,
                  const int duration = 0,
                  const int tagsFlag = 0)
        : _name(std::move(name))
          , _artist(std::move(artist))
          , _album(std::move(album))
          , _filePath(prefix::songsFile + filePath)
          , _cover(QPixmap(prefix::itemImages + coverPath))
          , _duration(duration)
          , _tagsFlag(tagsFlag) {
    }

    [[nodiscard]] static TagList getTags(int flag);

    [[nodiscard]] QString getName() const { return _name; }
    [[nodiscard]] QString getArtist() const { return _artist; }
    [[nodiscard]] QString getAlbum() const { return _album; }
    [[nodiscard]] QString getFilePath() const { return _filePath; }
    [[nodiscard]] QPixmap getCover() const { return _cover; }
    [[nodiscard]] int getDuration() const { return _duration; }
    [[nodiscard]] bool isLiked() const { return _isLiked; }
    [[nodiscard]] int getPlayCount() const { return _playCount; }

    [[nodiscard]] int getTagsFlag() const {
        return _tagsFlag;
    }

    void setLiked(const bool liked) { _isLiked = liked; }
    void incrementPlayCount() { _playCount++; }
    void setCover(const QPixmap& cover) { _cover = cover; }

    [[nodiscard]] QString getFormattedDuration() const {
        const int minutes = _duration / 60;
        const int seconds = _duration % 60;
        return QString("%1:%2").
               arg(minutes, 2, 10, '0').
               arg(seconds, 2, 10, '0');
    }

    [[nodiscard]] TagList getTags() const {
        return getTags(_tagsFlag);
    }

    bool operator==(const Song& other) const {
        // todo 补全歌曲属性
        return _name == other._name && _artist == other._artist;
    }

    bool operator!=(const Song& other) const {
        return !(*this == other);
    }

private:
    QString _name;
    QString _artist;
    QString _album;
    QString _filePath;
    QPixmap _cover;
    int _duration;
    bool _isLiked = false;
    int _playCount = 0;
    // 用于标识歌曲vip,音质啥的
    int _tagsFlag;

    // todo 未来打算添加的字段 发行时间
};
