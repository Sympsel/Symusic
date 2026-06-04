#pragma once

#include <vector>

#include "Song.h"

class SongManager {
public:
    using SongList = std::vector<Song>;

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

    static bool append(SongList& which, const Song& song) {
        if (std::find(which.begin(), which.end(), song) != which.end()) {
            which.emplace_back(song);
            return true;
        }
        return false;
    }

    static int append(SongList& which, const std::initializer_list<Song>& songs) {
        bool allAdded = true;
        for (const auto& song : songs) {
            allAdded &= append(which, song);
        }
        return allAdded;
    }

private:
    SongList _recommendList;
    SongList _youMayLikeList;

    SongList _likedList;
};
