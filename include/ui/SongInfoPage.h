#pragma once

#include <QLabel>
#include <QPushButton>

#include "entity/Song.h"
#include "entity/SongManager.h"

class SongInfoPage : public QWidget {
    Q_OBJECT

private:
    void setupUI();
    void applyStyles();

public:
    explicit SongInfoPage(const SongPtr& song, SongList& songList, QWidget* parent = nullptr);

    void updateSong(const SongPtr& song);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

signals:
    void likeStatusChanged();

private:
    SongPtr _song;
    SongList* _songList;
    bool _originLiked;
    bool _willLike;

    QLabel* _coverLabel;
    QLabel* _nameLabel;
    QLabel* _artistLabel;
    QLabel* _albumLabel;
    QLabel* _durationLabel;
    QLabel* _tagsLabel;
    QLabel* _listsLabel;
    QPushButton* _likeButton;
    QPushButton* _playButton;
    QPushButton* _closeButton;
    QPoint _dragPos;
};
;
