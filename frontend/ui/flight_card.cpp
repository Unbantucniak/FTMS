#include "flight_card.h"
#include <QGraphicsDropShadowEffect>

FlightCard::FlightCard(const Flight& flight, QWidget *parent) 
    : QWidget(parent), m_flight(flight) 
{
    setObjectName("FlightCard");
    setAttribute(Qt::WA_StyledBackground, true);
    setupUI();
}

void FlightCard::setupUI() {
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

    // 航班号与出发日期
    QLabel *flightIdLabel = new QLabel(m_flight.flight_id, this);
    flightIdLabel->setObjectName("FlightId");

    QLabel *dateLabel = new QLabel(m_flight.depart_time.date().toString("yyyy-MM-dd ddd"), this);
    dateLabel->setObjectName("AirportLabel");

    QVBoxLayout *idLayout = new QVBoxLayout();
    idLayout->setSpacing(4);
    idLayout->setContentsMargins(0, 0, 0, 0);
    idLayout->addWidget(flightIdLabel, 0, Qt::AlignLeft);
    idLayout->addWidget(dateLabel, 0, Qt::AlignLeft);
    
    // 左侧：出发信息
    QVBoxLayout *depLayout = new QVBoxLayout();
    QLabel *depTime = new QLabel(m_flight.depart_time.toString("HH:mm"), this);
    depTime->setObjectName("TimeLabel");
    
    QLabel *depCity = new QLabel(m_flight.departure, this);
    depCity->setObjectName("CityLabel");
    
    QLabel *depAirport = new QLabel(m_flight.departure_airport, this);
    depAirport->setObjectName("AirportLabel");
    
    depLayout->addWidget(depTime);
    depLayout->addWidget(depCity);
    if(!m_flight.departure_airport.isEmpty()) depLayout->addWidget(depAirport);
    depLayout->setAlignment(Qt::AlignCenter);

    // 中间：箭头和时长
    QVBoxLayout *midLayout = new QVBoxLayout();
    qint64 durationSecs = m_flight.depart_time.secsTo(m_flight.arrive_time);
    int hours = durationSecs / 3600;
    int minutes = (durationSecs % 3600) / 60;
    QLabel *duration = new QLabel(QString("%1h %2m").arg(hours).arg(minutes), this);
    duration->setObjectName("DurationLabel");
    duration->setAlignment(Qt::AlignCenter);
    
    QLabel *arrow = new QLabel("──────────➔", this);
    arrow->setObjectName("ArrowLabel");
    arrow->setAlignment(Qt::AlignCenter);

    midLayout->addWidget(duration);
    midLayout->addWidget(arrow);
    midLayout->setAlignment(Qt::AlignCenter);

    // 右侧：到达信息
    QVBoxLayout *arrLayout = new QVBoxLayout();
    QLabel *arrTime = new QLabel(m_flight.arrive_time.toString("HH:mm"), this);
    arrTime->setObjectName("TimeLabel");
    
    QLabel *arrCity = new QLabel(m_flight.destination, this);
    arrCity->setObjectName("CityLabel");
    
    QLabel *arrAirport = new QLabel(m_flight.arrival_airport, this);
    arrAirport->setObjectName("AirportLabel");
    
    arrLayout->addWidget(arrTime);
    arrLayout->addWidget(arrCity);
    if(!m_flight.arrival_airport.isEmpty()) arrLayout->addWidget(arrAirport);
    arrLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addLayout(idLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(depLayout);
    mainLayout->addLayout(midLayout);
    mainLayout->addLayout(arrLayout);
    mainLayout->addStretch();

    // 下部分：价格和操作
    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setObjectName("InfoContainer");
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(24, 14, 24, 14);
    bottomLayout->setSpacing(18);

    QLabel *price = new QLabel(QString("¥%1").arg(m_flight.price), this);
    price->setObjectName("PriceLabel");
    
    QLabel *seats = new QLabel(QString("💺 剩余 %1 座").arg(m_flight.rest_seats), this);
    seats->setObjectName(m_flight.rest_seats <= 10 ? "SeatsLabelLow" : "SeatsLabel");

    QPushButton *bookBtn = new QPushButton("立即预订", this);
    bookBtn->setObjectName("BookBtn");
    bookBtn->setMinimumWidth(140);
    bookBtn->setFixedHeight(42);
    bookBtn->setCursor(Qt::PointingHandCursor);
    connect(bookBtn, &QPushButton::clicked, this, [this](){ emit bookRequested(m_flight); });

    bottomLayout->addWidget(price);
    bottomLayout->addStretch();
    bottomLayout->addWidget(seats);
    bottomLayout->addWidget(bookBtn);

    cardLayout->addWidget(topWidget);
    cardLayout->addWidget(bottomWidget);
}