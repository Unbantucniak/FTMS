#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>
#include <QDate>
#include <QMutex>
#include <QHash>
#include <QThread>
#include "data_model.h"

class DBManager {
public:
    static DBManager* getInstance();

    // 初始化数据库文件路径和表结构
    bool init(const QString& dbPath = "ftms.db");

    ResponseStatus verifyUser(const QString& username, const QString& password);

    QList<Flight> queryFlights(const QString& departure,
                               const QString& destination,
                               const QDate& date);

    QString bookTicket(const QString& username, const QString& flight_id);
    QString bookTicketWithSeat(const QString& username, const QString& flightId, const QString& seatNumber);

    QList<Order> queryUserOrders(const QString& username);
    User getUserInfo(const QString& username);
    bool updateUserInfo(const User& user);
    bool cancelTicket(const QString& orderId);
    bool changeTicket(const QString& orderId, const QString& newFlightId, const QString& seatNumber);
    bool changePassword(const QString& username, const QString& oldPass, const QString& newPass);

    bool registerUser(const User& user);
    bool addFlight(const Flight& flight);
    bool isUserExist(const QString& username);

    int getRestSeats(const QString& flight_id);
    QStringList getCities();
    QStringList getOccupiedSeats(const QString& flightId);
    QList<Flight> getAllFlights(int limit = 20);
    void close();

private:
    DBManager();
    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

    bool createTables();

    QSqlDatabase getDb();

    QString m_dbPath;
    QHash<Qt::HANDLE, QString> m_connectionNames;
    QMutex m_mutex;
    static DBManager* m_instance;
};

#endif
