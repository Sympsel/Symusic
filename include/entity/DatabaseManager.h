#pragma once

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>

class Song;
class SongManager;

class DatabaseManager {
private:
    explicit DatabaseManager() : _parent(nullptr) {
        _sqlLiteDB = QSqlDatabase::addDatabase("QSQLITE");
    }

public:
    static DatabaseManager& getInstance() {
        static DatabaseManager instance;
        return instance;
    }

    DatabaseManager operator=(const DatabaseManager&) = delete;
    DatabaseManager operator=(DatabaseManager&&) = delete;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) = delete;


    void initDatabase(QWidget* parent);

    void loadData(SongManager& songManager) const;

    void saveData();

    bool isContainsSong(const QString& uuid) const;

    bool insertSong(const std::shared_ptr<Song>& song) const;

    bool removeSong(const QString& uuid) const;

private:
    QWidget* _parent;
    QSqlDatabase _sqlLiteDB;
};
