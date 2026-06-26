#pragma once

#include <QObject>
#include <QString>

class StatusManager : public QObject {
    Q_OBJECT

public:
    StatusManager(const StatusManager&) = delete;
    StatusManager(StatusManager&&) = delete;
    StatusManager& operator=(const StatusManager&) = delete;
    StatusManager& operator=(StatusManager&&) = delete;

    static StatusManager& getInstance() {
        static StatusManager instance;
        return instance;
    }

    /**
     * @brief 显示状态消息
     * @param message 要显示的消息文本
     * @param timeout 显示时长(毫秒),0 表示永久显示直到下次更新
     */
    void showMessage(const QString& message, const int timeout = 3000) {
        emit statusChanged(message, timeout);
    }

    /**
    * @brief 显示状态消息
    * @param message 要显示的消息文本
    * @param timeout 显示时长(毫秒),0 表示永久显示直到下次更新
    */
    void showMessage(const std::string& message, const int timeout = 3000) {
        emit statusChanged(message.c_str(), timeout);
    }

    /**
    * @brief 显示状态消息
    * @param message 要显示的消息文本
    * @param timeout 显示时长(毫秒),0 表示永久显示直到下次更新
    */
    void showMessage(const char* message, const int timeout = 3000) {
        emit statusChanged(message, timeout);
    }

    /**
   * @brief 清除状态消息,恢复默认文本
   */
    void clearMessage() {
        emit statusCleared();
    }

signals:
    void statusChanged(const QString& message, int timeout);
    void statusCleared();

private:
    explicit StatusManager(QObject* parent = nullptr) : QObject(parent) {}
};
