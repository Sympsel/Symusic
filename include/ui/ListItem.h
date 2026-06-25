#pragma once

#include <QLabel>
#include <QHBoxLayout>
#include <iostream>
#include <QWidget>

#include "entity/SongManager.h"
#include "utils/Sync.hpp"

class ListItem : public QWidget {
    Q_OBJECT

private:
    void setupUI();

public:
    friend std::ostream& operator<<(std::ostream& os, const ListItem& listItem) {
        os << "{";
        os << "song=" << listItem._song;
        os << "}";
        return os;
    }

    explicit ListItem(SongPtr  song);

    [[nodiscard]] const SongPtr& getSong() const {
        return _song;
    }

    void updateIconStatus() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void mouseDoubleClickEvent(QMouseEvent* event) override;

signals:
    void likeStatusUpdated();

    void doubleClicked(const SongPtr& song);

private:
    QTimer* _clickTimer;
    bool _pendingSingleClick{};
    bool _skipNextPress{};
    QPushButton* _likeButton;
    SongPtr _song;
};
