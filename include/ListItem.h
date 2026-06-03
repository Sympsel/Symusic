#pragma once

#include <QLabel>
#include <QHBoxLayout>
#include <iostream>

#include "QWidget"
#include "Song.h"
#include "Sync.hpp"

class ListItem : public QWidget {
    Q_OBJECT

private:
    void setupUI();

public:

    friend std::ostream& operator<<(std::ostream& os, const ListItem& listItem) {
        os << "{";
        os << "isLiked=" << listItem._isLiked << ",";
        os << "song=" << listItem._song << ",";
        os << "}";
        return os;
    }

    explicit ListItem(Song song, bool isLiked = false);

    void updateIconStatus() const;

signals:
    void likeStatusUpdated();

private:
    bool _isLiked;
    QPushButton* _likeButton;
    Song _song;
};
