#include "client_handler.h"
#include "../ai/ai_manager.h"
#include "db/db_manager.h"
#include <QDebug>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), m_socketDescriptor(socketDescriptor) {}

void ClientHandler::run() {
    m_socket = new QTcpSocket();
    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "客户端连接失败：" << m_socket->errorString();
        return;
    }
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead, Qt::DirectConnection);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected, Qt::DirectConnection);
    
    m_aiManager = new AIManager();

    m_recvBuffer.clear();
    m_expectedSize = 0;
    qDebug() << "客户端连接成功，等待数据...";
    exec();
    
    delete m_aiManager;  m_aiManager = nullptr;
}

void ClientHandler::onReadyRead() {
    m_recvBuffer.append(m_socket->readAll());
    
    while (true) {
        if (m_expectedSize == 0) {
            if (m_recvBuffer.size() < (int)sizeof(quint32)) {
                return;
            }
            QDataStream sizeStream(m_recvBuffer.left(sizeof(quint32)));
            sizeStream.setVersion(QDataStream::Qt_6_0);
            sizeStream >> m_expectedSize;
            m_recvBuffer.remove(0, sizeof(quint32));
        }
        if ((quint32)m_recvBuffer.size() < m_expectedSize) {
            return; 
        }
        QByteArray packet = m_recvBuffer.left(m_expectedSize);
        m_recvBuffer.remove(0, m_expectedSize);
        m_expectedSize = 0;
        processPacket(packet);
    }
}

// 解析请求类型并分发
void ClientHandler::processPacket(const QByteArray& packet) {
    QDataStream in(packet);
    in.setVersion(QDataStream::Qt_6_0);

    int requestType;
    in >> requestType;

    QByteArray data;
    in >> data;

    switch (requestType) {
    case LoginRequest:
        handleLoginRequest(data);
        break;
    case FlightQueryRequest:
        handleFlightQueryRequest(data);
        break;
    case BookTicketRequest:
        handleBookTicketRequest(data);
        break;
    case MyOrdersRequest:
        handleMyOrdersRequest(data);
        break;
    case GetUserInfoRequest:
        handleGetUserInfoRequest(data);
        break;
    case UpdateUserInfoRequest:
        handleUpdateUserInfoRequest(data);
        break;
    case CancelTicketRequest:
        handleCancelTicketRequest(data);
        break;
    case RegisterRequest:
        handleRegisterRequest(data);
        break;
    case ChangeTicketRequest:
        handleChangeTicketRequest(data);
        break;
    case CheckUsernameRequest:
        handleCheckUsernameRequest(data);
        break;
    case GetCitiesRequest:
        handleGetCitiesRequest(data);
        break;
    case GetOccupiedSeatsRequest:
        handleGetOccupiedSeatsRequest(data);
        break;
    case AIChatRequest:
        handleAIChatRequest(data);
        break;
    case ChangePasswordRequest:
        handleChangePasswordRequest(data);
        break;
    default:
        sendResponse(Failed);
        qDebug() << "收到未知请求类型：" << requestType;
        break;
    }
}

void ClientHandler::handleLoginRequest(const QByteArray& data) {
    QDataStream in(data);
    User user;
    in >> user;

    ResponseStatus status = DBManager::getInstance()->verifyUser(user.username, user.password);
    sendResponse(status);

    qDebug() << "登录请求 - 用户名：" << user.username << " 验证结果：" << (status == Success ? "成功" : "失败");
}

