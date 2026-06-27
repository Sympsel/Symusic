#include "ui/MarqueeLabel.h"

MarqueeLabel::MarqueeLabel(QWidget* parent) : QLabel(parent), _offset(0) {
    // 启用悬停支持,使 Tooltip 生效
    setAttribute(Qt::WA_Hover, true);
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [this]() {
        const int textWidth = fontMetrics().horizontalAdvance(_fullText);

        if (const int widgetWidth = width(); textWidth > widgetWidth && widgetWidth > 0) {
            // 滚动到文本末尾后,暂停一段时间再重新开始
            if (_offset >= textWidth) {
                _offset = 0;
            } else {
                _offset += 2;
            }
            update();
        } else {
            _timer->stop();
            _offset = 0;
        }
    });
}

void MarqueeLabel::setMarqueeText(const QString& text) {
    _fullText = text;
    setToolTip(text);

    // 直接在 paintEvent 中绘制

    const int textWidth = fontMetrics().horizontalAdvance(text);

    if (const int widgetWidth = width(); textWidth > widgetWidth && widgetWidth > 0) {
        _offset = 0;
        _timer->start(50);
    } else {
        _timer->stop();
        _offset = 0;
    }
    update();
}

void MarqueeLabel::paintEvent(QPaintEvent* event) {
    if (!isVisible()) {
        return;
    }

    QPainter painter(this);
    // 启用文本抗锯齿
    painter.setRenderHint(QPainter::TextAntialiasing);

    // 如果文本为空,使用默认绘制
    if (_fullText.isEmpty()) {
        QLabel::paintEvent(event);
        return;
    }
    const int textWidth = fontMetrics().horizontalAdvance(_fullText);
    const int widgetWidth = width();
    // 如果不需要滚动,居中显示
    if (textWidth <= widgetWidth || _offset == 0) {
        painter.drawText(0, 0, widgetWidth, height(), alignment(), _fullText);
        return;
    }

    // 绘制滚动文本
     painter.drawText(-_offset, 0,
                      fontMetrics().horizontalAdvance(_fullText), height(),
                      Qt::AlignLeft | Qt::AlignVCenter, _fullText);
}

void MarqueeLabel::resizeEvent(QResizeEvent* event) {
    QLabel::resizeEvent(event);
    // 窗口大小改变时重新判断是否需要滚动
    if (!_fullText.isEmpty()) {
        if (const int textWidth = fontMetrics().horizontalAdvance(_fullText);
            textWidth > width()) {
            _timer->start(50);
        } else {
            _timer->stop();
            _offset = 0;
            update();
        }
    }
}
