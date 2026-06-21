#pragma once

#include <QUuid>
#include <QPixmap>
#include <sstream>
#include <format>

#include <entity/PathManager.hpp>

class Song {
public:
    using TagList = std::vector<QString>;

    enum tag {
        NORMAL = 0,
        VIP = 1,
        SQ = 2
    };

    friend std::ostream& operator<<(std::ostream& os, const Song& song);

    explicit Song(QString name,
                  QString artist,
                  QString album,
                  const QString& filePath,
                  const QString& coverPath,
                  const bool isLiked = false,
                  const int duration = 0,
                  const int tagsFlag = 0)
        : _id(QUuid::createUuid().toString())
          , _name(std::move(name))
          , _artist(std::move(artist))
          , _album(std::move(album))
          , _filePath(prefix::songsFile + filePath)
          , _cover(QPixmap(prefix::itemImages + coverPath))
          , _duration(duration)
          , _isLiked(isLiked)
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
    [[nodiscard]] QString getId() const { return _id; }

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
        return _id == other._id;
    }

    bool operator!=(const Song& other) const {
        return !(*this == other);
    }

private:
    QString _id;
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

template <>
struct std::formatter<Song> : std::formatter<std::string> {
    auto format(const Song& song, auto& ctx) const {
        std::stringstream ss;

        ss << "{id='" << song.getId().toStdString()
            << "'name='" << song.getName().toStdString()
            << "',artist='" << song.getArtist().toStdString()
            << "',album='" << song.getAlbum().toStdString()
            << "',filepath='" << song.getFilePath().toStdString()
            << "',duration=" << song.getDuration() << 's'
            << ",playCount=" << song.getPlayCount()
            << ",tags=[";

        const int tagFlag = song.getTagsFlag();
        bool hasTag = false;
        if (tagFlag & Song::SQ) {
            ss << "SQ";
            hasTag = true;
        }
        if (tagFlag & Song::VIP) {
            if (hasTag) {
                ss << ",";
            }
            ss << "VIP";
        }
        ss << "]}";
        return std::formatter<std::string>::format(ss.str(), ctx);
    }
};

inline std::ostream& operator<<(std::ostream& os, const Song& song) {
    os << std::format("{}", song);
    return os;
}
