#include "ui/PlaySlider.h"

PlaySlider::PlaySlider(QWidget* parent) : QSlider(Qt::Horizontal, parent) {
    this->setRange(0, 100);
    this->setToolTip("拖动调整播放进度");

    const auto& playManager = PlayManager::getInstance();

    // 监听播放进度变化,实时更新滑块位置
    connect(&playManager, &PlayManager::positionChanged, this, [this](long long position, long long duration) {
        // 拖动时忽略进度更新,避免滑块被拽回
        if (!_isDragging && duration > 0) {
            const double percent = static_cast<double>(position) / static_cast<double>(duration);
            setValue(static_cast<int>(percent * 100));
        }
    });

    // 监听歌曲切换,重置进度条
    connect(&playManager, &PlayManager::songPlayed, this, [this]() {
        setValue(0);
    });

    // 开始拖动时标记状态
    connect(this, &QSlider::sliderPressed, this, [this]() {
        _isDragging = true;
    });
    // 用户拖动滑块时跳转播放位置
    connect(this, &QSlider::sliderReleased, this, [this, &playManager]() {
        _isDragging = false;
        const int percent = value();
        if (const LL duration = playManager.getDuration(); duration > 0) {
            const LL newPosition = percent * duration / 100;
            playManager.setPosition(newPosition);
        }
    });
}
