#pragma once

#include <random>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaMetaData>
#include <QtMultimedia/QMediaPlayer>

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

enum class PlayStatus {
    STOPPED = QMediaPlayer::StoppedState,
    PLAYING = QMediaPlayer::PlayingState,
    PAUSED = QMediaPlayer::PausedState,
    ERROR
};

class PlayManager : public QObject {
    Q_OBJECT

private:
    explicit PlayManager()
        : _player(new QMediaPlayer(this))
          , _audioOutput(new QAudioOutput(this))
          , _songCtx({nullptr, SongManager::getInstance().getYouMayLikeList()})
          , _playMode(PlayMode::ORDERED), _historyIndex(-1) {
        _player->setAudioOutput(_audioOutput);
        setVolume(50);

        connect(_player, &QMediaPlayer::mediaStatusChanged, this, [this](
                const QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::EndOfMedia) {
                        LOG_DEBUG() << "歌曲播放完毕,自动播放下一首";
                        nextPlay();
                    }
                });
        // 监听播放进度变化
        connect(_player, &QMediaPlayer::positionChanged, this, [this](const LL position) {
            emit positionChanged(position, _player->duration());
        });

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
        if (_historyIndex < 0 || static_cast<int>(_historyIndex >= historyList.size() - 1)) {
            statusManager.showMessage("已经到底了", 1000);
            return;
        }
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

    void play() {
        if (_songCtx.isValid()) {
            play(_songCtx);
        } else {
            StatusManager::getInstance().showMessage("还没有选中歌曲，请先播放一首歌曲");
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

    void setVolume(int value) {
        if (value < 0) {
            value = 0;
        }
        if (value > 100) {
            value = 100;
        }
        _audioOutput->setVolume(1.f * value / 100);
        emit volumeChanged(value);
    }

    [[nodiscard]] int getVolume() const {
        return 100 * _audioOutput->volume();
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

    /**
     * @brief 获取当前播放进度(毫秒)
     */
    [[nodiscard]] LL getPosition() const {
        return _player ? _player->position() : 0;
    }

    /**
     * @brief 获取歌曲总时长(毫秒)
     */
    [[nodiscard]] LL getDuration() const {
        return _player ? _player->duration() : 0;
    }

    /**
     * @brief 获取格式化的播放进度 "mm:ss/mm:ss"
     */
    [[nodiscard]] QString getFormattedProgress() const {
        if (!_player || !_songCtx.song) {
            return "00:00/00:00";
        }

        const LL position = _player->position();
        const LL duration = _player->duration();

        const auto formatTime = [](const LL ms) -> QString {
            const int totalSeconds = static_cast<int>(ms / 1000);
            const int minutes = totalSeconds / 60;
            const int seconds = totalSeconds % 60;
            return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
        };

        return QString("%1/%2").arg(formatTime(position)).arg(formatTime(duration));
    }

    /**
    * @brief 设置播放位置
    * @param position 位置(毫秒)
    */
    void setPosition(const LL position) const {
        if (_player && position >= 0) {
            _player->setPosition(position);
        }
    }


    int getPositionPercent() const {
        return static_cast<int>(100.0 * getPosition() / getDuration());
    }

    /**
     * @brief 获取播放器指针(用于设置播放位置)
     */
    [[nodiscard]] QMediaPlayer* getPlayer() const {
        return _player;
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

        _player->setSource(_songCtx.song->getUrl());
        _player->play();

        emit songPlayed(song);
    }

    [[nodiscard]] PlayStatus getPlayStatus() const {
        if (!_player) {
            LOG_ERROR() << "player is null";
            return PlayStatus::ERROR;
        }
        switch (_player->playbackState()) {
        case QMediaPlayer::StoppedState:
            return PlayStatus::STOPPED;
        case QMediaPlayer::PlayingState:
            return PlayStatus::PLAYING;
        case QMediaPlayer::PausedState:
            return PlayStatus::PAUSED;
        default:
            return PlayStatus::ERROR;
        }
    }

    void pause() const {
        if (_player) {
            _player->pause();
        }
    }

    void start() const {
        if (_player) {
            _player->play();
        }
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
        play();
    }

    void prevPlay() {
        setPrevPlay();
        play();
    }

signals:
    void songPlayed(const SongPtr&);
    void volumeChanged(double volume);
    void positionChanged(qint64 position, qint64 duration);

private:
    QMediaPlayer* _player;
    QAudioOutput* _audioOutput;
    SongContext _songCtx;
    PlayMode _playMode;
    int _historyIndex;
};
