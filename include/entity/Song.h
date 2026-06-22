#pragma once

#include <QUuid>
#include <QUrl>
#include <QPixmap>
#include <sstream>
#include <format>

#include "entity/Common.hpp"

class Song {
public:
    using TagList = std::vector<QString>;

    enum class ExistIn {
        RECOMMEND_LIST = 0,
        YOU_MAY_LIKE_LIST = 1,
        LIKED_LIST = 2,
        DOWNLOAD_LIST = 4
    };

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
          , _tagsFlag(tagsFlag)
          , _belongingList(isLiked ? static_cast<int>(ExistIn::LIKED_LIST) : 0) {
    }

    explicit Song(const QUrl& url)
        : _id(QUuid::createUuid().toString())
          , _filePath(url.toLocalFile())
          , _duration(0)
          , _tagsFlag(0)
          , _belongingList(0) {
        // todo
    }

    [[nodiscard]] static TagList getTags(int flag);

    [[nodiscard]] QString getName() const { return _name; }
    [[nodiscard]] QString getArtist() const { return _artist; }
    [[nodiscard]] QString getAlbum() const { return _album; }
    [[nodiscard]] QString getFilePath() const { return _filePath; }
    [[nodiscard]] QPixmap getCover() const { return _cover; }
    [[nodiscard]] int getDuration() const { return _duration; }
    [[nodiscard]] bool isLiked() const { return isInList(ExistIn::LIKED_LIST); }

    [[nodiscard]] int getPlayCount() const { return _playCount; }
    [[nodiscard]] QString getId() const { return _id; }
    [[nodiscard]] int getTagsFlag() const { return _tagsFlag; }
    [[nodiscard]] int getBelongStatus() const { return _belongingList; }

    void setLiked(const bool liked) {
        if (liked) {
            _belongingList |= static_cast<int>(ExistIn::LIKED_LIST);
        } else {
            _belongingList &= ~static_cast<int>(ExistIn::LIKED_LIST);
        }
    }

    /**
     * @brief 设置歌曲所属的列表状态
     * @param belongingList 歌曲所属的列表
     */
    void setBelongStatus(const ExistIn belongingList) {
        _belongingList = static_cast<int>(belongingList);
    }

    void setBelongStatus(const std::initializer_list<ExistIn>& belongingLists) {
        for (const auto& belongingList : belongingLists) {
            _belongingList |= static_cast<int>(belongingList);
        }
    }

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

    [[nodiscard]] bool isInList(ExistIn list) const {
        return (_belongingList & static_cast<int>(list)) != 0;
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
    int _playCount = 0;
    // 用于标识歌曲vip,音质啥的
    int _tagsFlag;
    // 所属列表
    int _belongingList;

    // todo 未来打算添加的字段 发行时间
};

template <>
struct std::formatter<Song> : std::formatter<std::string> {
    auto format(const Song& song, auto& ctx) const {
        std::stringstream ss;

        ss << "{id=" << song.getId().toStdString()
            << ",name='" << song.getName().toStdString()
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
