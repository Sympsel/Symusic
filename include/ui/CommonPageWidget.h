#pragma once

#include "ListItem.h"

#include <QListWidget>
#include <QWidget>

#include "entity/Color.hpp"
#include "entity/SongManager.h"
#include "utils/Log.hpp"
#include "utils/Sync.hpp"

class CommonPageWidget : public QWidget {
    Q_OBJECT

private:
    [[nodiscard]] QWidget* createHeadWidget(const QString& coverFileWithoutPath, const QString& description) const;

    static QWidget* createMiddleWidget();

public:
    explicit CommonPageWidget(QString pageName, const QString& coverFileWithoutPath, const QString& description = "",
                              QWidget* parent = nullptr);

    void initData(const SongManager::SongList& songList);

    void reloadData(const SongManager::SongList& songList);

    [[nodiscard]] const QString& getPageName() const {
        return _pageName;
    }

    bool eventFilter(QObject* watched, QEvent* event) override;

protected:
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void songItemDoubleClicked(const SongPtr& song);

private:
    QString _pageName;
    QPushButton* _playAllButton;
    QListWidget* _playlist;
};
