module;

#include <QLabel>
#include <QTimer>
#include <QPainter>

export module symusic.component.marqueel_label;

export class MarqueeLabel : public QLabel {
    Q_OBJECT

private:
    void updateWidth() {
        if (_minWidth > 0) {
            QLabel::setMinimumWidth(_minWidth);
        }
        if (_maxWidth > 0) {
            QLabel::setMaximumWidth(_maxWidth);
        }
    }

public:
    explicit MarqueeLabel(QWidget* parent = nullptr);

    /**
    * @brief 停止滚动动画
    */
    void stopMarquee() {
        _timer->stop();
        _pauseTimer->stop();
        _offset = 0;
        update();
    }

    /**
    * @brief 设置最小宽度
    */
    void setMinWidth(const int minWidth) {
        _minWidth = minWidth;
        updateWidth();
    }

    /**
    * @brief 设置最大宽度，-1 表示无限制
    */
    void setMaxWidth(const int maxWidth) {
        _maxWidth = maxWidth;
        updateWidth();
    }

    /**
     * @brief 根据父布局自动设置合适的最大宽度，确保能触发滚动
     * @param percentage 占父容器宽度的百分比 (0.0-1.0)
     */
    void setWidthByPercentage(const double percentage) {
        if (parentWidget()) {
            if (const int parentWidth = parentWidget()->width(); parentWidth > 0) {
                const int targetWidth = static_cast<int>(parentWidth * percentage);
                setMaxWidth(targetWidth);
            }
        }
    }


    /**
     * @brief 根据文本内容自动调整宽度
     * @param padding 额外留白像素
     */
    [[deprecated]] void adjustWidthToContent(int padding = 20);

    void setMarqueeText(const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    QString _fullText;
    int _offset;
    QTimer* _timer;
    QTimer* _pauseTimer;
    int _minWidth = 0;
    int _maxWidth = -1;
};
