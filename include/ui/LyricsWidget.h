#pragma once

#include <QLabel>
#include <QVBoxLayout>
#include "entity/Common.hpp"
#include "utils/Sync.hpp"

class LyricsWidget final : public QWidget {
    Q_OBJECT

public:
    explicit LyricsWidget(QWidget* parent = nullptr) : QWidget(parent) {
        const auto container = new QWidget(this);
        const auto containerLayout = new QVBoxLayout(container);
        Sync::clearLayoutMargins(containerLayout);
        containerLayout->setSpacing(0);

        const auto layout = new QVBoxLayout(this);
        layout->addStretch(1);
        layout->addWidget(container);
        layout->addStretch(1);
        Sync::clearLayoutMargins(layout);

        // 创建7个歌词标签
        for (int i{}; i < _count; ++i) {
            auto* label = new QLabel("歌词efdrgtt歌词", container);
            label->setAlignment(Qt::AlignCenter);
            label->setFixedHeight(60);
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            // 设置样式
            if (i == 3) {
                // 中间条目高亮
                label->setStyleSheet(
                    QString(
                        "QLabel {"
                        "   background-color: rgb(%1);"
                        "   color: white;"
                        "   font-size: 16px;"
                        "}"
                    ).arg(_hoverColor.isEmpty() ? "40,40,40" : _hoverColor)
                );
            } else {
                label->setStyleSheet(
                    "QLabel {"
                    "   background-color: transparent;"
                    "   color: white;"
                    "   font-size: 14px;"
                    "}"
                );
            }

            _labels.append(label);
            containerLayout->addWidget(label);
        }

        // 设置容器高度
        const int totalHeight = _count * 60;
        container->setFixedHeight(totalHeight);
    }

    ~LyricsWidget() = default;

private:
    static constexpr int _count = 7;
    QVector<QLabel*> _labels;
    QString _hoverColor = ColorTheme::getInstance().getPlayItemColor().hover;
};
