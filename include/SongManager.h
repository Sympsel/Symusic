#pragma once

#include <vector>
#include <random>

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
    void initData() {
        constexpr int maxIndex = 35;
        std::vector<int> indexs;
        for (int i{1}; i <= maxIndex; ++i) {
            indexs.emplace_back(i);
        }

        // 提供随机种子
        std::random_device rd;
        // 创建一个随机数发生器
        std::mt19937 g(rd());
        // 洗牌算法
        std::shuffle(indexs.begin(), indexs.end(), g);
        for (size_t i{}; i < indexs.size() / 2; ++i) {
            auto id = QString(std::to_string(indexs[i]).c_str());
            _recommendList.emplace_back(
                "歌曲" + id, "作曲家" + id, "专辑" + id,
                "暂无",
                id + ".png",
                198,
                Song::tag::VIP | Song::tag::SQ
            );
        }

        for (size_t i{indexs.size() / 2}; i < indexs.size(); ++i) {
            auto id = QString(std::to_string(indexs[i]).c_str());
            _youMayLikeList.emplace_back(
                "歌曲" + id, "作曲家" + id, "专辑" + id,
                "暂无",
                id + ".png",
                198,
                Song::tag::VIP | Song::tag::SQ
            );
        }

        for (size_t i{}; i < indexs.size() / 2; ++i) {
            auto id = QString(std::to_string(indexs[i]).c_str());
            _likedList.emplace_back(
                "歌曲" + id, "作曲家" + id, "专辑" + id,
                "暂无",
                id + ".png",
                198,
                Song::tag::VIP | Song::tag::SQ
            );
        }
    }

public:
    static SongManager& getInstance() {
        static SongManager instance;
        return instance;
    }

    [[nodiscard]] const SongList& getRecommendList() const {
        return _recommendList;
    }

    [[nodiscard]] const SongList& getYouMayLikeList() const {
        return _youMayLikeList;
    }

    [[nodiscard]] const SongList& getLikedList() const {
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
