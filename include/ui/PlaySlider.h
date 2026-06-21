#pragma once

#include <Qslider>
#include <QWidget>

#include "entity/Song.h"

class PlaySlider : public QSlider {
    Q_OBJECT
private:
    explicit PlaySlider(QWidget* parent = nullptr)
       : QSlider(Qt::Horizontal, parent)
         , _song(nullptr) {
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

    void setSongToPlay(Song* song) {
        _song = song;
    }

private:
    Song* _song;
};
