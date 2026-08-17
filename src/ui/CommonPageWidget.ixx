module;

#include <functional>
#include <QListWidget>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

export module symusic.ui.common_page_widget;
import symusic.utils.sync;
import symusic.entity.play_manager;
import symusic.entity.song_manager;
import symusic.entity.status_manager;
import symusic.component.list_item;

export class CommonPageWidget : public QWidget {
    Q_OBJECT

public:
    using SpecializationCallBack = std::function<void()>;
    using ReloadCallback = std::function<void(CommonPageWidget*)>;

private:
    [[nodiscard]] QWidget* createHeadWidget(const QString& description) const;

    void setCover() const;

    static QWidget* createMiddleWidget();

public:
    void updateCover() const;

    explicit CommonPageWidget(QString pageName, const QString& description = "", QWidget* parent = nullptr);

    void initData(const SongList& songList);

    void reloadData(const SongList& songList);

    [[nodiscard]] const QString& getPageName() const { return _pageName; }
    [[nodiscard]] SongList* getSongList() const { return _songList; }

    bool eventFilter(QObject* watched, QEvent* event) override;

    void setSpecialCallback(const SpecializationCallBack& specializationCb) {
        _specializationCb = specializationCb;
    }

    void setReloadCallback(const ReloadCallback& reloadCb) {
        _reloadCb = reloadCb;
    }

    void reloadData();

protected:
    void keyPressEvent(QKeyEvent* event) override;

    signals:
        void songItemDoubleClicked(const SongPtr& song);
    void needUpdate();

private:
    QString _pageName;
    QLabel* _coverLabel;
    QPushButton* _playAllButton;
    QListWidget* _playlist;
    SongList* _songList;
    SpecializationCallBack _specializationCb;
    ReloadCallback _reloadCb;
};

