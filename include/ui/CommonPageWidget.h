#pragma once

#include "ListItem.h"

#include <functional>
#include <QListWidget>
#include <QWidget>

#include "entity/SongManager.h"
#include "utils/Log.hpp"
#include "utils/Sync.hpp"

class CommonPageWidget : public QWidget {
    Q_OBJECT

public:
    using SpecializationCallBack = std::function<void()>;

private:
    [[nodiscard]] QWidget* createHeadWidget(const QString& coverFileWithoutPath, const QString& description) const;

    static QWidget* createMiddleWidget();

public:
    explicit CommonPageWidget(QString pageName, const QString& coverFileWithoutPath, const QString& description = "",
                              QWidget* parent = nullptr);

    void initData(const SongList& songList);

    void reloadData(const SongList& songList);

    [[nodiscard]] const QString& getPageName() const {
        return _pageName;
    }

    bool eventFilter(QObject* watched, QEvent* event) override;

    void setSpecial(const SpecializationCallBack& specializationCb) {
        _specializationCb = specializationCb;
    }

protected:
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void songItemDoubleClicked(const SongPtr& song);

private:
    QString _pageName;
    QPushButton* _playAllButton;
    QListWidget* _playlist;
    SpecializationCallBack _specializationCb;
};
