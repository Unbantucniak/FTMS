#include "db_manager.h"
#include <QUuid>
#include <QRandomGenerator>

DBManager* DBManager::m_instance = nullptr;

DBManager* DBManager::getInstance() {
    if (!m_instance) {
        m_instance = new DBManager();
    }
    return m_instance;
}

DBManager::DBManager() {}

bool DBManager::init(const QString& host, int port, const QString& dbName, const QString& user, const QString& pwd) {
    if (m_db.isOpen()) return true;

    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(pwd);

    bool success = m_db.open();
    if (success) {
        qDebug() << "✅ 数据库连接成功";
    } else {
        qDebug() << "❌ 数据库连接失败：" << m_db.lastError().text();
    }
    return success;
}

bool DBManager::registerUser(const User& user) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query(m_db);
    query.prepare("SELECT username FROM user WHERE username = :username");
    query.bindValue(":username", user.username);
    if (query.exec() && query.next()) {
        return false; // 用户名已存在
    }

    query.prepare("INSERT INTO user (username, password, real_name, phone) VALUES (:username, :password, :real_name, :phone)");
    query.bindValue(":username", user.username);
    query.bindValue(":password", user.password);
    query.bindValue(":real_name", user.real_name);
    query.bindValue(":phone", user.phone);

    return query.exec();
}

ResponseStatus DBManager::verifyUser(const QString& username, const QString& password) {
    if (!m_db.isOpen()) return Failed;

    QSqlQuery query(m_db);
    query.prepare("SELECT password FROM user WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec() || !query.next()) {
        return UserNotFound;
    }

    QString dbPwd = query.value(0).toString();
    return (dbPwd == password) ? Success : PasswordError;
}

QList<Flight> DBManager::queryFlights(const QString& departure, const QString& destination, const QDate& date) {
    QList<Flight> flights;
    if (!m_db.isOpen()) return flights;

    QString sql = "SELECT flight_id, departure, destination, departure_airport, arrival_airport, depart_time, arrive_time, price, rest_seats "
                  "FROM flight WHERE rest_seats > 0";
    // 使用 LIKE 进行部分匹配
    if (!departure.isEmpty())  sql += " AND departure LIKE :departure";
    if (!destination.isEmpty()) sql += " AND destination LIKE :destination";
    if (date.isValid())        sql += " AND DATE(depart_time) = :date";
    // 按起飞时间升序排列
    sql += " ORDER BY depart_time ASC";

    QSqlQuery query(m_db);
    query.prepare(sql);
    if (!departure.isEmpty())  query.bindValue(":departure", "%" + departure + "%");
    if (!destination.isEmpty()) query.bindValue(":destination", "%" + destination + "%");
    if (date.isValid())        query.bindValue(":date", date.toString("yyyy-MM-dd"));

    if (query.exec()) {
        while (query.next()) {
            Flight flight;
            flight.flight_id = query.value(0).toString();
            flight.departure = query.value(1).toString();
            flight.destination = query.value(2).toString();
            flight.departure_airport = query.value(3).toString();
            flight.arrival_airport = query.value(4).toString();
            flight.depart_time = query.value(5).toDateTime();
            flight.arrive_time = query.value(6).toDateTime();
            flight.price = query.value(7).toDouble();
            flight.rest_seats = query.value(8).toInt();
            flights.append(flight);
        }
    } else {
        qDebug() << "航班查询失败：" << query.lastError().text();
    }
    return flights;
}

