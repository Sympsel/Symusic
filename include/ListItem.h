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
    void setupUI();

public:
    explicit ListItem(Song song, bool isLiked = false);

    void updateStatus() const;

private:
    bool _isLiked;
    QPushButton* _likeButton;
    Song _song;
};
