#pragma once

#include <QLabel>
#include <QTimer>
#include <QPainter>

class MarqueeLabel : public QLabel {
    Q_OBJECT

public:
    explicit MarqueeLabel(QWidget* parent = nullptr);

    /**
    * @brief 停止滚动动画
    */
    void stopMarquee() {
        _timer->stop();
        _offset = 0;
        update();
    }

    void setMarqueeText(const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    QString _fullText;
    int _offset;
    QTimer* _timer;
};