QString DBManager::bookTicket(const QString& username, const QString& flight_id) {
    if (!m_db.isOpen()) return "";
    if (!m_db.transaction()) {
        qDebug() << "无法开启事务：" << m_db.lastError().text();
        return "";
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT rest_seats FROM flight WHERE flight_id = :flight_id FOR UPDATE");
    query.bindValue(":flight_id", flight_id);
    if (!query.exec() || !query.next()) {
        m_db.rollback();
        qDebug() << "航班不存在：" << flight_id << query.lastError().text();
        return "";
    }

    int remaining = query.value(0).toInt();
    if (remaining <= 0) {
        m_db.rollback();
        qDebug() << "航班无剩余座位：" << flight_id;
        return "";
    }

    QString orderId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    // 简单的座位生成逻辑：随机生成一个座位号 (例如 12A)
    // 实际项目中应该查询已占用的座位并分配一个新的
    int row = QRandomGenerator::global()->bounded(1, 31);
    char col = 'A' + QRandomGenerator::global()->bounded(0, 6);
    QString seatNumber = QString("%1%2").arg(row).arg(col);

    query.prepare("INSERT INTO ticket (order_id, username, flight_id, book_time, status, seat_number) "
                  "VALUES (:order_id, :username, :flight_id, NOW(), 1, :seat_number)");
    query.bindValue(":order_id", orderId);
    query.bindValue(":username", username);
    query.bindValue(":flight_id", flight_id);
    query.bindValue(":seat_number", seatNumber);
    if (!query.exec()) {
        m_db.rollback();
        qDebug() << "订单插入失败：" << query.lastError().text();
        return "";
    }

    query.prepare("UPDATE flight SET rest_seats = rest_seats - 1 WHERE flight_id = :flight_id");
    query.bindValue(":flight_id", flight_id);
    if (!query.exec()) {
        m_db.rollback();
        qDebug() << "座位更新失败：" << query.lastError().text();
        return "";
    }

    if (!m_db.commit()) {
        qDebug() << "事务提交失败：" << m_db.lastError().text();
        m_db.rollback();
        return "";
    }
    return orderId;
}

int DBManager::getRestSeats(const QString& flight_id) {
    if (!m_db.isOpen()) return -2;

    QSqlQuery query(m_db);
    query.prepare("SELECT rest_seats FROM flight WHERE flight_id = :flight_id");
    query.bindValue(":flight_id", flight_id);
    if (!query.exec()) {
        qDebug() << "查询剩余座位失败：" << query.lastError().text();
        return -2;
    }
    if (!query.next()) {
        return -1;
    }
    return query.value(0).toInt();
}

QList<Order> DBManager::queryUserOrders(const QString& username) {
    QList<Order> orders;
    if (!m_db.isOpen()) return orders;

    QSqlQuery query(m_db);
    // 关联航班表以获取行程详情，按出发时间倒序
    query.prepare("SELECT t.order_id, t.username, t.flight_id, t.book_time, t.seat_number, "
                  "f.departure, f.destination, f.departure_airport, f.arrival_airport, f.depart_time, f.arrive_time "
                  "FROM ticket t "
                  "JOIN flight f ON t.flight_id = f.flight_id "
                  "WHERE t.username = :username "
                  "ORDER BY f.depart_time DESC");
    query.bindValue(":username", username);

    if (query.exec()) {
        while (query.next()) {
            Order order;
            order.order_id = query.value(0).toString();
            order.username = query.value(1).toString();
            order.flight_id = query.value(2).toString();
            order.book_time = query.value(3).toDateTime();
            order.seat_number = query.value(4).toString();
            order.departure = query.value(5).toString();
            order.destination = query.value(6).toString();
            order.departure_airport = query.value(7).toString();
            order.arrival_airport = query.value(8).toString();
            order.depart_time = query.value(9).toDateTime();
            order.arrive_time = query.value(10).toDateTime();
            orders.append(order);
        }
    } else {
        qDebug() << "查询订单失败：" << query.lastError().text();
    }
    return orders;
}

User DBManager::getUserInfo(const QString& username) {
    User user;
    if (!m_db.isOpen()) return user;

    QSqlQuery query(m_db);
    query.prepare("SELECT username, password, real_name, phone FROM user WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        user.username = query.value(0).toString();
        user.password = query.value(1).toString();
        user.real_name = query.value(2).toString();
        user.phone = query.value(3).toString();
    } else {
        qDebug() << "查询用户信息失败：" << query.lastError().text();
    }
    return user;
}

bool DBManager::updateUserInfo(const User& user) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query(m_db);
    query.prepare("UPDATE user SET phone = :phone WHERE username = :username");
    query.bindValue(":phone", user.phone);
    query.bindValue(":username", user.username);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "更新用户信息失败：" << query.lastError().text();
        return false;
    }
}

