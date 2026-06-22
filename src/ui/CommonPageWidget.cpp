#include "ui/CommonPageWidget.h"
#include "entity/SongManager.h"

#include <QKeyEvent>

QWidget* CommonPageWidget::createHeadWidget(const QString& coverFileWithoutPath, const QString& description) const {
    const auto headWidget = new QWidget();
    headWidget->setFixedHeight(120);
    const auto coverLabel = new QLabel();
    coverLabel->setScaledContents(true);
    coverLabel->setPixmap(QPixmap(prefix::normalImages + coverFileWithoutPath));
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

void CommonPageWidget::initData(const SongManager::SongList& songList) {
    reloadData(songList);
}

void CommonPageWidget::reloadData(const SongManager::SongList& songList) {
    _playlist->clear();
    for (const auto& song : songList) {
        const auto item = new QListWidgetItem(_playlist);
        item->setSizeHint(QSize(0, 40));
        const auto listItem = new ListItem(song);
        _playlist->setItemWidget(item, listItem);
        connect(listItem, &ListItem::doubleClicked, this, [this, song]() {
            emit songItemDoubleClicked(song);
        });
    }
}

void CommonPageWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        if (const auto currItem = _playlist->currentItem()) {
            if (const auto listItemWidget = qobject_cast<ListItem*>(_playlist->itemWidget(currItem))) {
                const auto song = listItemWidget->getSong();
                emit songItemDoubleClicked(song);
            }
        }
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

bool CommonPageWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == _playlist && event->type() == QEvent::KeyPress) {
        if (const auto keyEvent = static_cast<QKeyEvent*>(event); keyEvent->key() == Qt::Key_Space) {
            if (const auto currItem = _playlist->currentItem()) {
                if (const auto listItemWidget = qobject_cast<ListItem*>(_playlist->itemWidget(currItem))) {
                    const auto song = listItemWidget->getSong();
                    emit songItemDoubleClicked(song);
                }
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}


CommonPageWidget::CommonPageWidget(QString pageName, const QString& coverFileWithoutPath, const QString& description,
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

    _playlist->installEventFilter(this);

    const auto mainLayout = new QVBoxLayout(this);

    const auto headWidget = createHeadWidget(coverFileWithoutPath, description);
    const auto middleLabel = createMiddleWidget();

    Sync::widgetToLayout(mainLayout, {
                             headWidget, middleLabel, _playlist
                         });
}
