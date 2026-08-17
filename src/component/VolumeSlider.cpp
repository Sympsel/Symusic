module symusic.component.volume_slider;

#include <QVBoxLayout>

import symusic.common;
import symusic.entity.play_manager;
import symusic.uitls.sync;

VolumeSlider::VolumeSlider(QWidget* parent) : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
                                              , _quiet(false)
                                              , _slider(new QSlider(Qt::Vertical))
                                              , _label(new QLabel())
                                              , _relateButton(nullptr) {
    const auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    _slider->setRange(0, 100);
    auto& playManager = PlayManager::getInstance();
    _slider->setValue(playManager.getVolume());
    _label->setText(QString("%1%").arg(_slider->value()));
    connect(_slider, &QSlider::valueChanged, this, [this, &playManager](const int value) {
        _label->setText(QString("%1%").arg(value));
        playManager.setVolume(value);
        emit volumeChanged(value);
        _quiet = (value == 0);
        if (_relateButton) {
            if (_quiet) {
                _relateButton->setIcon(QIcon(prefix::normalImages + "关闭音量.png"));
            } else {
                if (const int volume = _slider->value(); volume < 33) {
                    _relateButton->setIcon(QIcon(prefix::normalImages + "最小音量.png"));
                } else if (value > 66) {
                    _relateButton->setIcon(QIcon(prefix::normalImages + "最大音量.png"));
                } else {
                    _relateButton->setIcon(QIcon(prefix::normalImages + "中等音量.png"));
                }
            }
        } else {
            logWarn() << "音量按钮未设置";
        }
    });
    layout->setAlignment(_slider, Qt::AlignHCenter);
    layout->setAlignment(_label, Qt::AlignHCenter);
    Sync::widgetToLayout(layout, {_slider, _label});
}
