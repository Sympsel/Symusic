#pragma once

#include "ListItem.h"
#include <QListWidget>
#include <QWidget>
#include <Color.hpp>

#include "Log.hpp"
#include "SongManager.h"
#include "Sync.hpp"

class CommonPageWidget : public QWidget {
    Q_OBJECT

private:
    [[nodiscard]] QWidget* createHeadWidget(const QString& coverPath, const QString& description) const;

    QWidget* createMiddleWidget();

public:
    explicit CommonPageWidget(QString pageName, const QString& coverPath, const QString& description = "",
                              QWidget* parent = nullptr);

    void initData(const SongManager::SongList& songList);

    void reloadData(const SongManager::SongList& songList);

    [[nodiscard]] const QString& getPageName() const {
        return _pageName;
    }
protected:
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void songItemDoubleClicked(const Song& song);

private:
    QString _pageName;
    QPushButton* _playAllButton;
    QListWidget* _playlist;
};
