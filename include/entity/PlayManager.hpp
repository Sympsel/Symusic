#pragma once

#include <random>

#include "Common.hpp"
#include "SongManager.h"

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
        : _songList(nullptr)
          , _playMode(PlayMode::ORDERED) {
    }

    void setPrevPlay() {
        if (const auto& historyList = SongManager::getInstance().getHistoryList();
            historyList.empty()) {
            /* do nothing */
            LOG_DEBUG() << "have no play a song yet";
        } else {
            _song = *historyList.begin();
        }
    }

    void setNextPlay() {
        switch (_playMode) {
        case PlayMode::ORDERED:
            if (_songList->empty()) {
                return;
            }
            if (const auto curr = std::ranges::find(*_songList, _song);
                curr != _songList->end()) {
                if (curr + 1 == _songList->end()) {
                    _song = *_songList->begin();
                } else {
                    _song = *(curr + 1);
                }
            } else {
                LOG_ERROR() << "当前歌曲不在播放列表中";
            }
            break;
        case PlayMode::RANDOMED:
            if (_songList->empty()) {
                return;
            }
            static std::mt19937 gen(std::random_device{}());
            std::ranges::sample(*_songList, &_song, 1, gen);
            break;
        case PlayMode::SINGLE_LOOPING:
            if (_song) {
                if (!_songList->empty()) {
                    _song = *_songList->begin();
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
        if (_songList != &songList) {
            _songList = &songList;
            _song = nullptr;
            nextPlay();
        }
    }

    [[nodiscard]] const PlayMode& getPlayMode() const {
        return _playMode;
    }

    // 播放相关
    void play(const SongPtr& song, SongList& playList) {
        _song = song;
        _songList = &playList;
        const auto& listMappingManager = ListMappingManager::getInstance();
        LOG_DEBUG() << std::format("当前播放歌曲 {}，所在歌单 {}",
                                   _song->getName().toStdString(),
                                   listMappingManager.getName(&playList).toStdString());
        auto& songManager = SongManager::getInstance();
        songManager.appendToHistoryList(_song);
        _song->incrementPlayCount();
        emit songPlayed(_song);
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
    // 当前正在播放的歌曲
    SongPtr _song;
    // 当前正在播放的列表
    SongList* _songList;
    PlayMode _playMode;
};
