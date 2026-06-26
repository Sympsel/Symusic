#pragma once

#include <random>

#include "Common.hpp"
#include "SongManager.h"
#include "StatusManager.hpp"

/**
 * @brief 歌曲上下文，记录触发播放逻辑时的歌曲和所在列表
 */
struct SongContext {
    SongPtr song;
    SongList* list;

    SongContext(SongPtr song, SongList& listRef)
        : song(std::move(song)), list(&listRef) {}

    [[nodiscard]] bool isValid() const {
        return song && list;
    }
};

class PlayManager : public QObject {
    Q_OBJECT

private:
    explicit PlayManager()
        : _isPlaying()
          , _songCtx({nullptr, SongManager::getInstance().getYouMayLikeList()})
          , _playMode(PlayMode::ORDERED), _historyIndex(-1) {
    }

    void setPrevPlay() {
        auto& songManager = SongManager::getInstance();
        const auto& historyList = songManager.getHistoryList();

        auto& statusManager = StatusManager::getInstance();
        if (historyList.empty()) {
            statusManager.showMessage("还没有播放过歌曲，请先播放一首歌曲");
            return;
        }

        // 如果当前歌曲在历史记录中，向后移动
        if (_historyIndex >= 0 && _historyIndex < static_cast<int>(historyList.size()) - 1) {
            ++_historyIndex;
            _songCtx.song = historyList[_historyIndex];
            statusManager.showMessage(
                std::format("上一首: {}", _songCtx.song->getName().toStdString()).c_str()
            );
        } else if (_historyIndex == -1) {
            // 当前歌曲不在历史记录中，跳转到第一首
            _historyIndex = 0;
            _songCtx.song = historyList.front();
            statusManager.showMessage(
                std::format("上一首: {}", _songCtx.song->getName().toStdString()).c_str()
            );
        } else {
            statusManager.showMessage("已经到底了", 1000);
        }
    }

    void setNextPlay() {
        auto& [song, list] = _songCtx;
        switch (_playMode) {
        case PlayMode::ORDERED:
            if (list->empty()) {
                return;
            }
            if (const auto curr = std::ranges::find(*list, song);
                curr != list->end()) {
                if (curr + 1 == list->end()) {
                    song = list->front();
                } else {
                    song = *(curr + 1);
                }
            } else {
                StatusManager::getInstance().showMessage("还没有选中歌曲，请先播放一首歌曲");
            }
            break;
        case PlayMode::RANDOMED:
            if (list->empty()) {
                return;
            }
            static std::mt19937 gen(std::random_device{}());
            std::ranges::sample(*list, &song, 1, gen);
            break;
        case PlayMode::SINGLE_LOOPING:
            if (song) {
                if (!list->empty()) {
                    song = list->front();
                }
            }
            break;
        }
    }

public:
    PlayManager(const PlayManager&) = delete;
    PlayManager(PlayManager&&) = delete;
    PlayManager& operator=(const PlayManager&) = delete;
    PlayManager& operator=(PlayManager&&) = delete;

    static PlayManager& getInstance() {
        static PlayManager instance;
        return instance;
    }

    void setPlayMode(const PlayMode& playMode) {
        _playMode = playMode;
    }

    void setSongList(SongList& songList) {
        if (auto& [song, list] = _songCtx; list != &songList) {
            list = &songList;
            song = nullptr;
            nextPlay();
        }
    }

    [[nodiscard]] const PlayMode& getPlayMode() const {
        return _playMode;
    }

    [[nodiscard]] const SongPtr& getCurrPlay() const {
        return _songCtx.song;
    }

    // 播放相关
    void play(const SongContext& songCtx) {
        _songCtx = songCtx;
        auto& [song, list] = _songCtx;
        const auto& listMappingManager = ListMappingManager::getInstance();
        StatusManager::getInstance().showMessage(
            std::format("正在播放: {}", song->getName().toStdString()).c_str()
        );
        LOG_DEBUG() << std::format("当前播放歌曲 {}，所在歌单 {}",
                                   song->getName().toStdString(),
                                   listMappingManager.getName(list).toStdString());
        auto& songManager = SongManager::getInstance();
        songManager.appendToHistoryList(song);
        // 播放歌曲后重置历史索引
        _historyIndex = 0;
        song->incrementPlayCount();
        start();
        emit songPlayed(song);
    }

    [[nodiscard]] bool getPlayStatus() const {
        return _isPlaying;
    }

    void pause() {
        _isPlaying = false;
    }

    void start() {
        _isPlaying = true;
    }

    // 用于漫游模式
    // void play(const SongPtr& song, const std::initializer_list<SongList&>& playLists) {
    // todo
    // _song = song;
    // _songList = &playList;
    // auto& songManager = SongManager::getInstance();
    // songManager.appendToHistoryList(_song);
    // _song->incrementPlayCount();
    // emit songPlayed(_song);
    // }

    void nextPlay() {
        setNextPlay();
    }

    void prevPlay() {
        setPrevPlay();
    }

signals:
    void songPlayed(const SongPtr&);

private:
    bool _isPlaying;
    SongContext _songCtx;
    PlayMode _playMode;
    int _historyIndex;
};
