#include "entity/SongManager.h"

#include <random>

SongManager::SongManager() {
    // 注册映射关系
    mapTo();
    // 初始化数据
    DatabaseManager::getInstance().loadData(*this);
    // 如果数据库没有数据，则生成假数据用作测试
    if (packUp().empty()) {
        initData();
    }
}

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
            id + ".png",
            false,
            198000,
            Song::tag::VIP | Song::tag::SQ
        ));
    }

    for (size_t i{indexs.size() / 2}; i < indexs.size(); ++i) {
        auto id = QString(std::to_string(indexs[i]).c_str());
        _youMayLikeList.emplace_back(std::make_shared<Song>(
            "歌曲" + id, "作曲家" + id, "专辑" + id,
            id + ".png",
            false,
            198000,
            Song::tag::VIP | Song::tag::SQ
        ));
    }

    for (size_t i{}; i < indexs.size() / 2; ++i) {
        auto id = QString(std::to_string(indexs[i]).c_str());
        _likedList.emplace_back(std::make_shared<Song>(
            "歌曲" + id, "作曲家" + id, "专辑" + id,
            id + ".png",
            true,
            198000,
            Song::tag::VIP | Song::tag::SQ
        ));
    }
}
