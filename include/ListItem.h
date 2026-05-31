#pragma once

#include <QLabel>
#include <QHBoxLayout>
#include <utility>

#include "QWidget"
#include "Song.h"
#include "Sync.hpp"

class ListItem : public QWidget {
    Q_OBJECT

private:
    void setupUI() {
        const auto mainLayout = new QHBoxLayout(this);

        const auto leftWidget = new QWidget();
        const auto leftLayout = new QHBoxLayout(leftWidget);
        leftLayout->addWidget(_likeButton);
        leftLayout->addWidget(new QLabel(_song.getName()));
        const auto tags = _song.getTags();
        QString tagsStr = "[";
        for (const auto& tag : tags) {
            tagsStr.append(tag + ", ");
        }
        if (!tags.empty()) {
            tagsStr.removeLast();
            tagsStr.removeLast();
        }
        tagsStr.append("]");
        leftLayout->addWidget(new QLabel(tagsStr));

        const auto centralWidget = new QWidget();
        const auto centralLayout = new QHBoxLayout(centralWidget);
        centralLayout->addWidget(new QLabel(_song.getArtist()));

        const auto rightWidget = new QWidget();
        const auto rightLayout = new QHBoxLayout(rightWidget);
        rightLayout->addWidget(new QLabel(_song.getAlbum()));

        mainLayout->addWidget(leftWidget);
        mainLayout->addStretch(1);
        mainLayout->addWidget(centralWidget);
        mainLayout->addStretch(1);
        mainLayout->addWidget(rightWidget);
    }

public:
    explicit ListItem(Song song)
        : _likeButton(new QPushButton)
          , _song(std::move(song)) {
    }

private:
    QPushButton* _likeButton;
    Song _song;
};
