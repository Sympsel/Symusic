#include "entity/SongManager.h"

#include <random>

void SongManager::initData() {
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
    std::ranges::shuffle(indexs, g);
    for (size_t i{}; i < indexs.size() / 2; ++i) {
        auto id = QString(std::to_string(indexs[i]).c_str());
        _recommendList.emplace_back(std::make_shared<Song>(
            "歌曲" + id, "作曲家" + id, "专辑" + id,
            "暂无",
            id + ".png",
            false,
            198,
            Song::tag::VIP | Song::tag::SQ
        ));
    }

    for (size_t i{indexs.size() / 2}; i < indexs.size(); ++i) {
        auto id = QString(std::to_string(indexs[i]).c_str());
        _youMayLikeList.emplace_back(std::make_shared<Song>(
            "歌曲" + id, "作曲家" + id, "专辑" + id,
            "暂无",
            id + ".png",
            false,
            198,
            Song::tag::VIP | Song::tag::SQ
        ));
    }

    for (size_t i{}; i < indexs.size() / 2; ++i) {
        auto id = QString(std::to_string(indexs[i]).c_str());
        _likedList.emplace_back(std::make_shared<Song>(
            "歌曲" + id, "作曲家" + id, "专辑" + id,
            "暂无",
            id + ".png",
            true,
            198,
            Song::tag::VIP | Song::tag::SQ
        ));
    }
}
