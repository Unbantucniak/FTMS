#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <iostream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "network/tcp_server.h"
#include "db/db_manager.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    qDebug() << "\n=== 启动后端服务器 ===";

    // 初始化数据库
    bool dbConnected = DBManager::getInstance()->init(
        "localhost", 3306, "FTMSDB", "root", "114514"
    );
    if (!dbConnected) {
        qCritical() << "数据库连接失败，程序退出！";
        return -1;
    }
    qDebug() << "数据库连接成功！";

    // 启动TCP服务器
    TcpServer server;
    if (!server.listen(QHostAddress::Any, 12345)) {
        qCritical() << "服务器启动失败：" << server.errorString();
        return -1;
    }
    qDebug() << "服务器正在监听端口 12345...";

    return a.exec();
}
