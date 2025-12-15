#include "db_manager.h"
#include <QUuid>
#include <QRandomGenerator>
#include <QDateTime>
#include <QCoreApplication>
#include <QFileInfo>

DBManager* DBManager::m_instance = nullptr;

DBManager* DBManager::getInstance() {
    if (!m_instance) {
        m_instance = new DBManager();
    }
    return m_instance;
}

DBManager::DBManager() {}

// ==================== SQLite 初始化 ====================
bool DBManager::init(const QString& dbPath) {
    if (m_db.isOpen()) return true;

    // 使用 SQLite 驱动（Qt 内置，无需额外安装）
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "❌ SQLite 数据库打开失败：" << m_db.lastError().text();
        return false;
    }

    qDebug() << "✅ SQLite 数据库连接成功：" << QFileInfo(dbPath).absoluteFilePath();

    // 启用外键约束
    QSqlQuery query(m_db);
    query.exec("PRAGMA foreign_keys = ON;");

    // 创建表结构（如果不存在）
    if (!createTables()) {
        qDebug() << "❌ 创建数据库表失败";
        return false;
    }

    return true;
}

// ==================== 创建表结构 ====================
bool DBManager::createTables() {
    QSqlQuery query(m_db);

    // 用户表
    QString createUserTable = R"(
        CREATE TABLE IF NOT EXISTS user (
            username    TEXT PRIMARY KEY,
            password    TEXT NOT NULL,
            real_name   TEXT,
            phone       TEXT
        )
    )";

    // 航班表
    QString createFlightTable = R"(
        CREATE TABLE IF NOT EXISTS flight (
            flight_id           TEXT PRIMARY KEY,
            departure           TEXT NOT NULL,
            destination         TEXT NOT NULL,
            departure_airport   TEXT,
            arrival_airport     TEXT,
            depart_time         TEXT NOT NULL,
            arrive_time         TEXT NOT NULL,
            price               REAL NOT NULL,
            rest_seats          INTEGER NOT NULL
        )
    )";

    // 订票表
    QString createTicketTable = R"(
        CREATE TABLE IF NOT EXISTS ticket (
            order_id    TEXT PRIMARY KEY,
            username    TEXT NOT NULL,
            flight_id   TEXT NOT NULL,
            book_time   TEXT NOT NULL,
            status      INTEGER DEFAULT 1,
            seat_number TEXT,
            FOREIGN KEY (username) REFERENCES user(username),
            FOREIGN KEY (flight_id) REFERENCES flight(flight_id)
        )
    )";

    if (!query.exec(createUserTable)) {
        qDebug() << "创建 user 表失败：" << query.lastError().text();
        return false;
    }

    if (!query.exec(createFlightTable)) {
        qDebug() << "创建 flight 表失败：" << query.lastError().text();
        return false;
    }

    if (!query.exec(createTicketTable)) {
        qDebug() << "创建 ticket 表失败：" << query.lastError().text();
        return false;
    }

    // 创建索引以提高查询性能
    query.exec("CREATE INDEX IF NOT EXISTS idx_flight_departure ON flight(departure)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_flight_destination ON flight(destination)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_flight_depart_time ON flight(depart_time)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_ticket_username ON ticket(username)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_ticket_flight ON ticket(flight_id)");

    qDebug() << "✅ 数据库表结构创建成功";
    return true;
}

// ==================== 用户相关 ====================
bool DBManager::registerUser(const User& user) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query(m_db);
    query.prepare("SELECT username FROM user WHERE username = :username");
    query.bindValue(":username", user.username);
    if (query.exec() && query.next()) {
        return false;  // 用户名已存在
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
    }
    return user;
}

bool DBManager::updateUserInfo(const User& user) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query(m_db);
    query.prepare("UPDATE user SET phone = :phone WHERE username = :username");
    query.bindValue(":phone", user.phone);
    query.bindValue(":username", user.username);

    return query.exec();
}

