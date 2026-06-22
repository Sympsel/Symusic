#pragma once

#include <QMimeDatabase>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "Song.h"
#include "entity/Common.hpp"
#include "utils/Log.hpp"

using SongPtr = std::shared_ptr<Song>;

class SongManager {
public:
    using SongList = std::vector<SongPtr>;

private:
    SongManager() {
        mapTo();
        // 初始化数据
        initData();
    }

    // todo 改为从数据库读取
    void initData();

    /**
     * @brief 映射枚举值到对应的列表，并反向映射
     */
    void mapTo() {
        _mapOfES[Song::ExistIn::RECOMMEND_LIST] = &_recommendList;
        _mapOfES[Song::ExistIn::YOU_MAY_LIKE_LIST] = &_youMayLikeList;
        _mapOfES[Song::ExistIn::LIKED_LIST] = &_likedList;
        _mapOfES[Song::ExistIn::DOWNLOAD_LIST] = &_downloadList;

        _mapOfSE[&_recommendList] = Song::ExistIn::RECOMMEND_LIST;
        _mapOfSE[&_youMayLikeList] = Song::ExistIn::YOU_MAY_LIKE_LIST;
        _mapOfSE[&_likedList] = Song::ExistIn::LIKED_LIST;
        _mapOfSE[&_downloadList] = Song::ExistIn::DOWNLOAD_LIST;
    }

public:
    static SongManager& getInstance() {
        static SongManager instance;
        return instance;
    }

    [[nodiscard]] SongList& getRecommendList() {
        return _recommendList;
    }

    [[nodiscard]] SongList& getYouMayLikeList() {
        return _youMayLikeList;
    }

    [[nodiscard]] SongList& getLikedList() {
        return _likedList;
    }

    [[nodiscard]] SongList& getDownloadList() {
        return _downloadList;
    }

    [[nodiscard]] std::optional<SongPtr> findSong(const QString& id) {
        if (_findCache.contains(id)) {
            return _findCache[id];
        }
        for (const auto& list : _mapOfSE | std::views::keys) {
            for (auto& song : *list) {
                if (song->getId() == id) {
                    _findCache[id] = song;
                    return song;
                }
            }
        }
        return std::nullopt;
    }

    bool append(SongList& which, const SongPtr& song) {
        if (!_mapOfSE.contains(&which)) {
            LOG_ERROR() << "have no reflect";
            return false;
        }
        if (!contains(which, song->getBelongStatus())) {
            which.emplace_back(song);
            const auto newStatus = song->getBelongStatus() | static_cast<int>(_mapOfSE[&which]);
            song->setBelongStatus(static_cast<Song::ExistIn>(newStatus));
            return true;
        }
        return false;
    }

    bool append(SongList& which, const QList<QUrl>& urls) {
        for (const auto& url : urls) {
            QMimeDatabase mineDb;
            QMimeType mimeType = mineDb.mimeTypeForName(url.toLocalFile());
            QString mime = mimeType.name();
            if (const auto& supportedList = SupportSongType::getInstance().getList();
                !supportedList.contains(mime)) {
                continue;
            }
            const auto song = std::make_shared<Song>(url);
            append(which, song);
        }
        return false;
    }

    bool remove(SongList& which, const QString& id) {
        if (!_mapOfSE.contains(&which)) {
            LOG_ERROR() << "have no reflect";
            return false;
        }

        if (const auto it =
            std::ranges::find_if(which, [&id](const SongPtr& song) {
                return song->getId() == id;
            }); it != which.end()) {
            // 从歌曲的 belongingList 中移除当前列表的状态
            const auto currStatus = (*it)->getBelongStatus();
            const auto listStatus = static_cast<int>(_mapOfSE[&which]);
            const auto newStatus = currStatus & ~listStatus;
            (*it)->setBelongStatus(static_cast<Song::ExistIn>(newStatus));

            which.erase(it);
            return true;
        }
        return false;
    }

    bool contains(SongList& list, const int belongStatus) {
        if (!_mapOfSE.contains(&list)) {
            LOG_ERROR() << "have no reflect";
        }
        return static_cast<int>(_mapOfSE[&list]) & belongStatus;
    }

    void clearCache() {
        _findCache.clear();
    }

private:
    SongList _recommendList;
    SongList _youMayLikeList;

    SongList _likedList;
    SongList _downloadList;

    std::unordered_map<QString, SongPtr> _findCache{};
    std::unordered_map<Song::ExistIn, SongList*> _mapOfES{};
    std::unordered_map<SongList*, Song::ExistIn> _mapOfSE{};
};
