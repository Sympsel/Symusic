#pragma once

#include <QSlider>
#include <QLabel>

#include <entity/Color.hpp>
#include "utils/Sync.hpp"

class VolumeSlider : public QWidget {
    Q_OBJECT

private:
    explicit VolumeSlider(QWidget* parent = nullptr);

    bool isQuiet() const {
        return _quiet;
    }

public:
    static VolumeSlider* getInstance(QWidget* parent = nullptr) {
        static auto instance = new VolumeSlider(parent);
        return instance;
    }

    void showAtPosition(const QPoint& position) {
        this->move(position);
        this->show();
        this->setFocus();
    }

    int getVolume() const {
        return _slider->value();
    }

    void setRelationButton(QPushButton* button) {
        _relateButton = button;
    }

signals:
    void volumeChanged(int volume);

private:
    bool _quiet;
    QSlider* _slider;
    // to show percent of the volume
    QLabel* _label;
    QPushButton* _relateButton;
};
