module;

#include <QPushButton>
#include <QVBoxLayout>
#include <QEvent>

export module symusic.component.playlist_item;
export import symusic.entity.play_manager;

export class PlaylistItem : public QWidget {
    Q_OBJECT

public:
    explicit PlaylistItem(const PlayContext& songCtx, QString description, QWidget* parent = nullptr);

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    PlayContext _songCtx;
    QPushButton* _button;
    QString _description;
};
