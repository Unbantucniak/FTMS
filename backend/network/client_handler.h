#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include <QObject>

#include "data_model.h"

class ClientHandler : public QThread {
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);
    void run() override;

signals:
    void finished();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    qintptr m_socketDescriptor;
    QTcpSocket* m_socket;

    void handleLoginRequest(const QByteArray& data);
    void handleFlightQueryRequest(const QByteArray& data);
    void handleBookTicketRequest(const QByteArray& data);
    void handleMyOrdersRequest(const QByteArray& data);
    void handleGetUserInfoRequest(const QByteArray& data);
    void handleUpdateUserInfoRequest(const QByteArray& data);
    void handleCancelTicketRequest(const QByteArray& data);
    void handleRegisterRequest(const QByteArray& data);
    void handleChangeTicketRequest(const QByteArray& data);
    void handleCheckUsernameRequest(const QByteArray& data);

    void sendResponse(ResponseStatus status, const QByteArray& data = QByteArray());
};

#endif // CLIENT_HANDLER_H
