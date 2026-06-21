#include "ui/VolumeSlider.h"

VolumeSlider::VolumeSlider(QWidget* parent): QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
                                             , _quiet(false)
                                             , _slider(new QSlider(Qt::Vertical))
                                             , _label(new QLabel()) {
    const auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    _slider->setRange(0, 100);
    _slider->setValue(50);
    _label->setText("50%");
    connect(_slider, &QSlider::valueChanged, this, [this](int value) {
        _label->setText(QString("%1%").arg(value));
        emit volumeChanged(value);
        _quiet = (value == 0);
    });
    layout->setAlignment(_slider, Qt::AlignHCenter);
    layout->setAlignment(_label, Qt::AlignHCenter);
    Sync::widgetToLayout(layout, {_slider, _label});
}
