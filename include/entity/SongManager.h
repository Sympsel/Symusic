#pragma once

#include <vector>
#include <unordered_map>

#include "Song.h"

using SongPtr = std::shared_ptr<Song>;

class SongManager {
public:
    using SongList = std::vector<SongPtr>;

private:
    SongManager() {
        // 初始化数据
        initData();
    }

    // todo 改为从数据库读取
    void initData();

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

    [[nodiscard]] std::optional<Song*> findSongById(const QString& id) {
        if (_findCache.contains(id)) {
            return _findCache[id];
        }
        for (auto& list : {_recommendList, _youMayLikeList, _likedList}) {
            for (auto& song : list) {
                if (song->getId() == id) {
                    _findCache[id] = song.get();
                    return song.get();
                }
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] static std::optional<Song*> findSongInList(const SongList& which, const QString& id) {
        for (auto& song : which) {
            if (song->getId() == id) {
                return song.get();
            }
        }
        return std::nullopt;
    }

   static bool append(SongList& which, const SongPtr& song) {
        if (std::ranges::find(which, song) == which.end()) {
            which.emplace_back(song);
            return true;
        }
        return false;
    }

    static bool append(SongList& which, SongPtr&& song) {
        if (std::ranges::find(which, song) == which.end()) {
            which.emplace_back(std::move(song));
            return true;
        }
        return false;
    }

    static int append(SongList& which, const std::initializer_list<SongPtr>& songs) {
        int addedCount = 0;
        for (const auto& song : songs) {
            if (append(which, song)) {
                ++addedCount;
            }
        }
        return addedCount;
    }

    static bool removeById(SongList& which, const QString& id) {
        if (const auto it = std::ranges::find_if(which, [&id](const SongPtr& song) {
            return song->getId() == id;
        }); it != which.end()) {
            which.erase(it);
            return true;
        }
        return false;
    }

    void clearCache() {
        _findCache.clear();
    }

private:
    SongList _recommendList;
    SongList _youMayLikeList;

    SongList _likedList;

    std::unordered_map<QString, Song*> _findCache{};
};
