module;

#include <QWidget>
#include <QLabel>

export module symusic.component.lyrics_item;

export class LyricsItem : public QWidget {
    Q_OBJECT
public:
    explicit LyricsItem(const QString& text = "", QWidget* parent = nullptr)
        : QWidget(parent), _textLabel(new QLabel(text, this)) {
        _textLabel->setAlignment(Qt::AlignCenter);
        _textLabel->setGeometry(0, 0, width(), height());
        setBackground("");
    }

    void setBackground(const QString& colorRgb) const {
        if (colorRgb.isEmpty()) {
            _textLabel->setStyleSheet(
                "QLabel {"
                "   background-color: transparent;"
                "   margin: 0px;"
                "   padding: 0px;"
                "}"
            );
        } else {
            _textLabel->setStyleSheet(
                QString(
                    "QLabel {"
                    "   background-color: rgb(%1);"
                    "   margin: 0px;"
                    "   padding: 0px;"
                    "}"
                ).arg(colorRgb)
            );
        }
    }

    ~LyricsItem() override = default;

protected:
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        _textLabel->setGeometry(0, 0, width(), height());
    }

private:
    QLabel* _textLabel;
};
