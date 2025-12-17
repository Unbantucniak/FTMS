#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <QString>
#include <QDateTime>
#include <QDataStream>
#include <QList>

// 请求类型
enum RequestType {
    LoginRequest = 1,       // 登录请求
    FlightQueryRequest,     // 航班查询请求
    BookTicketRequest,      // 订票请求
    MyOrdersRequest,        // 我的订单请求
    GetUserInfoRequest,     // 获取用户信息请求
    UpdateUserInfoRequest,  // 更新用户信息请求
    CancelTicketRequest,    // 取消订单请求
    RegisterRequest,        // 注册请求
    ChangeTicketRequest,    // 改签请求
    CheckUsernameRequest,   // 检查用户名请求
    GetCitiesRequest,       // 获取城市列表请求
    GetOccupiedSeatsRequest,// 获取已占座位请求
    AIChatRequest,          // AI对话请求
    ChangePasswordRequest   // 修改密码请求
};

// 响应结果
enum ResponseStatus {
    Success = 0,            // 成功
    Failed,                 // 失败
    UserNotFound,           // 用户不存在
    PasswordError,          // 密码错误
    FlightNotFound,         // 航班不存在
    NoSeatsLeft,            // 无剩余座位
    UsernameExist           // 用户名已存在
};

// 用户结构体
struct User {
    QString username;       // 用户名
    QString password;       // 密码
    QString real_name;      // 真实姓名
    QString phone;          // 联系方式

    friend QDataStream& operator<<(QDataStream& out, const User& user) {
        out << user.username << user.password << user.real_name << user.phone;
        return out;
    }
    friend QDataStream& operator>>(QDataStream& in, User& user) {
        in >> user.username >> user.password >> user.real_name >> user.phone;
        return in;
    }
};

// 航班结构体
struct Flight {
    QString flight_id;      // 航班号
    QString departure;      // 出发地
    QString destination;    // 目的地
    QString departure_airport; // 出发机场
    QString arrival_airport;   // 到达机场
    QDateTime depart_time;  // 出发时间
    QDateTime arrive_time;  // 到达时间
    double price;           // 票价
    int rest_seats;         // 剩余座位

    // QDataStream序列化
    friend QDataStream& operator<<(QDataStream& out, const Flight& flight) {
        out << flight.flight_id << flight.departure << flight.destination
            << flight.departure_airport << flight.arrival_airport
            << flight.depart_time << flight.arrive_time 
            << flight.price << flight.rest_seats;       
        return out;
    }
    friend QDataStream& operator>>(QDataStream& in, Flight& flight) {
        in >> flight.flight_id >> flight.departure >> flight.destination
           >> flight.departure_airport >> flight.arrival_airport
           >> flight.depart_time >> flight.arrive_time 
           >> flight.price >> flight.rest_seats;       
        return in;
    }
};

struct Order {
    QString order_id;       // 订单号
    QString username;       // 用户名
    QString flight_id;      // 关联航班号
    QDateTime book_time;    // 订票时间
    QString seat_number;    // 座位号
    
    // 行程信息
    QString departure;
    QString destination;
    QString departure_airport;
    QString arrival_airport;
    QDateTime depart_time;
    QDateTime arrive_time;

    friend QDataStream& operator<<(QDataStream& out, const Order& order) {
        out << order.order_id << order.username << order.flight_id << order.book_time << order.seat_number
            << order.departure << order.destination << order.departure_airport << order.arrival_airport 
            << order.depart_time << order.arrive_time;
        return out;
    }
    friend QDataStream& operator>>(QDataStream& in, Order& order) {
        in >> order.order_id >> order.username >> order.flight_id >> order.book_time >> order.seat_number
           >> order.departure >> order.destination >> order.departure_airport >> order.arrival_airport
           >> order.depart_time >> order.arrive_time;
        return in;
    }
};

#endif // DATA_MODEL_H