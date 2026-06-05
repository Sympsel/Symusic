#pragma once

#include <QLabel>
#include <QHBoxLayout>
#include <iostream>
#include <QWidget>

#include "entity/Song.h"
#include "utils/Sync.hpp"

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

    [[nodiscard]] const Song& getSong() const {
        return _song;
    }

    void updateIconStatus() const;
protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

signals:
    void likeStatusUpdated();

    void doubleClicked(const Song& song);

private:
    bool _isLiked;
    QPushButton* _likeButton;
    Song _song;
};
