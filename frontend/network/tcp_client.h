#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "data_model.h"

class TcpClient : public QObject
{
    Q_OBJECT
public:
    static TcpClient* getInstance();
    void connectToServer(const QString& ip, int port);
    void login(const QString& username, const QString& password);
    void queryFlights(const QString& departure, const QString& destination, const QDate& date);
    void bookTicket(const QString& username, const QString& flightId);
    void queryOrders(const QString& username);
    void getUserInfo(const QString& username);
    void updateUserInfo(const User& user);
    void cancelTicket(const QString& orderId);
    void changeTicket(const QString& orderId, const QString& newFlightId);
    void registerUser(const User& user);
    void checkUsername(const QString& username);

signals:
    void loginResult(ResponseStatus status);
    void registerResult(bool success);
    void checkUsernameResult(bool exist);
    void flightQueryResults(const QList<Flight>& flights);
    void bookTicketResult(bool success, const QString& message);
    void myOrdersResults(const QList<Order>& orders);
    void userInfoResult(const User& user);
    void updateUserInfoResult(bool success);
    void cancelTicketResult(bool success);
    void changeTicketResult(bool success);

private slots:
    void onReadyRead();

private:
    explicit TcpClient(QObject *parent = nullptr);
    QTcpSocket *m_socket;
    static TcpClient *m_instance;
    int m_lastRequestType = 0;
};

#endif // TCP_CLIENT_H
