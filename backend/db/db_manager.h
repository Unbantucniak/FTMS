#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>
#include <QDate>
#include "data_model.h"

class DBManager {
public:
    static DBManager* getInstance();

    bool init(const QString& host = "localhost",
              int port = 3306,
              const QString& dbName = "FTMSDB",
              const QString& user = "root",
              const QString& pwd = "114514");

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
    bool changeTicket(const QString& orderId, const QString& newFlightId);
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

    QSqlDatabase m_db;
    static DBManager* m_instance;
};

#endif