void ClientHandler::handleFlightQueryRequest(const QByteArray& data) {
    QDataStream in(data);
    QString departure, destination;
    QDate date;
    in >> departure >> destination >> date;

    QList<Flight> flights = DBManager::getInstance()->queryFlights(departure, destination, date);

    QByteArray responseData;
    QDataStream out(&responseData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << static_cast<quint32>(flights.size());
    for (const Flight& flight : flights) {
        out << flight;
    }

    ResponseStatus status = flights.isEmpty() ? FlightNotFound : Success;
    sendResponse(status, responseData);

    qDebug() << "航班查询请求 - 出发地：" << departure << " 目的地：" << destination << " 日期：" << date << " 查到航班数：" << flights.size();
}

void ClientHandler::handleBookTicketRequest(const QByteArray& data) {
    QDataStream in(data);
    QString username, flight_id, seat_number;
    in >> username >> flight_id >> seat_number;

    QString orderId;
    if (seat_number.isEmpty()) {
        orderId = DBManager::getInstance()->bookTicket(username, flight_id);
    } else {
        orderId = DBManager::getInstance()->bookTicketWithSeat(username, flight_id, seat_number);
    }

    QByteArray responseData;
    QDataStream out(&responseData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << orderId;

    ResponseStatus status = Success;
    if (orderId.isEmpty()) {
        const int restSeats = DBManager::getInstance()->getRestSeats(flight_id);
        if (restSeats == -1) {
            status = FlightNotFound;
        } else if (restSeats == 0) {
            status = NoSeatsLeft;
        } else {
            status = Failed;
        }
    }

    sendResponse(status, responseData);
    qDebug() << "订票请求 - 用户名：" << username << " 航班号：" << flight_id << " 座位：" << seat_number << " 订单号：" << (orderId.isEmpty() ? "无" : orderId);
}

void ClientHandler::handleMyOrdersRequest(const QByteArray& data) {
    QDataStream in(data);
    QString username;
    in >> username;

    QList<Order> orders = DBManager::getInstance()->queryUserOrders(username);

    QByteArray responseData;
    QDataStream out(&responseData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << static_cast<quint32>(orders.size());
    for (const Order& order : orders) {
        out << order;
    }

    sendResponse(Success, responseData);
    qDebug() << "订单查询请求 - 用户名：" << username << " 查到订单数：" << orders.size();
}

void ClientHandler::handleGetUserInfoRequest(const QByteArray& data) {
    QDataStream in(data);
    QString username;
    in >> username;

    User user = DBManager::getInstance()->getUserInfo(username);

    QByteArray responseData;
    QDataStream out(&responseData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << user;

    ResponseStatus status = user.username.isEmpty() ? UserNotFound : Success;
    sendResponse(status, responseData);
    qDebug() << "用户信息请求 - 用户名：" << username << " 结果：" << (status == Success ? "成功" : "失败");
}

void ClientHandler::handleUpdateUserInfoRequest(const QByteArray& data) {
    QDataStream in(data);
    User user;
    in >> user;

    bool success = DBManager::getInstance()->updateUserInfo(user);
    sendResponse(success ? Success : Failed);

    qDebug() << "更新用户信息请求 - 用户名：" << user.username << " 结果：" << (success ? "成功" : "失败");
}

void ClientHandler::handleCancelTicketRequest(const QByteArray& data) {
    QDataStream in(data);
    QString orderId;
    in >> orderId;

    bool success = DBManager::getInstance()->cancelTicket(orderId);
    sendResponse(success ? Success : Failed);

    qDebug() << "取消订单请求 - 订单号：" << orderId << " 结果：" << (success ? "成功" : "失败");
}

void ClientHandler::handleRegisterRequest(const QByteArray& data) {
    QDataStream in(data);
    User user;
    in >> user;

    bool success = DBManager::getInstance()->registerUser(user);
    sendResponse(success ? Success : Failed);

    qDebug() << "注册请求 - 用户名：" << user.username << " 结果：" << (success ? "成功" : "失败");
}

void ClientHandler::handleChangeTicketRequest(const QByteArray& data) {
    QDataStream in(data);
    QString orderId, newFlightId, seatNumber;
    in >> orderId >> newFlightId >> seatNumber;

    bool success = DBManager::getInstance()->changeTicket(orderId, newFlightId, seatNumber);
    sendResponse(success ? Success : Failed);

    qDebug() << "改签请求 - 订单号：" << orderId << " 新航班：" << newFlightId << " 座位：" << seatNumber << " 结果：" << (success ? "成功" : "失败");
}

void ClientHandler::handleCheckUsernameRequest(const QByteArray& data) {
    QDataStream in(data);
    QString username;
    in >> username;

    bool exist = DBManager::getInstance()->isUserExist(username);
    sendResponse(exist ? UsernameExist : Success);
    
    qDebug() << "检查用户名请求 - 用户名：" << username << " 存在：" << exist;
}

void ClientHandler::handleGetCitiesRequest(const QByteArray& /*data*/) {
    QStringList cities = DBManager::getInstance()->getCities();

    QByteArray responseData;
    QDataStream out(&responseData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << cities;

    sendResponse(Success, responseData);
    qDebug() << "城市列表请求 - 返回城市数：" << cities.size();
}

void ClientHandler::handleGetOccupiedSeatsRequest(const QByteArray& data) {
    QDataStream in(data);
    QString flightId;
    in >> flightId;

    QStringList seats = DBManager::getInstance()->getOccupiedSeats(flightId);

    QByteArray responseData;
    QDataStream out(&responseData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << seats;

    sendResponse(Success, responseData);
    qDebug() << "已占座位请求 - 航班：" << flightId << " 已占座位数：" << seats.size();
}

// 小助手：先拼 payload 再写长度
void ClientHandler::sendResponse(ResponseStatus status, const QByteArray& data) {
    QByteArray payload;
    QDataStream payloadOut(&payload, QIODevice::WriteOnly);
    payloadOut.setVersion(QDataStream::Qt_6_0);
    payloadOut << status << data;
    
    QByteArray packet;

    
    QDataStream packetOut(&packet, QIODevice::WriteOnly);
    packetOut.setVersion(QDataStream::Qt_6_0);
    packetOut << (quint32)payload.size();
    packet.append(payload);
    
    m_socket->write(packet);
    m_socket->flush();
}


void ClientHandler::handleAIChatRequest(const QByteArray& data) {
    QDataStream in(data);
    QString username, message;
    in >> username >> message;

    QString context = "";

    QMetaObject::Connection *conn = new QMetaObject::Connection;
    *conn = connect(m_aiManager, &AIManager::responseReceived, this, [this, conn](const QString& response) {
        QByteArray responseData;
        QDataStream out(&responseData, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_0);
        out << response;
        sendResponse(Success, responseData);
        QObject::disconnect(*conn);
        delete conn;
    });
    
    QMetaObject::Connection *errConn = new QMetaObject::Connection;
    *errConn = connect(m_aiManager, &AIManager::errorOccurred, this, [this, errConn](const QString& error) {
        QByteArray responseData;
        QDataStream out(&responseData, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_0);
        out << error;
        sendResponse(Failed, responseData);
        QObject::disconnect(*errConn);
        delete errConn;
    });

    m_aiManager->sendMessage(message, context);
}

void ClientHandler::handleChangePasswordRequest(const QByteArray& data) {
    QDataStream in(data);
    QString username, oldPass, newPass;
    in >> username >> oldPass >> newPass;

    bool success = DBManager::getInstance()->changePassword(username, oldPass, newPass);
    sendResponse(success ? Success : Failed);

    qDebug() << "修改密码请求 - 用户名：" << username << " 结果：" << (success ? "成功" : "失败");
}

void ClientHandler::onDisconnected() {
    qDebug() << "客户端断开连接，描述符：" << m_socketDescriptor;
    m_socket->close();
    m_socket->deleteLater();
    quit();
}
