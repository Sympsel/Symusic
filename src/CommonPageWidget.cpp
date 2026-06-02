#include "CommonPageWidget.h"
#include "SongManager.h"

QWidget* CommonPageWidget::createHeadWidget(const QString& coverPath, const QString& description) const {
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

    Sync::clearLayoutVMargins({vLayout});
    _playAllButton->setFixedWidth(150);

    Sync::widgetToLayout(hLayout, {
                             coverLabel, rightWidget
                         });

    return headWidget;
}

QWidget* CommonPageWidget::createMiddleWidget() {
    const Color& color = ColorTheme::getInstance().getColor();

    const auto middleWidget = new QWidget();
    const auto layout = new QHBoxLayout(middleWidget);
    const auto musicLabel = new QLabel("音乐");
    const auto singerLabel = new QLabel("歌手");
    const auto albumLabel = new QLabel("专辑");

    const QString labelStyle = QString(
        "QLabel {"
        "   background-color: transparent;"
        "   padding: 4px 8px;"
        "}"
        "QLabel:hover {"
        "   background-color: rgb(%1);"
        "}"
    ).arg(color.hoverOn);

    musicLabel->setAttribute(Qt::WA_Hover, true);
    musicLabel->setStyleSheet(labelStyle);
    singerLabel->setAttribute(Qt::WA_Hover, true);
    singerLabel->setStyleSheet(labelStyle);
    albumLabel->setAttribute(Qt::WA_Hover, true);
    albumLabel->setStyleSheet(labelStyle);


    middleWidget->setFixedHeight(40);
    Sync::widgetToLayout(layout, {
                             musicLabel, singerLabel, albumLabel
                         });
    return middleWidget;
}

void CommonPageWidget::initData() const {
    for (const auto song : SongManager::getInstance().getLikedList()) {
        const auto item = new QListWidgetItem(_playlist);
        item->setSizeHint(QSize(0, 40));
        _playlist->setItemWidget(item, new ListItem(song, true));
    }
}

CommonPageWidget::CommonPageWidget(QString pageName, const QString& coverPath, const QString& description,
                                   QWidget* parent) : QWidget(parent)
                                                      , _pageName(std::move(pageName))
                                                      , _playAllButton(new QPushButton("播放全部"))
                                                      , _playlist(new QListWidget()) {
    const Color& color = ColorTheme::getInstance().getColor();
    _playlist->setStyleSheet(QString(
        "QListWidget {"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   padding: 0px;"
        "   border: none;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: rgb(%1);"
        "}"
        "QListWidget::item:hover {"
        "   background-color: rgb(%2);"
        "}"
    ).arg(color.hoverOn, color.pressed));

    const auto mainLayout = new QVBoxLayout(this);

    const auto headWidget = createHeadWidget(coverPath, description);
    const auto middleLabel = createMiddleWidget();

    Sync::widgetToLayout(mainLayout, {
                             headWidget, middleLabel, _playlist
                         });
    initData();
}