bool DBManager::cancelTicket(const QString& orderId) {
    if (!m_db.isOpen()) return false;

    m_db.transaction();

    QSqlQuery query(m_db);
    
    // 1. 删除前获取航班ID
    query.prepare("SELECT flight_id FROM ticket WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec() || !query.next()) {
        qDebug() << "取消订单失败：找不到订单" << orderId;
        m_db.rollback();
        return false;
    }
    QString flightId = query.value(0).toString();

    // 2. 删除票务记录
    query.prepare("DELETE FROM ticket WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        qDebug() << "取消订单失败：删除记录失败" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 3. 增加剩余座位
    query.prepare("UPDATE flight SET rest_seats = rest_seats + 1 WHERE flight_id = :flightId");
    query.bindValue(":flightId", flightId);
    if (!query.exec()) {
        qDebug() << "取消订单失败：更新座位失败" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    m_db.commit();
    return true;
}

bool DBManager::addFlight(const Flight& flight) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO flight (flight_id, departure, destination, departure_airport, arrival_airport, depart_time, arrive_time, price, rest_seats) "
                  "VALUES (:id, :dep, :dest, :dep_airport, :arr_airport, :dtime, :atime, :price, :seats)");
    query.bindValue(":id", flight.flight_id);
    query.bindValue(":dep", flight.departure);
    query.bindValue(":dest", flight.destination);
    query.bindValue(":dep_airport", flight.departure_airport);
    query.bindValue(":arr_airport", flight.arrival_airport);
    query.bindValue(":dtime", flight.depart_time);
    query.bindValue(":atime", flight.arrive_time);
    query.bindValue(":price", flight.price);
    query.bindValue(":seats", flight.rest_seats);

    if (!query.exec()) {
        qDebug() << "添加航班失败：" << query.lastError().text();
        return false;
    }
    return true;
}