bool DBManager::changePassword(const QString& username, const QString& oldPass, const QString& newPass) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query(m_db);
    
    // 验证旧密码
    query.prepare("SELECT password FROM user WHERE username = :username");
    query.bindValue(":username", username);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "修改密码失败：用户不存在" << username;
        return false;
    }
    
    if (query.value(0).toString() != oldPass) {
        qDebug() << "修改密码失败：旧密码不正确";
        return false;
    }
    
    // 更新密码
    query.prepare("UPDATE user SET password = :newPass WHERE username = :username");
    query.bindValue(":newPass", newPass);
    query.bindValue(":username", username);
    
    if (query.exec()) {
        qDebug() << "用户" << username << "密码修改成功";
        return true;
    }
    return false;
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

// ==================== 航班查询 ====================
QList<Flight> DBManager::queryFlights(const QString& departure, const QString& destination, const QDate& date) {
    QList<Flight> flights;
    if (!m_db.isOpen()) return flights;

    QString sql = "SELECT flight_id, departure, destination, departure_airport, arrival_airport, "
                  "depart_time, arrive_time, price, rest_seats FROM flight WHERE rest_seats > 0";
    
    if (!departure.isEmpty())   sql += " AND departure LIKE :departure";
    if (!destination.isEmpty()) sql += " AND destination LIKE :destination";
    if (date.isValid())         sql += " AND DATE(depart_time) = :date";
    sql += " ORDER BY depart_time ASC";

    QSqlQuery query(m_db);
    query.prepare(sql);
    if (!departure.isEmpty())   query.bindValue(":departure", "%" + departure + "%");
    if (!destination.isEmpty()) query.bindValue(":destination", "%" + destination + "%");
    if (date.isValid())         query.bindValue(":date", date.toString("yyyy-MM-dd"));

    if (query.exec()) {
        while (query.next()) {
            Flight f;
            f.flight_id = query.value(0).toString();
            f.departure = query.value(1).toString();
            f.destination = query.value(2).toString();
            f.departure_airport = query.value(3).toString();
            f.arrival_airport = query.value(4).toString();
            f.depart_time = QDateTime::fromString(query.value(5).toString(), Qt::ISODate);
            f.arrive_time = QDateTime::fromString(query.value(6).toString(), Qt::ISODate);
            f.price = query.value(7).toDouble();
            f.rest_seats = query.value(8).toInt();
            flights.append(f);
        }
    }
    return flights;
}

QList<Flight> DBManager::getAllFlights(int limit) {
    QList<Flight> flights;
    if (!m_db.isOpen()) return flights;

    QSqlQuery query(m_db);
    // SQLite 使用 datetime('now') 代替 NOW()
    query.prepare(QString("SELECT flight_id, departure, destination, departure_airport, arrival_airport, "
                  "depart_time, arrive_time, price, rest_seats FROM flight "
                  "WHERE depart_time > datetime('now', 'localtime') ORDER BY depart_time ASC LIMIT %1").arg(limit));
    
    if (query.exec()) {
        while (query.next()) {
            Flight f;
            f.flight_id = query.value(0).toString();
            f.departure = query.value(1).toString();
            f.destination = query.value(2).toString();
            f.departure_airport = query.value(3).toString();
            f.arrival_airport = query.value(4).toString();
            f.depart_time = QDateTime::fromString(query.value(5).toString(), Qt::ISODate);
            f.arrive_time = QDateTime::fromString(query.value(6).toString(), Qt::ISODate);
            f.price = query.value(7).toDouble();
            f.rest_seats = query.value(8).toInt();
            flights.append(f);
        }
    }
    return flights;
}

QStringList DBManager::getCities() {
    QStringList cities;
    if (!m_db.isOpen()) return cities;

    QSqlQuery query(m_db);
    query.exec("SELECT DISTINCT departure FROM flight UNION SELECT DISTINCT destination FROM flight ORDER BY 1");
    while (query.next()) {
        cities.append(query.value(0).toString());
    }
    return cities;
}

