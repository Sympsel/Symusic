#include "ui/PlaySlider.h"

void PlaySlider::setValueByTime(int second) {
    if (second < 0) {
        second = 0;
    }
    if (second > _totalDuration) {
        second = _totalDuration;
    }
    this->setValue(second * 100 / _totalDuration);
}

void PlaySlider::setProcessByPercent(const int percent) {
    _currDuration = percent * _totalDuration / 100;
    this->setValue(_currDuration);
}
