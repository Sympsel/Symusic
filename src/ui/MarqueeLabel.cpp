#include "ui/MarqueeLabel.h"

MarqueeLabel::MarqueeLabel(QWidget* parent) : QLabel(parent), _offset(0), _pauseTimer(new QTimer(this)) {
    setAttribute(Qt::WA_Hover, true);
    _timer = new QTimer(this);

    connect(_pauseTimer, &QTimer::timeout, this, [this]() {
        _pauseTimer->stop();
        _offset = 0;
        if (const int textWidth = fontMetrics().horizontalAdvance(_fullText);
            textWidth > width() && width() > 0) {
            _timer->start(50);
        }
        update();
    });

    connect(_timer, &QTimer::timeout, this, [this]() {
        const int textWidth = fontMetrics().horizontalAdvance(_fullText);
        const int widgetWidth = width();

        if (textWidth > widgetWidth && widgetWidth > 0) {
            if (const int scrollEndOffset = textWidth - widgetWidth; _offset >= scrollEndOffset) {
                _offset = scrollEndOffset;
                _timer->stop();
                _pauseTimer->start(1000);
                update();
            } else {
                _offset += 2;
                update();
            }
        } else {
            _timer->stop();
            _offset = 0;
        }
    });
}

void MarqueeLabel::adjustWidthToContent(const int padding) {
    const int textWidth = fontMetrics().horizontalAdvance(_fullText);
    const int targetWidth = textWidth + padding;

    if (_minWidth > 0 || _maxWidth > 0) {
        int constrainedWidth = targetWidth;
        if (_minWidth > 0 && constrainedWidth < _minWidth) {
            constrainedWidth = _minWidth;
        }
        if (_maxWidth > 0 && constrainedWidth > _maxWidth) {
            constrainedWidth = _maxWidth;
        }
        setFixedWidth(constrainedWidth);
    } else {
        setFixedWidth(targetWidth);
    }
}

void MarqueeLabel::setMarqueeText(const QString& text) {
    _fullText = text;
    setToolTip(text);

    const int textWidth = fontMetrics().horizontalAdvance(text);
    const int widgetWidth = width();

    _offset = 0;
    _timer->stop();
    _pauseTimer->stop();

    if (textWidth > widgetWidth && widgetWidth > 0) {
        _pauseTimer->start(1000);
    }

    update();
}

void MarqueeLabel::paintEvent(QPaintEvent* event) {
    if (!isVisible()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing);

    if (_fullText.isEmpty()) {
        QLabel::paintEvent(event);
        return;
    }

    const int textWidth = fontMetrics().horizontalAdvance(_fullText);
    const int widgetWidth = width();
    const int widgetHeight = height();

    if (textWidth <= widgetWidth) {
        painter.drawText(0, 0, widgetWidth, widgetHeight, Qt::AlignLeft | Qt::AlignVCenter, _fullText);
        return;
    }

    if (_pauseTimer->isActive()) {
        painter.drawText(0, 0, widgetWidth, widgetHeight, Qt::AlignLeft | Qt::AlignVCenter, _fullText);
        return;
    }

    painter.drawText(-_offset, 0, textWidth, widgetHeight, Qt::AlignLeft | Qt::AlignVCenter, _fullText);
}

void MarqueeLabel::resizeEvent(QResizeEvent* event) {
    QLabel::resizeEvent(event);
    if (!_fullText.isEmpty()) {
        const int textWidth = fontMetrics().horizontalAdvance(_fullText);

        _timer->stop();
        _pauseTimer->stop();
        _offset = 0;

        if (textWidth > width()) {
            _pauseTimer->start(1000);
        }

        update();
    }
}