int DBManager::getRestSeats(const QString& flight_id) {
    if (!m_db.isOpen()) return -2;

    QSqlQuery query(m_db);
    query.prepare("SELECT rest_seats FROM flight WHERE flight_id = :flight_id");
    query.bindValue(":flight_id", flight_id);
    
    if (!query.exec()) return -2;
    if (!query.next()) return -1;
    return query.value(0).toInt();
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

bool DBManager::addFlight(const Flight& flight) {
    if (!m_db.isOpen()) return false;

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO flight (flight_id, departure, destination, departure_airport, arrival_airport, "
                  "depart_time, arrive_time, price, rest_seats) "
                  "VALUES (:id, :dep, :dest, :dep_airport, :arr_airport, :dtime, :atime, :price, :seats)");
    query.bindValue(":id", flight.flight_id);
    query.bindValue(":dep", flight.departure);
    query.bindValue(":dest", flight.destination);
    query.bindValue(":dep_airport", flight.departure_airport);
    query.bindValue(":arr_airport", flight.arrival_airport);
    query.bindValue(":dtime", flight.depart_time.toString(Qt::ISODate));
    query.bindValue(":atime", flight.arrive_time.toString(Qt::ISODate));
    query.bindValue(":price", flight.price);
    query.bindValue(":seats", flight.rest_seats);

    return query.exec();
}

// ==================== 订票相关 ====================
QString DBManager::bookTicket(const QString& username, const QString& flight_id) {
    if (!m_db.isOpen()) return "";
    
    m_db.transaction();

    QSqlQuery query(m_db);
    
    // 检查余票（SQLite 不支持 FOR UPDATE，但单线程访问问题不大）
    query.prepare("SELECT rest_seats FROM flight WHERE flight_id = :flight_id");
    query.bindValue(":flight_id", flight_id);
    if (!query.exec() || !query.next()) {
        m_db.rollback();
        return "";
    }

    int remaining = query.value(0).toInt();
    if (remaining <= 0) {
        m_db.rollback();
        return "";
    }

    // 生成订单和座位
    QString orderId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    int row = QRandomGenerator::global()->bounded(1, 31);
    char col = 'A' + QRandomGenerator::global()->bounded(0, 6);
    QString seatNumber = QString("%1%2").arg(row).arg(col);
    QString bookTime = QDateTime::currentDateTime().toString(Qt::ISODate);

    // 插入订单
    query.prepare("INSERT INTO ticket (order_id, username, flight_id, book_time, status, seat_number) "
                  "VALUES (:order_id, :username, :flight_id, :book_time, 1, :seat_number)");
    query.bindValue(":order_id", orderId);
    query.bindValue(":username", username);
    query.bindValue(":flight_id", flight_id);
    query.bindValue(":book_time", bookTime);
    query.bindValue(":seat_number", seatNumber);
    
    if (!query.exec()) {
        m_db.rollback();
        return "";
    }

    // 更新余票
    query.prepare("UPDATE flight SET rest_seats = rest_seats - 1 WHERE flight_id = :flight_id");
    query.bindValue(":flight_id", flight_id);
    if (!query.exec()) {
        m_db.rollback();
        return "";
    }

    m_db.commit();
    return orderId;
}

QString DBManager::bookTicketWithSeat(const QString& username, const QString& flightId, const QString& seatNumber) {
    if (!m_db.isOpen()) return "";
    
    m_db.transaction();

    QSqlQuery query(m_db);
    
    // 检查余票
    query.prepare("SELECT rest_seats FROM flight WHERE flight_id = :flightId");
    query.bindValue(":flightId", flightId);
    if (!query.exec() || !query.next() || query.value(0).toInt() <= 0) {
        m_db.rollback();
        return "";
    }

    // 检查座位是否已被占用
    query.prepare("SELECT COUNT(*) FROM ticket WHERE flight_id = :flightId AND seat_number = :seat");
    query.bindValue(":flightId", flightId);
    query.bindValue(":seat", seatNumber);
    if (!query.exec() || !query.next() || query.value(0).toInt() > 0) {
        m_db.rollback();
        return "";
    }

    QString orderId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString bookTime = QDateTime::currentDateTime().toString(Qt::ISODate);

    query.prepare("INSERT INTO ticket (order_id, username, flight_id, book_time, status, seat_number) "
                  "VALUES (:orderId, :username, :flightId, :bookTime, 1, :seat)");
    query.bindValue(":orderId", orderId);
    query.bindValue(":username", username);
    query.bindValue(":flightId", flightId);
    query.bindValue(":bookTime", bookTime);
    query.bindValue(":seat", seatNumber);
    
    if (!query.exec()) {
        m_db.rollback();
        return "";
    }

    query.prepare("UPDATE flight SET rest_seats = rest_seats - 1 WHERE flight_id = :flightId");
    query.bindValue(":flightId", flightId);
    if (!query.exec()) {
        m_db.rollback();
        return "";
    }

    m_db.commit();
    return orderId;
}

QList<Order> DBManager::queryUserOrders(const QString& username) {
    QList<Order> orders;
    if (!m_db.isOpen()) return orders;

    QSqlQuery query(m_db);
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
            order.book_time = QDateTime::fromString(query.value(3).toString(), Qt::ISODate);
            order.seat_number = query.value(4).toString();
            order.departure = query.value(5).toString();
            order.destination = query.value(6).toString();
            order.departure_airport = query.value(7).toString();
            order.arrival_airport = query.value(8).toString();
            order.depart_time = QDateTime::fromString(query.value(9).toString(), Qt::ISODate);
            order.arrive_time = QDateTime::fromString(query.value(10).toString(), Qt::ISODate);
            orders.append(order);
        }
    }
    return orders;
}

