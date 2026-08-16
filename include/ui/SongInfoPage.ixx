module;

#include <QLabel>
#include <QPushButton>

#include "entity/PlayManager.hpp"
#include "entity/SongManager.h"

export module symusic.ui.song_info_page;

class SongInfoPage : public QWidget {
    Q_OBJECT

private:
    void setupUI();
    void setupButtonStyle();
    void setupButtonConnect();

public:
    explicit SongInfoPage(const SongContext& songCtx, QWidget* parent = nullptr);

    void updateSong(const SongPtr& song);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    signals:
        void likeStatusChanged();

private:
    SongContext _songCtx;
    bool _originLiked;
    bool _willLike;

    QLabel* _coverLabel;
    QLabel* _nameLabel;
    QLabel* _artistLabel;
    QLabel* _albumLabel;
    QLabel* _durationLabel;
    QLabel* _playCountLabel;
    QLabel* _tagsLabel;
    QLabel* _listsLabel;
    QPushButton* _likeButton;
    QPushButton* _playButton;
    QPushButton* _closeButton;
    QPoint _dragPos;
};
;
