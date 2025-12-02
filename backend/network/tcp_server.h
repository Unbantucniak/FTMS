#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QTcpServer>

class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TcpServer(QObject* parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif // TCP_SERVER_H