bool DBManager::changeTicket(const QString& orderId, const QString& newFlightId) {
    if (!m_db.isOpen()) return false;

    if (!m_db.transaction()) return false;

    // 1. 获取原订单信息（用户名）
    QSqlQuery query(m_db);
    query.prepare("SELECT username, flight_id FROM ticket WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec() || !query.next()) {
        m_db.rollback();
        return false;
    }
    QString username = query.value(0).toString();
    QString oldFlightId = query.value(1).toString();

    // 2. 检查新航班是否有票
    query.prepare("SELECT rest_seats FROM flight WHERE flight_id = :flightId FOR UPDATE");
    query.bindValue(":flightId", newFlightId);
    if (!query.exec() || !query.next()) {
        m_db.rollback();
        return false;
    }
    int seats = query.value(0).toInt();
    if (seats <= 0) {
        m_db.rollback();
        return false;
    }

    // 3. 删除原订单（或标记为改签，这里直接删除旧的创建新的）
    query.prepare("DELETE FROM ticket WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    // 4. 恢复原航班座位
    query.prepare("UPDATE flight SET rest_seats = rest_seats + 1 WHERE flight_id = :flightId");
    query.bindValue(":flightId", oldFlightId);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    // 5. 创建新订单
    QString newOrderId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    // 生成新座位号
    int row = QRandomGenerator::global()->bounded(1, 31);
    char col = 'A' + QRandomGenerator::global()->bounded(0, 6);
    QString seatNumber = QString("%1%2").arg(row).arg(col);

    query.prepare("INSERT INTO ticket (order_id, username, flight_id, book_time, status, seat_number) "
                  "VALUES (:orderId, :username, :flightId, NOW(), 1, :seatNumber)");
    query.bindValue(":orderId", newOrderId);
    query.bindValue(":username", username);
    query.bindValue(":flightId", newFlightId);
    query.bindValue(":seatNumber", seatNumber);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    // 6. 扣减新航班座位
    query.prepare("UPDATE flight SET rest_seats = rest_seats - 1 WHERE flight_id = :flightId");
    query.bindValue(":flightId", newFlightId);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool DBManager::isUserExist(const QString& username) {
    if (!m_db.isOpen()) return false;
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM user WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

QStringList DBManager::getCities() {
    QStringList cities;
    if (!m_db.isOpen()) return cities;

    QSqlQuery query(m_db);
    // 获取所有不重复的出发地和目的地
    query.prepare("SELECT DISTINCT departure FROM flight UNION SELECT DISTINCT destination FROM flight ORDER BY 1");
    if (query.exec()) {
        while (query.next()) {
            cities.append(query.value(0).toString());
        }
    }
    return cities;
}

QStringList DBManager::getOccupiedSeats(const QString& flightId) {
    QStringList seats;
    if (!m_db.isOpen()) return seats;

    QSqlQuery query(m_db);
    query.prepare("SELECT seat_number FROM ticket WHERE flight_id = :flightId");
    query.bindValue(":flightId", flightId);
    if (query.exec()) {
        while (query.next()) {
            seats.append(query.value(0).toString());
        }
    }
    return seats;
}

QString DBManager::bookTicketWithSeat(const QString& username, const QString& flightId, const QString& seatNumber) {
    if (!m_db.isOpen()) return "";
    if (!m_db.transaction()) {
        qDebug() << "无法开启事务：" << m_db.lastError().text();
        return "";
    }

    QSqlQuery query(m_db);
    // 检查航班是否存在且有余票
    query.prepare("SELECT rest_seats FROM flight WHERE flight_id = :flightId FOR UPDATE");
    query.bindValue(":flightId", flightId);
    if (!query.exec() || !query.next()) {
        m_db.rollback();
        qDebug() << "航班不存在：" << flightId;
        return "";
    }
    int remaining = query.value(0).toInt();
    if (remaining <= 0) {
        m_db.rollback();
        qDebug() << "航班无剩余座位：" << flightId;
        return "";
    }

    // 检查座位是否已被占用
    query.prepare("SELECT COUNT(*) FROM ticket WHERE flight_id = :flightId AND seat_number = :seat");
    query.bindValue(":flightId", flightId);
    query.bindValue(":seat", seatNumber);
    if (!query.exec() || !query.next() || query.value(0).toInt() > 0) {
        m_db.rollback();
        qDebug() << "座位已被占用：" << seatNumber;
        return "";
    }

    QString orderId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    query.prepare("INSERT INTO ticket (order_id, username, flight_id, book_time, status, seat_number) "
                  "VALUES (:orderId, :username, :flightId, NOW(), 1, :seat)");
    query.bindValue(":orderId", orderId);
    query.bindValue(":username", username);
    query.bindValue(":flightId", flightId);
    query.bindValue(":seat", seatNumber);
    if (!query.exec()) {
        m_db.rollback();
        qDebug() << "订单插入失败：" << query.lastError().text();
        return "";
    }

    query.prepare("UPDATE flight SET rest_seats = rest_seats - 1 WHERE flight_id = :flightId");
    query.bindValue(":flightId", flightId);
    if (!query.exec()) {
        m_db.rollback();
        qDebug() << "座位更新失败：" << query.lastError().text();
        return "";
    }

    if (!m_db.commit()) {
        qDebug() << "事务提交失败：" << m_db.lastError().text();
        m_db.rollback();
        return "";
    }
    return orderId;
}

void DBManager::close() {
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "数据库已关闭";
    }
}
