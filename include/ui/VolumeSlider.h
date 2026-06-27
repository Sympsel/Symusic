#pragma once

#include <QSlider>
#include <QLabel>
#include <QPushButton>

class VolumeSlider : public QWidget {
    Q_OBJECT

private:
    explicit VolumeSlider(QWidget* parent = nullptr);

    bool isQuiet() const {
        return _quiet;
    }

public:
    VolumeSlider(const VolumeSlider&) = delete;
    VolumeSlider(VolumeSlider&&) = delete;
    VolumeSlider& operator=(const VolumeSlider&) = delete;
    VolumeSlider& operator=(VolumeSlider&&) = delete;

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

    void setVolume(const int percent) const {
        _slider->setValue(percent);
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
