#include "tcp_client.h"
#include <QDataStream>

TcpClient* TcpClient::m_instance = nullptr;

TcpClient* TcpClient::getInstance()
{
    if (!m_instance) {
        m_instance = new TcpClient();
    }
    return m_instance;
}

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

void TcpClient::connectToServer(const QString& ip, int port)
{
    m_socket->connectToHost(ip, port);
    // 重置接收缓冲区
    m_recvBuffer.clear();
    m_expectedSize = 0;
}

// ==================== 辅助函数：发送带长度前缀的数据包 ====================
static void sendPacket(QTcpSocket* socket, const QByteArray& payload)
{
    QByteArray packet;
    QDataStream packetOut(&packet, QIODevice::WriteOnly);
    packetOut.setVersion(QDataStream::Qt_6_0);
    packetOut << (quint32)payload.size();
    packet.append(payload);
    socket->write(packet);
    socket->flush();
}

// ==================== 请求发送方法 ====================
void TcpClient::login(const QString& username, const QString& password)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    User user;
    user.username = username;
    user.password = password;
    
    out << (int)LoginRequest;
    m_lastRequestType = LoginRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << user;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::registerUser(const User& user)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    out << (int)RegisterRequest;
    m_lastRequestType = RegisterRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << user;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::queryFlights(const QString& departure, const QString& destination, const QDate& date)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    out << (int)FlightQueryRequest;
    m_lastRequestType = FlightQueryRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << departure << destination << date;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::bookTicket(const QString& username, const QString& flightId, const QString& seatNumber)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    out << (int)BookTicketRequest;
    m_lastRequestType = BookTicketRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << username << flightId << seatNumber;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::queryOrders(const QString& username)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    out << (int)MyOrdersRequest;
    m_lastRequestType = MyOrdersRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << username;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::getUserInfo(const QString& username)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    out << (int)GetUserInfoRequest;
    m_lastRequestType = GetUserInfoRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << username;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::updateUserInfo(const User& user)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    out << (int)UpdateUserInfoRequest;
    m_lastRequestType = UpdateUserInfoRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << user;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::cancelTicket(const QString& orderId)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    out << (int)CancelTicketRequest;
    m_lastRequestType = CancelTicketRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << orderId;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::changeTicket(const QString& orderId, const QString& newFlightId)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;

    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    
    out << (int)ChangeTicketRequest;
    m_lastRequestType = ChangeTicketRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << orderId << newFlightId;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::checkUsername(const QString& username)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;
    m_lastRequestType = CheckUsernameRequest;
    
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << (int)CheckUsernameRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << username;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::getCities()
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;
    m_lastRequestType = GetCitiesRequest;
    
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << (int)GetCitiesRequest << QByteArray();
    
    sendPacket(m_socket, payload);
}

void TcpClient::getOccupiedSeats(const QString& flightId)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;
    m_lastRequestType = GetOccupiedSeatsRequest;
    
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << (int)GetOccupiedSeatsRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << flightId;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::sendAIChatMessage(const QString& username, const QString& message)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;
    m_lastRequestType = AIChatRequest;
    
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << (int)AIChatRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << username << message;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

void TcpClient::changePassword(const QString& username, const QString& oldPass, const QString& newPass)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) return;
    m_lastRequestType = ChangePasswordRequest;
    
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << (int)ChangePasswordRequest;
    QByteArray requestData;
    QDataStream requestOut(&requestData, QIODevice::WriteOnly);
    requestOut.setVersion(QDataStream::Qt_6_0);
    requestOut << username << oldPass << newPass;
    out << requestData;
    
    sendPacket(m_socket, payload);
}

// ==================== 响应接收处理（带粘包/拆包处理） ====================
void TcpClient::onReadyRead()
{
    // 将新数据追加到缓冲区
    m_recvBuffer.append(m_socket->readAll());
    
    // 循环处理完整的数据包
    while (true) {
        // 如果还没读取长度前缀，且缓冲区足够读取长度（4字节）
        if (m_expectedSize == 0) {
            if (m_recvBuffer.size() < (int)sizeof(quint32)) {
                return; // 数据不足，等待更多数据
            }
            // 读取长度前缀
            QDataStream sizeStream(m_recvBuffer.left(sizeof(quint32)));
            sizeStream.setVersion(QDataStream::Qt_6_0);
            sizeStream >> m_expectedSize;
            m_recvBuffer.remove(0, sizeof(quint32));
        }
        
        // 检查是否收到完整数据包
        if ((quint32)m_recvBuffer.size() < m_expectedSize) {
            return; // 数据不完整，等待更多数据
        }
        
        // 提取完整数据包
        QByteArray packet = m_recvBuffer.left(m_expectedSize);
        m_recvBuffer.remove(0, m_expectedSize);
        m_expectedSize = 0;
        
        // 处理数据包
        processResponse(packet);
    }
}

void TcpClient::processResponse(const QByteArray& packet)
{
    QDataStream in(packet);
    in.setVersion(QDataStream::Qt_6_0);

    int statusInt;
    in >> statusInt;
    ResponseStatus status = (ResponseStatus)statusInt;

    QByteArray data;
    in >> data;

    QDataStream dataIn(data);
    dataIn.setVersion(QDataStream::Qt_6_0);

    switch (m_lastRequestType) {
    case LoginRequest:
        emit loginResult(status);
        break;
    case RegisterRequest:
        emit registerResult(status == Success);
        break;
    case FlightQueryRequest: {
        QList<Flight> flights;
        if (status == Success) {
            quint32 count;
            dataIn >> count;
            for (quint32 i = 0; i < count; ++i) {
                Flight f;
                dataIn >> f;
                flights.append(f);
            }
        }
        emit flightQueryResults(flights);
        break;
    }
    case BookTicketRequest: {
        QString orderId;
        if (status == Success) {
            dataIn >> orderId;
            emit bookTicketResult(true, orderId);
        } else {
            emit bookTicketResult(false, "预订失败");
        }
        break;
    }
    case MyOrdersRequest: {
        QList<Order> orders;
        if (status == Success) {
            quint32 count;
            dataIn >> count;
            for (quint32 i = 0; i < count; ++i) {
                Order o;
                dataIn >> o;
                orders.append(o);
            }
        }
        emit myOrdersResults(orders);
        break;
    }
    case GetUserInfoRequest: {
        User user;
        if (status == Success) {
            dataIn >> user;
        }
        emit userInfoResult(user);
        break;
    }
    case UpdateUserInfoRequest:
        emit updateUserInfoResult(status == Success);
        break;
    case CancelTicketRequest:
        emit cancelTicketResult(status == Success);
        break;
    case ChangeTicketRequest:
        emit changeTicketResult(status == Success);
        break;
    case CheckUsernameRequest:
        emit checkUsernameResult(status == UsernameExist);
        break;
    case GetCitiesRequest: {
        QStringList cities;
        if (status == Success) {
            dataIn >> cities;
        }
        emit citiesResult(cities);
        break;
    }
    case GetOccupiedSeatsRequest: {
        QStringList seats;
        if (status == Success) {
            dataIn >> seats;
        }
        emit occupiedSeatsResult(seats);
        break;
    }
    case AIChatRequest: {
        QString response;
        if (status == Success || status == Failed) {
            dataIn >> response;
        }
        emit aiChatResult(status == Success, response);
        break;
    }
    case ChangePasswordRequest:
        emit changePasswordResult(status == Success);
        break;
    default:
        break;
    }
}
