#include <QCoreApplication>
#include <QDebug>
#include "network/tcp_server.h"
#include "db/db_manager.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    qDebug() << "\n=== 启动后端服务器 ===";

    // 初始化 SQLite 数据库（自动创建表结构和示例数据）
    bool dbConnected = DBManager::getInstance()->init("ftms.db");
    if (!dbConnected) {
        qCritical() << "数据库初始化失败，程序退出！";
        return -1;
    }

    // 启动TCP服务器
    TcpServer server;
    if (!server.listen(QHostAddress::Any, 12345)) {
        qCritical() << "服务器启动失败：" << server.errorString();
        return -1;
    }
    qDebug() << "服务器正在监听端口 12345...";

    return a.exec();
}
