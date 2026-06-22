#pragma once

#include <QSlider>
#include <QWidget>

#include "entity/SongManager.h"
#include "utils/Log.hpp"

class PlaySlider : public QSlider {
    Q_OBJECT

private:
    explicit PlaySlider(QWidget* parent = nullptr)
        : QSlider(Qt::Horizontal, parent)
          , _totalDuration()
          , _currDuration() {
        this->setRange(0, 100);
    }

public:
    /**
     *
     * @param parent the parent will be fixed all the time
     * @return an instance of this
     */
    static PlaySlider* getInstance(QWidget* parent = nullptr) {
        static auto slider = new PlaySlider(parent);
        return slider;
    }

    void setSongToPlay(const SongPtr& song) {
        _totalDuration = song->getDuration();
        if (_totalDuration <= 0) {
            LOG_ERROR() << std::format("错误的歌曲总时长：{}", _totalDuration);
        }
    }


    void setValueByTime(int second);

    void setProcessByPercent(int percent);

private:
    QString _songId{};
    int _totalDuration;
    int _currDuration;
};
