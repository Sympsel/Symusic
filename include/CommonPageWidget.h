#pragma once

#include <qboxlayout.h>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>
#include <utility>

#include "Sync.hpp"

class CommonPageWidget : public QWidget {
    Q_OBJECT

private:
    QWidget* createHeadWidget(const QString& coverPath, const QString& description) {
        const auto headWidget = new QWidget();
        headWidget->setFixedHeight(120);
        const auto coverLabel = new QLabel();
        coverLabel->setScaledContents(true);
        coverLabel->setPixmap(QPixmap(coverPath));
        coverLabel->setFixedSize(QSize(120, 120));

        const auto hLayout = new QHBoxLayout(headWidget);

        const auto rightWidget = new QWidget(headWidget);
        Sync::clearWidgetMargins(rightWidget);
        const auto vLayout = new QVBoxLayout(rightWidget);

        const auto descriptionLabel = new QLabel(description);
        vLayout->addWidget(descriptionLabel);
        vLayout->addWidget(_playAllButton);
        vLayout->addWidget(descriptionLabel);
        vLayout->addStretch(1);
        vLayout->addWidget(_playAllButton);
        //Sync::widgetToLayout(vLayout, {
        //                         descriptionLabel, _playAllButton
        //                     });
        Sync::clearLayoutVMargins({vLayout});
        _playAllButton->setFixedWidth(150);

        Sync::widgetToLayout(hLayout, {
                                 coverLabel, rightWidget
                             });

        return headWidget;
    }

    QWidget* createMiddleWidget() {
        const auto middleWidget = new QWidget();
        const auto layout = new QHBoxLayout(middleWidget);
        const auto musicLabel = new QLabel("音乐");
        const auto singerLabel = new QLabel("歌手");
        const auto albumLabel = new QLabel("专辑");

        middleWidget->setFixedHeight(30);
        Sync::widgetToLayout(layout, {
                                 musicLabel, singerLabel, albumLabel
                             });
        return middleWidget;
    }

public:
    explicit CommonPageWidget(QString pageName, const QString& coverPath, QWidget* parent = nullptr)
        : QWidget(parent)
          , _pageName(std::move(pageName))
          , _playAllButton(new QPushButton("播放全部"))
          , _playlist(new QListWidget()) {
        const auto mainLayout = new QVBoxLayout(this);

        const auto headWidget = createHeadWidget(coverPath, "简介：eswgrftg");
        const auto middleLabel = createMiddleWidget();

        Sync::widgetToLayout(mainLayout, {
                                 headWidget, middleLabel, _playlist
                             });
        // _playlist->addItem(new QListWidgetItem())
    }

private:
    QString _pageName;
    QPushButton* _playAllButton;
    QListWidget* _playlist;
};
