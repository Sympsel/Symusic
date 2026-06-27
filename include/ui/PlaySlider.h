#pragma once

#include <QSlider>
#include <QWidget>

#include "entity/PlayManager.hpp"

class PlaySlider : public QSlider {
    Q_OBJECT

private:
    explicit PlaySlider(QWidget* parent = nullptr);

public:
    PlaySlider(const PlaySlider&) = delete;
    PlaySlider(PlaySlider&&) = delete;
    PlaySlider& operator=(const PlaySlider&) = delete;
    PlaySlider& operator=(PlaySlider&&) = delete;

    /**
     * @param parent the parent will be fixed all the time
     * @return an instance of this
     */
    static PlaySlider* getInstance(QWidget* parent = nullptr) {
        static auto slider = new PlaySlider(parent);
        return slider;
    }
private:
    bool _isDragging{};
};