bool DBManager::cancelTicket(const QString& orderId) {
    if (!m_db.isOpen()) return false;

    m_db.transaction();

    QSqlQuery query(m_db);
    
    // 获取航班ID
    query.prepare("SELECT flight_id FROM ticket WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec() || !query.next()) {
        m_db.rollback();
        return false;
    }
    QString flightId = query.value(0).toString();

    // 删除订单
    query.prepare("DELETE FROM ticket WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    // 恢复座位
    query.prepare("UPDATE flight SET rest_seats = rest_seats + 1 WHERE flight_id = :flightId");
    query.bindValue(":flightId", flightId);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    m_db.commit();
    return true;
}

bool DBManager::changeTicket(const QString& orderId, const QString& newFlightId) {
    if (!m_db.isOpen()) return false;

    m_db.transaction();

    QSqlQuery query(m_db);
    
    // 获取原订单信息
    query.prepare("SELECT username, flight_id FROM ticket WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec() || !query.next()) {
        m_db.rollback();
        return false;
    }
    QString username = query.value(0).toString();
    QString oldFlightId = query.value(1).toString();

    // 检查新航班余票
    query.prepare("SELECT rest_seats FROM flight WHERE flight_id = :flightId");
    query.bindValue(":flightId", newFlightId);
    if (!query.exec() || !query.next() || query.value(0).toInt() <= 0) {
        m_db.rollback();
        return false;
    }

    // 删除原订单
    query.prepare("DELETE FROM ticket WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    // 恢复原航班座位
    query.prepare("UPDATE flight SET rest_seats = rest_seats + 1 WHERE flight_id = :flightId");
    query.bindValue(":flightId", oldFlightId);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    // 创建新订单
    QString newOrderId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    int row = QRandomGenerator::global()->bounded(1, 31);
    char col = 'A' + QRandomGenerator::global()->bounded(0, 6);
    QString seatNumber = QString("%1%2").arg(row).arg(col);
    QString bookTime = QDateTime::currentDateTime().toString(Qt::ISODate);

    query.prepare("INSERT INTO ticket (order_id, username, flight_id, book_time, status, seat_number) "
                  "VALUES (:orderId, :username, :flightId, :bookTime, 1, :seatNumber)");
    query.bindValue(":orderId", newOrderId);
    query.bindValue(":username", username);
    query.bindValue(":flightId", newFlightId);
    query.bindValue(":bookTime", bookTime);
    query.bindValue(":seatNumber", seatNumber);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    // 扣减新航班座位
    query.prepare("UPDATE flight SET rest_seats = rest_seats - 1 WHERE flight_id = :flightId");
    query.bindValue(":flightId", newFlightId);
    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    m_db.commit();
    return true;
}

void DBManager::close() {
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "数据库已关闭";
    }
}
