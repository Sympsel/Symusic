#pragma once

#include <QThread>
#include <QTimer>
#include <random>
#include <QtMultimedia/QAudioDevice>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaDevices>
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
          , _audioOutput(nullptr)
          , _deviceCheckTimer(new QTimer(this))
          , _lastDeviceName("")
          , _songCtx({nullptr, SongManager::getInstance().getYouMayLikeList()})
          , _playMode(PlayMode::ORDERED), _historyIndex(-1) {
        initAudioOutput();
        setVolume(50);

        connect(_player, &QMediaPlayer::mediaStatusChanged, this, [this](
                const QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::EndOfMedia) {
                        LOG_DEBUG() << "歌曲播放完毕,自动播放下一首";
                        nextPlay();
                    }
                });

        connect(_player, &QMediaPlayer::errorOccurred, this, [this](
                const QMediaPlayer::Error error, const QString& errorString) {
                    LOG_ERROR() << "播放器错误:" << errorString << "错误码:" << static_cast<int>(error);

                    if (error == QMediaPlayer::ResourceError) {
                        LOG_WARN() << "资源错误，暂停播放";
                        _player->pause();

                        StatusManager::getInstance().showMessage("音频设备已断开，播放已暂停", 2000);
                    }
                });

        connect(_player, &QMediaPlayer::positionChanged, this, [this](const LL position) {
            emit positionChanged(position, _player->duration());
        });

        connect(_deviceCheckTimer, &QTimer::timeout, this, [this]() {
            checkAndRefreshAudioOutput();
        });
        _deviceCheckTimer->start(1000);
    }

    void initAudioOutput() {
        delete _audioOutput;
        if (const QAudioDevice defaultDevice = QMediaDevices::defaultAudioOutput(); defaultDevice.isNull()) {
            LOG_WARN() << "默认音频输出设备为空，使用默认构造函数";
            _audioOutput = new QAudioOutput(this);
        } else {
            LOG_INFO() << "初始化音频输出设备:" << defaultDevice.description();
            _audioOutput = new QAudioOutput(defaultDevice, this);
            _lastDeviceName = defaultDevice.description();
        }
        _player->setAudioOutput(_audioOutput);
    }

    void checkAndRefreshAudioOutput() {
        const QAudioDevice currentDevice = QMediaDevices::defaultAudioOutput();
        if (currentDevice.isNull()) {
            return;
        }

        if (const QString currentDeviceName = currentDevice.description(); _lastDeviceName != currentDeviceName) {
            LOG_INFO() << "检测到音频设备变化:" << _lastDeviceName << "->" << currentDeviceName;

            if (_player->playbackState() == QMediaPlayer::PlayingState) {
                LOG_INFO() << "音频设备切换，暂停播放并重新初始化";
                _player->pause();

                initAudioOutput();
                setVolume(getVolume());

                emit playStateChanged();

                QString message = std::format("已从 {} 切换到 {}，播放已暂停",
                                              _lastDeviceName.toStdString(),
                                              currentDeviceName.toStdString()).c_str();
                StatusManager::getInstance().showMessage(message, 2000);
            } else {
                LOG_INFO() << "非播放状态下设备切换，仅更新音频输出";
                initAudioOutput();
                setVolume(getVolume());

                const QString message = std::format("已从 {} 切换到 {}",
                                                    _lastDeviceName.toStdString(),
                                                    currentDeviceName.toStdString()).c_str();
                StatusManager::getInstance().showMessage(message, 2000);
            }

            _lastDeviceName = currentDeviceName;
            return;
        }

        if (!_audioOutput) {
            LOG_INFO() << "音频输出未初始化，需要刷新";
            initAudioOutput();
            setVolume(getVolume());
        }
    }

    void recoverFromError() {
        LOG_INFO() << "重新初始化音频系统";
        _player->stop();

        QThread::msleep(100);
        initAudioOutput();
        setVolume(getVolume());

        if (_songCtx.isValid()) {
            LOG_INFO() << "尝试重新加载当前歌曲";
            _player->setSource(_songCtx.song->getUrl());
            _player->setPosition(_player->position());

            QTimer::singleShot(200, this, [this]() {
                _player->play();
                LOG_INFO() << "重新开始播放";
            });
        }
    }

    void setPrevPlay() {
        auto& songManager = SongManager::getInstance();
        const auto& historyList = songManager.getHistoryList();

        auto& statusManager = StatusManager::getInstance();
        if (historyList.empty()) {
            statusManager.showMessage("还没有播放过歌曲，请先播放一首歌曲");
            return;
        }

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
        if (!_audioOutput) {
            LOG_WARN() << "音频输出未初始化";
            return;
        }
        _audioOutput->setVolume(static_cast<float>(value) / 100);
        emit volumeChanged(value);
    }

    [[nodiscard]] int getVolume() const {
        return _audioOutput ? static_cast<int>(100 * _audioOutput->volume()) : 0;
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

    [[nodiscard]] LL getPosition() const {
        return _player ? _player->position() : 0;
    }

    [[nodiscard]] LL getDuration() const {
        return _player ? _player->duration() : 0;
    }

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

        return QString("%1/%2").arg(formatTime(position), formatTime(duration));
    }

    void setPosition(const LL position) const {
        if (_player && position >= 0) {
            _player->setPosition(position);
        }
    }


    [[nodiscard]] int getPositionPercent() const {
        return static_cast<int>(100 * getPosition() / getDuration());
    }

    [[nodiscard]] QMediaPlayer* getPlayer() const {
        return _player;
    }

    void play(const SongContext& songCtx) {
        initAudioOutput();

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
        _historyIndex = 0;
        song->incrementPlayCount();

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
    void playStateChanged();

private:
    QMediaPlayer* _player;
    QAudioOutput* _audioOutput;
    QTimer* _deviceCheckTimer;
    QString _lastDeviceName;
    SongContext _songCtx;
    PlayMode _playMode;
    int _historyIndex;
};
