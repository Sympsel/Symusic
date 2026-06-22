#pragma once

#include <QMimeDatabase>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "Song.h"
#include "entity/Common.hpp"
#include "utils/Log.hpp"

using SongPtr = std::shared_ptr<Song>;
using SongList = std::vector<SongPtr>;

/**
 * @brief 多向映射管理器，管理 ExistIn、SongList* 和 列表名称 之间的映射关系
 */
class ListMappingManager {
public:
    struct ListInfo {
        Song::ExistIn existIn;
        SongList* list;
        QString name;

        ListInfo(const Song::ExistIn existIn, SongList* list, QString name)
            : existIn(existIn), list(list), name(std::move(name)) {}
    };

    static ListMappingManager& getInstance() {
        static ListMappingManager instance;
        return instance;
    }

    /**
     * @brief 注册列表及其映射关系
     *
     * 建立枚举值、列表指针和名称之间的三向映射关系。
     * 注册后可以通过任意一个键访问其他两个值。
     *
     * @param existIn 列表的枚举标识符，用于唯一标识列表类型
     * @param list 指向歌曲列表的指针，必须是有效的 SongList 对象地址
     * @param name 列表的显示名称，用于 UI 展示和用户交互
     *
     * @note 该方法应在 SongManager 初始化时调用，确保所有列表都被正确注册
     * @warning 同一个枚举值或列表指针不能被重复注册
     *
     * @see getList(Song::ExistIn)
     * @see getExistIn(SongList*)
     * @see getName(Song::ExistIn)
     */
    void registerList(Song::ExistIn existIn, SongList* list, const QString& name) {
        _lists.emplace_back(existIn, list, name);
        const size_t size = _lists.size();
        _mapEnumToIndex[existIn] = static_cast<int>(size) - 1;
        _mapListToIndex[list] = static_cast<int>(size) - 1;
        _mapStringToIndex[name] = static_cast<int>(size) - 1;
    }

    [[nodiscard]] SongList* getList(const Song::ExistIn existIn) const {
        if (const auto it = _mapEnumToIndex.find(existIn); it != _mapEnumToIndex.end()) {
            return _lists[it->second].list;
        }
        return nullptr;
    }

    [[nodiscard]] Song::ExistIn getExistIn(SongList* list) const {
        if (const auto it = _mapListToIndex.find(list); it != _mapListToIndex.end()) {
            return _lists[it->second].existIn;
        }
        throw std::runtime_error("List not found in mapping");
    }

    [[nodiscard]] QString getName(const Song::ExistIn existIn) const {
        if (const auto it = _mapEnumToIndex.find(existIn); it != _mapEnumToIndex.end()) {
            return _lists[it->second].name;
        }
        return {};
    }

    [[nodiscard]] Song::ExistIn getExistIn(const QString& name) const {
        if (const auto it = _mapStringToIndex.find(name); it != _mapStringToIndex.end()) {
            return _lists[it->second].existIn;
        }
        throw std::runtime_error("Name not found in mapping");
    }

    [[nodiscard]] SongList* getList(const QString& name) const {
        if (const auto it = _mapStringToIndex.find(name); it != _mapStringToIndex.end()) {
            return _lists[it->second].list;
        }
        return nullptr;
    }

    [[nodiscard]] QString getName(SongList* list) const {
        if (const auto it = _mapListToIndex.find(list); it != _mapListToIndex.end()) {
            return _lists[it->second].name;
        }
        return {};
    }

    [[nodiscard]] bool contains(SongList* list) const {
        return _mapListToIndex.contains(list);
    }

    [[nodiscard]] bool contains(const QString& name) const {
        return _mapStringToIndex.contains(name);
    }

    [[nodiscard]] const std::vector<ListInfo>& getAllLists() const {
        return _lists;
    }

private:
    std::vector<ListInfo> _lists;
    std::unordered_map<Song::ExistIn, int> _mapEnumToIndex;
    std::unordered_map<SongList*, int> _mapListToIndex;
    std::unordered_map<QString, int> _mapStringToIndex;
};

class SongManager {
private:
    SongManager() {
        // 注册映射关系
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
        auto& mapper = ListMappingManager::getInstance();
        mapper.registerList(Song::ExistIn::RECOMMEND_LIST, &_recommendList, "推荐");
        mapper.registerList(Song::ExistIn::YOU_MAY_LIKE_LIST, &_youMayLikeList, "你可能喜欢");
        mapper.registerList(Song::ExistIn::LIKED_LIST, &_likedList, "我喜欢的");
        mapper.registerList(Song::ExistIn::DOWNLOAD_LIST, &_downloadList, "本地下载");
        mapper.registerList(Song::ExistIn::HISTORY_LIST, &_historyList, "最近播放");
    }

    /**
    * @brief 获取列表对应的枚举值
    */
    [[nodiscard]] static Song::ExistIn getListEnum(SongList* list) {
        return ListMappingManager::getInstance().getExistIn(list);
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

    [[nodiscard]] SongList& getHistoryList() {
        return _historyList;
    }

    [[nodiscard]] std::optional<SongPtr> findSong(const QString& id) {
        if (_findCache.contains(id)) {
            return _findCache[id];
        }
        for (const auto& allLists = ListMappingManager::getInstance().getAllLists();
             const auto& listInfo : allLists) {
            for (const auto& song : *(listInfo.list)) {
                if (song->getId() == id) {
                    _findCache[id] = song;
                    return song;
                }
            }
        }
        return std::nullopt;
    }

    static bool append(SongList& which, const SongPtr& song) {
        if (!ListMappingManager::getInstance().contains(&which)) {
            LOG_ERROR() << "have no reflect";
            return false;
        }
        if (const auto listEnum = getListEnum(&which);
            !(song->getBelongStatus() & static_cast<int>(listEnum))) {
            which.emplace_back(song);
            const auto newStatus = song->getBelongStatus() | static_cast<int>(listEnum);
            song->setBelongStatus(static_cast<Song::ExistIn>(newStatus));
            return true;
        }
        return false;
    }

    static bool append(SongList& which, const QList<QUrl>& urls) {
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

    static bool remove(SongList& which, const QString& id) {
        if (!ListMappingManager::getInstance().contains(&which)) {
            LOG_ERROR() << "have no reflect";
            return false;
        }

        if (const auto it =
            std::ranges::find_if(which, [&id](const SongPtr& song) {
                return song->getId() == id;
            }); it != which.end()) {
            const auto currStatus = (*it)->getBelongStatus();
            const auto listStatus = static_cast<int>(getListEnum(&which));
            const auto newStatus = currStatus & ~listStatus;
            (*it)->setBelongStatus(static_cast<Song::ExistIn>(newStatus));

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
    SongList _downloadList;
    SongList _historyList;

    std::unordered_map<QString, SongPtr> _findCache{};
};
