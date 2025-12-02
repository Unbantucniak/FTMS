#include "order_card.h"
#include <QGraphicsDropShadowEffect>

OrderCard::OrderCard(const Order& order, QWidget *parent) 
    : QWidget(parent), m_order(order) 
{
    setObjectName("OrderCard");
    setAttribute(Qt::WA_StyledBackground, true);
    setupUI();
}

void OrderCard::setupUI() {
    // 样式已移至 MainWindow 的全局主题中统一管理

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 4);
    this->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(this);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    // 上部分：主要信息
    QWidget *topWidget = new QWidget(this);
    topWidget->setObjectName("CardBody");
    QHBoxLayout *mainLayout = new QHBoxLayout(topWidget);
    mainLayout->setContentsMargins(20, 20, 20, 10);
    mainLayout->setSpacing(20);

    // 航班号和日期
    QVBoxLayout *infoLayout = new QVBoxLayout();
    QLabel *flightId = new QLabel(m_order.flight_id, this);
    flightId->setObjectName("FlightId");
    QLabel *date = new QLabel(m_order.depart_time.toString("yyyy-MM-dd"), this);
    date->setObjectName("DurationLabel"); // Use DurationLabel style for date
    infoLayout->addWidget(flightId);
    infoLayout->addWidget(date);
    infoLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 行程信息
    QVBoxLayout *routeLayout = new QVBoxLayout();
    QHBoxLayout *timeLayout = new QHBoxLayout();
    
    QLabel *depTime = new QLabel(m_order.depart_time.toString("HH:mm"), this);
    depTime->setObjectName("TimeLabel");
    QLabel *arrow = new QLabel("➔", this);
    arrow->setObjectName("ArrowLabel");
    arrow->setStyleSheet("margin: 0 10px;"); // Keep margin
    QLabel *arrTime = new QLabel(m_order.arrive_time.toString("HH:mm"), this);
    arrTime->setObjectName("TimeLabel");
    
    timeLayout->addWidget(depTime);
    timeLayout->addWidget(arrow);
    timeLayout->addWidget(arrTime);
    
    QHBoxLayout *airportLayout = new QHBoxLayout();
    
    // 出发地信息
    QVBoxLayout *depInfo = new QVBoxLayout();
    QLabel *depCity = new QLabel(m_order.departure, this);
    depCity->setObjectName("CityLabel");
    depInfo->addWidget(depCity);
    if(!m_order.departure_airport.isEmpty()) {
        QLabel *depAirport = new QLabel(m_order.departure_airport, this);
        depAirport->setObjectName("AirportLabel");
        depInfo->addWidget(depAirport);
    }
    depInfo->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    
    // 目的地信息
    QVBoxLayout *arrInfo = new QVBoxLayout();
    QLabel *arrCity = new QLabel(m_order.destination, this);
    arrCity->setObjectName("CityLabel");
    arrInfo->addWidget(arrCity);
    if(!m_order.arrival_airport.isEmpty()) {
        QLabel *arrAirport = new QLabel(m_order.arrival_airport, this);
        arrAirport->setObjectName("AirportLabel");
        arrInfo->addWidget(arrAirport);
    }
    arrInfo->setAlignment(Qt::AlignRight | Qt::AlignTop);
    
    airportLayout->addLayout(depInfo);
    airportLayout->addStretch();
    airportLayout->addLayout(arrInfo);

    routeLayout->addLayout(timeLayout);
    routeLayout->addLayout(airportLayout);

    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(routeLayout);
    mainLayout->addStretch();

    // 下部分：座位和操作 (底部颜色块)
    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setObjectName("InfoContainer");
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(20, 12, 20, 12);
    bottomLayout->setSpacing(15);

    // 座位信息
    QLabel *seat = new QLabel(QString("座位: %1").arg(m_order.seat_number.isEmpty() ? "未分配" : m_order.seat_number), this);
    seat->setObjectName("SeatLabel");

    // 按钮
    QPushButton *changeBtn = new QPushButton("改签", this);
    changeBtn->setObjectName("ChangeBtn");
    changeBtn->setMinimumWidth(120);
    changeBtn->setFixedHeight(40);
    changeBtn->setCursor(Qt::PointingHandCursor);
    connect(changeBtn, &QPushButton::clicked, this, [this](){ emit changeRequested(m_order.order_id); });

    QPushButton *cancelBtn = new QPushButton("取消订单", this);
    cancelBtn->setObjectName("CancelBtn");
    cancelBtn->setMinimumWidth(120);
    cancelBtn->setFixedHeight(40);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, [this](){ emit cancelRequested(m_order.order_id); });

    bottomLayout->addWidget(seat);
    bottomLayout->addStretch();
    bottomLayout->addWidget(changeBtn);
    bottomLayout->addWidget(cancelBtn);

    cardLayout->addWidget(topWidget);
    cardLayout->addWidget(bottomWidget);
}