#include "main_window.h"
#include "network/tcp_client.h"
#include "flight_card.h"
#include "login_page.h"
#include "seat_selection_dialog.h"
#include "theme_manager.h"
#include <QMessageBox>
#include <QApplication>
#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_isDarkTheme(true),
      m_pendingFlightSeats(180)
{
    setupUI();
    applyTheme();
    setupConnections();
    
    // 启动时请求城市列表
    QTimer::singleShot(500, this, [](){
        TcpClient::getInstance()->getCities();
    });
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setWindowTitle("扶摇航空票务系统");
    setMinimumSize(1200, 800);
    
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // ========== 侧边栏 ==========
    setupSidebar();
    mainLayout->addWidget(m_sidebar);
    
    // ========== 主内容区 ==========
    m_stack = new QStackedWidget(m_centralWidget);
    m_stack->setObjectName("MainStack");
    
    // 航班查询页
    setupFlightPage();
    m_stack->addWidget(m_flightPage);
    
    // 订单页
    m_ordersPage = new OrdersPage();
    m_stack->addWidget(m_ordersPage);
    
    // 个人中心页
    m_profilePage = new ProfilePage();
    m_stack->addWidget(m_profilePage);
    
    mainLayout->addWidget(m_stack, 1);
}

void MainWindow::setupSidebar()
{
    m_sidebar = new QWidget(m_centralWidget);
    m_sidebar->setObjectName("Sidebar");
    m_sidebar->setFixedWidth(260);
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    // Logo区域
    QWidget *logoArea = new QWidget(m_sidebar);
    logoArea->setObjectName("LogoArea");
    logoArea->setFixedHeight(80);
    QHBoxLayout *logoLayout = new QHBoxLayout(logoArea);
    logoLayout->setContentsMargins(25, 0, 25, 0);
    
    QLabel *logoIcon = new QLabel("✈", logoArea);
    logoIcon->setObjectName("SidebarLogo");
    QLabel *logoText = new QLabel("扶摇航空", logoArea);
    logoText->setObjectName("SidebarLogoText");
    
    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(logoText);
    logoLayout->addStretch();
    sidebarLayout->addWidget(logoArea);
    
    // 分隔线
    QFrame *line = new QFrame(m_sidebar);
    line->setObjectName("SidebarLine");
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    sidebarLayout->addWidget(line);
    
    sidebarLayout->addSpacing(15);
    
    // 导航按钮
    auto createNavBtn = [this](const QString& icon, const QString& text) {
        QPushButton *btn = new QPushButton(m_sidebar);
        btn->setObjectName("NavBtn");
        btn->setText(QString("  %1   %2").arg(icon, text));
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMinimumHeight(50);
        return btn;
    };
    
    m_flightBtn = createNavBtn("🔍", "航班查询");
    m_flightBtn->setChecked(true);
    sidebarLayout->addWidget(m_flightBtn);
    
    m_ordersBtn = createNavBtn("📋", "我的订单");
    sidebarLayout->addWidget(m_ordersBtn);
    
    m_profileBtn = createNavBtn("👤", "个人中心");
    sidebarLayout->addWidget(m_profileBtn);
    
    sidebarLayout->addStretch();
    
    // 底部按钮
    m_themeBtn = new QPushButton(m_sidebar);
    m_themeBtn->setObjectName("NavBtn");
    m_themeBtn->setText("  🌗   切换主题");
    m_themeBtn->setCursor(Qt::PointingHandCursor);
    m_themeBtn->setMinimumHeight(50);
    sidebarLayout->addWidget(m_themeBtn);
    
    m_logoutBtn = new QPushButton(m_sidebar);
    m_logoutBtn->setObjectName("LogoutBtn");
    m_logoutBtn->setText("  🚪   退出登录");
    m_logoutBtn->setCursor(Qt::PointingHandCursor);
    m_logoutBtn->setMinimumHeight(50);
    sidebarLayout->addWidget(m_logoutBtn);
    
    sidebarLayout->addSpacing(20);
}

void MainWindow::setupFlightPage()
{
    m_flightPage = new QWidget();
    m_flightPage->setObjectName("FlightPage");
    
    QVBoxLayout *pageLayout = new QVBoxLayout(m_flightPage);
    pageLayout->setContentsMargins(40, 30, 40, 30);
    pageLayout->setSpacing(25);
    
    // ========== 顶部标题区 ==========
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_mainTitleLabel = new QLabel("航班查询");
    m_mainTitleLabel->setObjectName("PageTitle");
    headerLayout->addWidget(m_mainTitleLabel);
    headerLayout->addStretch();
    
    // 结果计数
    m_resultCountLabel = new QLabel("");
    m_resultCountLabel->setObjectName("ResultCount");
    headerLayout->addWidget(m_resultCountLabel);
    
    pageLayout->addLayout(headerLayout);
    
    // ========== 搜索面板 ==========
    QWidget *searchPanel = new QWidget(m_flightPage);
    searchPanel->setObjectName("SearchPanel");
    searchPanel->setGraphicsEffect(createShadow(QColor(0, 0, 0, 20), 20, 5));
    
    QVBoxLayout *searchLayout = new QVBoxLayout(searchPanel);
    searchLayout->setContentsMargins(30, 25, 30, 25);
    searchLayout->setSpacing(20);
    
    // 搜索输入行
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(15);
    
    // 出发地
    QVBoxLayout *depLayout = new QVBoxLayout();
    QLabel *depLabel = new QLabel("出发城市");
    depLabel->setObjectName("FieldLabel");
    m_departureCombo = new QComboBox();
    m_departureCombo->setObjectName("CityCombo");
    m_departureCombo->setEditable(true);
    m_departureCombo->setMinimumHeight(50);
    m_departureCombo->setMinimumWidth(200);
    m_departureCombo->lineEdit()->setPlaceholderText("请选择或输入城市");
    m_departureCombo->setInsertPolicy(QComboBox::NoInsert);
    depLayout->addWidget(depLabel);
    depLayout->addWidget(m_departureCombo);
    inputLayout->addLayout(depLayout);
    
    // 交换按钮
    m_swapBtn = new QPushButton("⇄");
    m_swapBtn->setObjectName("SwapBtn");
    m_swapBtn->setFixedSize(45, 45);
    m_swapBtn->setCursor(Qt::PointingHandCursor);
    m_swapBtn->setToolTip("交换出发地和目的地");
    inputLayout->addWidget(m_swapBtn, 0, Qt::AlignBottom);
    
    // 目的地
    QVBoxLayout *destLayout = new QVBoxLayout();
    QLabel *destLabel = new QLabel("到达城市");
    destLabel->setObjectName("FieldLabel");
    m_destinationCombo = new QComboBox();
    m_destinationCombo->setObjectName("CityCombo");
    m_destinationCombo->setEditable(true);
    m_destinationCombo->setMinimumHeight(50);
    m_destinationCombo->setMinimumWidth(200);
    m_destinationCombo->lineEdit()->setPlaceholderText("请选择或输入城市");
    m_destinationCombo->setInsertPolicy(QComboBox::NoInsert);
    destLayout->addWidget(destLabel);
    destLayout->addWidget(m_destinationCombo);
    inputLayout->addLayout(destLayout);
    
    inputLayout->addSpacing(20);
    
    // 日期
    QVBoxLayout *dateLayout = new QVBoxLayout();
    QLabel *dateLabel = new QLabel("出发日期");
    dateLabel->setObjectName("FieldLabel");
    m_dateEdit = new QDateEdit();
    m_dateEdit->setObjectName("DateEdit");
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDate(QDate::currentDate());
    m_dateEdit->setMinimumDate(QDate::currentDate());
    m_dateEdit->setDisplayFormat("yyyy年MM月dd日");
    m_dateEdit->setMinimumHeight(50);
    m_dateEdit->setMinimumWidth(180);
    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(m_dateEdit);
    inputLayout->addLayout(dateLayout);
    
    inputLayout->addStretch();
    
    // 搜索按钮
    m_searchBtn = new QPushButton("搜索航班");
    m_searchBtn->setObjectName("SearchBtn");
    m_searchBtn->setMinimumSize(140, 50);
    m_searchBtn->setCursor(Qt::PointingHandCursor);
    inputLayout->addWidget(m_searchBtn, 0, Qt::AlignBottom);
    
    searchLayout->addLayout(inputLayout);
    
    // 快捷日期选择
    QHBoxLayout *quickDateLayout = new QHBoxLayout();
    quickDateLayout->setSpacing(10);
    
    QLabel *quickLabel = new QLabel("快捷选择：");
    quickLabel->setObjectName("QuickLabel");
    quickDateLayout->addWidget(quickLabel);
    
    auto createQuickBtn = [this](const QString& text, int daysOffset) {
        QPushButton *btn = new QPushButton(text);
        btn->setObjectName("QuickDateBtn");
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, [this, daysOffset](){
            m_dateEdit->setDate(QDate::currentDate().addDays(daysOffset));
        });
        return btn;
    };
    
    quickDateLayout->addWidget(createQuickBtn("今天", 0));
    quickDateLayout->addWidget(createQuickBtn("明天", 1));
    quickDateLayout->addWidget(createQuickBtn("后天", 2));
    quickDateLayout->addWidget(createQuickBtn("本周末", (6 - QDate::currentDate().dayOfWeek() + 7) % 7 + 1));
    quickDateLayout->addStretch();
    
    searchLayout->addLayout(quickDateLayout);
    
    pageLayout->addWidget(searchPanel);
    
    // ========== 结果列表 ==========
    m_flightScrollArea = new QScrollArea(m_flightPage);
    m_flightScrollArea->setObjectName("FlightScrollArea");
    m_flightScrollArea->setWidgetResizable(true);
    m_flightScrollArea->setFrameShape(QFrame::NoFrame);
    m_flightScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget *scrollContent = new QWidget();
    scrollContent->setObjectName("ScrollContent");
    m_flightLayout = new QVBoxLayout(scrollContent);
    m_flightLayout->setContentsMargins(0, 10, 10, 20);
    m_flightLayout->setSpacing(20);
    
    // 初始提示
    QLabel *hintLabel = new QLabel("请输入出发地和目的地，搜索航班");
    hintLabel->setObjectName("HintLabel");
    hintLabel->setAlignment(Qt::AlignCenter);
    m_flightLayout->addWidget(hintLabel);
    m_flightLayout->addStretch();
    
    m_flightScrollArea->setWidget(scrollContent);
    pageLayout->addWidget(m_flightScrollArea, 1);
}

void MainWindow::setupConnections()
{
    // 导航按钮
    connect(m_flightBtn, &QPushButton::clicked, [this](){ navigateTo(0); });
    connect(m_ordersBtn, &QPushButton::clicked, [this](){ navigateTo(1); });
    connect(m_profileBtn, &QPushButton::clicked, [this](){ navigateTo(2); });
    connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::switchTheme);
    
    connect(m_logoutBtn, &QPushButton::clicked, [this](){
        this->close();
        LoginPage *login = new LoginPage();
        login->show();
    });
    
    // 搜索功能
    connect(m_searchBtn, &QPushButton::clicked, this, &MainWindow::performSearch);
    connect(m_swapBtn, &QPushButton::clicked, [this](){
        QString temp = m_departureCombo->currentText();
        m_departureCombo->setCurrentText(m_destinationCombo->currentText());
        m_destinationCombo->setCurrentText(temp);
    });
    
    // 城市数据
    connect(TcpClient::getInstance(), &TcpClient::citiesResult, this, &MainWindow::onCitiesReceived);
    
    // 航班查询结果
    connect(TcpClient::getInstance(), &TcpClient::flightQueryResults, this, [this](const QList<Flight>& flights){
        // 清空现有
        QLayoutItem *item;
        while ((item = m_flightLayout->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
        
        if (flights.isEmpty()) {
            QLabel *emptyLabel = new QLabel("未找到符合条件的航班");
            emptyLabel->setObjectName("EmptyLabel");
            emptyLabel->setAlignment(Qt::AlignCenter);
            m_flightLayout->addWidget(emptyLabel);
            m_resultCountLabel->setText("");
        } else {
            m_resultCountLabel->setText(QString("找到 %1 个航班").arg(flights.size()));
            
            for (const Flight& f : flights) {
                FlightCard *card = new FlightCard(f);
                connect(card, &FlightCard::bookRequested, this, [this](const Flight& flight){
                    if (!m_changingOrderId.isEmpty()) {
                        // 改签确认
                        QMessageBox msgBox(this);
                        msgBox.setWindowTitle("确认改签");
                        msgBox.setText(QString("确定要改签到航班 %1 吗？\n%2 → %3")
                                       .arg(flight.flight_id, flight.departure, flight.destination));
                        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        msgBox.setButtonText(QMessageBox::Yes, "确定");
                        msgBox.setButtonText(QMessageBox::No, "取消");
                        if (msgBox.exec() == QMessageBox::Yes) {
                            TcpClient::getInstance()->changeTicket(m_changingOrderId, flight.flight_id);
                            m_changingOrderId.clear();
                        }
                    } else {
                        // 选座预订
                        m_pendingFlightId = flight.flight_id;
                        m_pendingFlightSeats = flight.rest_seats;
                        TcpClient::getInstance()->getOccupiedSeats(flight.flight_id);
                    }
                });
                m_flightLayout->addWidget(card);
            }
        }
        m_flightLayout->addStretch();
    });
    
    // 已占座位
    connect(TcpClient::getInstance(), &TcpClient::occupiedSeatsResult, this, &MainWindow::onOccupiedSeatsReceived);
    
    // 预订结果
    connect(TcpClient::getInstance(), &TcpClient::bookTicketResult, this, [this](bool success, const QString& msg){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("预订成功");
            msgBox.setText("🎉 恭喜！预订成功\n订单号：" + msg);
            msgBox.setIcon(QMessageBox::Information);
        } else {
            msgBox.setWindowTitle("预订失败");
            msgBox.setText("预订失败，请重试");
            msgBox.setIcon(QMessageBox::Warning);
        }
        msgBox.exec();
    });
    
    // 订单相关
    connect(m_ordersPage, &OrdersPage::cancelOrder, TcpClient::getInstance(), &TcpClient::cancelTicket);
    connect(TcpClient::getInstance(), &TcpClient::cancelTicketResult, this, [this](bool success){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("取消成功");
            msgBox.setText("订单已取消");
            msgBox.setIcon(QMessageBox::Information);
            TcpClient::getInstance()->queryOrders(m_username);
        } else {
            msgBox.setWindowTitle("取消失败");
            msgBox.setText("取消失败，请重试");
            msgBox.setIcon(QMessageBox::Warning);
        }
        msgBox.exec();
    });
    
    connect(TcpClient::getInstance(), &TcpClient::myOrdersResults, m_ordersPage, &OrdersPage::setOrders);
    
    connect(m_ordersPage, &OrdersPage::changeOrder, this, [this](const QString& orderId){
        m_changingOrderId = orderId;
        navigateTo(0);
        QMessageBox::information(this, "改签", "请搜索并选择要改签的新航班");
    });
    
    connect(TcpClient::getInstance(), &TcpClient::changeTicketResult, this, [this](bool success){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("改签成功");
            msgBox.setText("航班改签成功！");
            msgBox.setIcon(QMessageBox::Information);
            navigateTo(1);
        } else {
            msgBox.setWindowTitle("改签失败");
            msgBox.setText("改签失败，请重试");
            msgBox.setIcon(QMessageBox::Warning);
        }
        msgBox.exec();
    });
    
    // 个人信息
    connect(m_profilePage, &ProfilePage::updateUserInfo, TcpClient::getInstance(), &TcpClient::updateUserInfo);
    connect(TcpClient::getInstance(), &TcpClient::updateUserInfoResult, this, [this](bool success){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("更新成功");
            msgBox.setText("个人信息已更新");
            msgBox.setIcon(QMessageBox::Information);
            TcpClient::getInstance()->getUserInfo(m_username);
        } else {
            msgBox.setWindowTitle("更新失败");
            msgBox.setText("更新失败，请重试");
            msgBox.setIcon(QMessageBox::Warning);
        }
        msgBox.exec();
    });
    
    connect(TcpClient::getInstance(), &TcpClient::userInfoResult, m_profilePage, &ProfilePage::setUserInfo);
}

void MainWindow::performSearch()
{
    QString dep = m_departureCombo->currentText().trimmed();
    QString dest = m_destinationCombo->currentText().trimmed();
    QDate date = m_dateEdit->date();
    
    TcpClient::getInstance()->queryFlights(dep, dest, date);
}

void MainWindow::onCitiesReceived(const QStringList& cities)
{
    m_cities = cities;
    m_departureCombo->clear();
    m_destinationCombo->clear();
    
    m_departureCombo->addItem("");  // 空选项
    m_destinationCombo->addItem("");
    
    for (const QString& city : cities) {
        m_departureCombo->addItem(city);
        m_destinationCombo->addItem(city);
    }
    
    // 设置自动补全
    QCompleter *depCompleter = new QCompleter(cities, m_departureCombo);
    depCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    depCompleter->setFilterMode(Qt::MatchContains);
    m_departureCombo->setCompleter(depCompleter);
    
    QCompleter *destCompleter = new QCompleter(cities, m_destinationCombo);
    destCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    destCompleter->setFilterMode(Qt::MatchContains);
    m_destinationCombo->setCompleter(destCompleter);
}

void MainWindow::onOccupiedSeatsReceived(const QStringList& seats)
{
    if (m_pendingFlightId.isEmpty()) return;
    
    int totalSeats = seats.size() + m_pendingFlightSeats;
    if (totalSeats <= 120) totalSeats = 120;
    else if (totalSeats <= 150) totalSeats = 150;
    else if (totalSeats <= 180) totalSeats = 180;
    else if (totalSeats <= 200) totalSeats = 200;
    else if (totalSeats <= 220) totalSeats = 220;
    else totalSeats = 250;
    
    SeatSelectionDialog dialog(m_pendingFlightId, seats, totalSeats, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString selectedSeat = dialog.selectedSeat();
        TcpClient::getInstance()->bookTicket(m_username, m_pendingFlightId, selectedSeat);
    }
    m_pendingFlightId.clear();
}

void MainWindow::setUsername(const QString& username)
{
    m_username = username;
}

void MainWindow::navigateTo(int index)
{
    m_stack->setCurrentIndex(index);
    
    if (index == 1) {
        TcpClient::getInstance()->queryOrders(m_username);
    } else if (index == 2) {
        TcpClient::getInstance()->getUserInfo(m_username);
    }
}

void MainWindow::switchTheme()
{
    m_isDarkTheme = !m_isDarkTheme;
    applyTheme();
    if (m_profilePage) {
        m_profilePage->updateTheme(m_isDarkTheme);
    }
}

void MainWindow::applyTheme()
{
    QString theme = m_isDarkTheme ? R"(
        /* ========== 全局 ========== */
        * { font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; }
        
        QMainWindow, QWidget#FlightPage, QWidget#ScrollContent {
            background-color: #0f172a;
        }
        
        /* ========== 侧边栏 ========== */
        QWidget#Sidebar {
            background-color: #1e293b;
            border-right: 1px solid #334155;
        }
        
        QWidget#LogoArea {
            background-color: transparent;
        }
        
        QLabel#SidebarLogo {
            font-size: 32px;
            color: #60a5fa;
        }
        
        QLabel#SidebarLogoText {
            font-size: 20px;
            font-weight: bold;
            color: #f1f5f9;
        }
        
        QFrame#SidebarLine {
            background-color: #334155;
        }
        
        QPushButton#NavBtn {
            background-color: transparent;
            border: none;
            border-radius: 8px;
            color: #94a3b8;
            font-size: 15px;
            text-align: left;
            padding: 12px 20px;
            margin: 2px 12px;
        }
        
        QPushButton#NavBtn:hover {
            background-color: #334155;
            color: #f1f5f9;
        }
        
        QPushButton#NavBtn:checked {
            background-color: #3b82f6;
            color: white;
            font-weight: bold;
        }
        
        QPushButton#LogoutBtn {
            background-color: transparent;
            border: none;
            border-radius: 8px;
            color: #f87171;
            font-size: 15px;
            text-align: left;
            padding: 12px 20px;
            margin: 2px 12px;
        }
        
        QPushButton#LogoutBtn:hover {
            background-color: rgba(248, 113, 113, 0.15);
        }
        
        /* ========== 页面标题 ========== */
        QLabel#PageTitle {
            font-size: 28px;
            font-weight: bold;
            color: #f1f5f9;
        }
        
        QLabel#ResultCount {
            font-size: 14px;
            color: #64748b;
        }
        
        /* ========== 搜索面板 ========== */
        QWidget#SearchPanel {
            background-color: #1e293b;
            border-radius: 16px;
            border: 1px solid #334155;
        }
        
        QLabel#FieldLabel {
            font-size: 13px;
            font-weight: 600;
            color: #94a3b8;
            margin-bottom: 6px;
        }
        
        QComboBox#CityCombo {
            background-color: #0f172a;
            border: 2px solid #334155;
            border-radius: 10px;
            padding: 10px 15px;
            font-size: 15px;
            color: #f1f5f9;
        }
        
        QComboBox#CityCombo:focus {
            border-color: #3b82f6;
        }
        
        QComboBox#CityCombo::drop-down {
            border: none;
            width: 30px;
        }
        
        QComboBox#CityCombo::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #64748b;
            margin-right: 10px;
        }
        
        QComboBox#CityCombo QAbstractItemView {
            background-color: #1e293b;
            border: 1px solid #334155;
            border-radius: 8px;
            color: #f1f5f9;
            selection-background-color: #3b82f6;
            outline: none;
        }
        
        QDateEdit#DateEdit {
            background-color: #0f172a;
            border: 2px solid #334155;
            border-radius: 10px;
            padding: 10px 15px;
            font-size: 15px;
            color: #f1f5f9;
        }
        
        QDateEdit#DateEdit:focus {
            border-color: #3b82f6;
        }
        
        QDateEdit#DateEdit::drop-down {
            border: none;
            width: 30px;
        }
        
        QPushButton#SwapBtn {
            background-color: #334155;
            border: none;
            border-radius: 22px;
            color: #94a3b8;
            font-size: 18px;
            font-weight: bold;
        }
        
        QPushButton#SwapBtn:hover {
            background-color: #475569;
            color: #f1f5f9;
        }
        
        QPushButton#SearchBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #3b82f6, stop:1 #8b5cf6);
            border: none;
            border-radius: 10px;
            color: white;
            font-size: 15px;
            font-weight: bold;
        }
        
        QPushButton#SearchBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #2563eb, stop:1 #7c3aed);
        }
        
        QLabel#QuickLabel {
            font-size: 13px;
            color: #64748b;
        }
        
        QPushButton#QuickDateBtn {
            background-color: #334155;
            border: none;
            border-radius: 6px;
            color: #94a3b8;
            font-size: 12px;
            padding: 6px 12px;
        }
        
        QPushButton#QuickDateBtn:hover {
            background-color: #475569;
            color: #f1f5f9;
        }
        
        /* ========== 结果区域 ========== */
        QScrollArea#FlightScrollArea {
            background-color: transparent;
            border: none;
        }
        
        QLabel#HintLabel, QLabel#EmptyLabel {
            font-size: 16px;
            color: #64748b;
            padding: 60px;
        }
        
        /* ========== 滚动条 ========== */
        QScrollBar:vertical {
            background: transparent;
            width: 8px;
            margin: 0;
        }
        
        QScrollBar::handle:vertical {
            background: #475569;
            border-radius: 4px;
            min-height: 30px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: #64748b;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        
        /* ========== 卡片 ========== */
        QWidget#FlightCard, QWidget#OrderCard {
            background-color: #1e293b;
            border: 1px solid #334155;
            border-radius: 16px;
        }
        
        QWidget#FlightCard:hover, QWidget#OrderCard:hover {
            border-color: #3b82f6;
            background-color: #243044;
        }
        
        QLabel#FlightId { font-size: 16px; font-weight: 600; color: #f1f5f9; }
        QLabel#TimeLabel { font-size: 26px; font-weight: 700; color: #60a5fa; }
        QLabel#CityLabel { font-size: 15px; font-weight: 500; color: #f1f5f9; }
        QLabel#AirportLabel { font-size: 12px; color: #64748b; }
        QLabel#PriceLabel { font-size: 22px; font-weight: 700; color: #fb923c; }
        QLabel#DurationLabel { font-size: 12px; color: #64748b; }
        QLabel#SeatsLabel { font-size: 12px; color: #94a3b8; }
        QLabel#ArrowLabel { font-size: 20px; color: #475569; }
        
        QPushButton#BookBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3b82f6, stop:1 #8b5cf6);
            border: none;
            border-radius: 20px;
            color: white;
            font-size: 14px;
            font-weight: bold;
            padding: 10px 25px;
        }
        
        QPushButton#BookBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2563eb, stop:1 #7c3aed);
        }
        
        QPushButton#ChangeBtn {
            background-color: rgba(34, 197, 94, 0.15);
            border: 1px solid #22c55e;
            border-radius: 20px;
            color: #4ade80;
            font-size: 14px;
            font-weight: bold;
            padding: 10px 25px;
        }
        
        QPushButton#ChangeBtn:hover {
            background-color: #22c55e;
            color: white;
        }
        
        QPushButton#CancelBtn {
            background-color: rgba(239, 68, 68, 0.15);
            border: 1px solid #ef4444;
            border-radius: 20px;
            color: #f87171;
            font-size: 14px;
            font-weight: bold;
            padding: 10px 25px;
        }
        
        QPushButton#CancelBtn:hover {
            background-color: #ef4444;
            color: white;
        }
        
        /* ========== 对话框 ========== */
        QMessageBox {
            background-color: #1e293b;
        }
        
        QMessageBox QLabel {
            color: #f1f5f9;
        }
        
        QMessageBox QPushButton {
            background-color: #3b82f6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 20px;
            font-weight: bold;
        }
        
        QMessageBox QPushButton:hover {
            background-color: #2563eb;
        }
    )" : R"(
        /* ========== 全局 ========== */
        * { font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; }
        
        QMainWindow, QWidget#FlightPage, QWidget#ScrollContent {
            background-color: #f8fafc;
        }
        
        /* ========== 侧边栏 ========== */
        QWidget#Sidebar {
            background-color: #ffffff;
            border-right: 1px solid #e2e8f0;
        }
        
        QWidget#LogoArea {
            background-color: transparent;
        }
        
        QLabel#SidebarLogo {
            font-size: 32px;
            color: #3b82f6;
        }
        
        QLabel#SidebarLogoText {
            font-size: 20px;
            font-weight: bold;
            color: #1e293b;
        }
        
        QFrame#SidebarLine {
            background-color: #e2e8f0;
        }
        
        QPushButton#NavBtn {
            background-color: transparent;
            border: none;
            border-radius: 8px;
            color: #64748b;
            font-size: 15px;
            text-align: left;
            padding: 12px 20px;
            margin: 2px 12px;
        }
        
        QPushButton#NavBtn:hover {
            background-color: #f1f5f9;
            color: #1e293b;
        }
        
        QPushButton#NavBtn:checked {
            background-color: #3b82f6;
            color: white;
            font-weight: bold;
        }
        
        QPushButton#LogoutBtn {
            background-color: transparent;
            border: none;
            border-radius: 8px;
            color: #ef4444;
            font-size: 15px;
            text-align: left;
            padding: 12px 20px;
            margin: 2px 12px;
        }
        
        QPushButton#LogoutBtn:hover {
            background-color: rgba(239, 68, 68, 0.1);
        }
        
        /* ========== 页面标题 ========== */
        QLabel#PageTitle {
            font-size: 28px;
            font-weight: bold;
            color: #1e293b;
        }
        
        QLabel#ResultCount {
            font-size: 14px;
            color: #64748b;
        }
        
        /* ========== 搜索面板 ========== */
        QWidget#SearchPanel {
            background-color: #ffffff;
            border-radius: 16px;
            border: 1px solid #e2e8f0;
        }
        
        QLabel#FieldLabel {
            font-size: 13px;
            font-weight: 600;
            color: #64748b;
            margin-bottom: 6px;
        }
        
        QComboBox#CityCombo {
            background-color: #f8fafc;
            border: 2px solid #e2e8f0;
            border-radius: 10px;
            padding: 10px 15px;
            font-size: 15px;
            color: #1e293b;
        }
        
        QComboBox#CityCombo:focus {
            border-color: #3b82f6;
            background-color: white;
        }
        
        QComboBox#CityCombo::drop-down {
            border: none;
            width: 30px;
        }
        
        QComboBox#CityCombo::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #94a3b8;
            margin-right: 10px;
        }
        
        QComboBox#CityCombo QAbstractItemView {
            background-color: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            color: #1e293b;
            selection-background-color: #3b82f6;
            selection-color: white;
            outline: none;
        }
        
        QDateEdit#DateEdit {
            background-color: #f8fafc;
            border: 2px solid #e2e8f0;
            border-radius: 10px;
            padding: 10px 15px;
            font-size: 15px;
            color: #1e293b;
        }
        
        QDateEdit#DateEdit:focus {
            border-color: #3b82f6;
            background-color: white;
        }
        
        QDateEdit#DateEdit::drop-down {
            border: none;
            width: 30px;
        }
        
        QPushButton#SwapBtn {
            background-color: #e2e8f0;
            border: none;
            border-radius: 22px;
            color: #64748b;
            font-size: 18px;
            font-weight: bold;
        }
        
        QPushButton#SwapBtn:hover {
            background-color: #cbd5e1;
            color: #1e293b;
        }
        
        QPushButton#SearchBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #3b82f6, stop:1 #8b5cf6);
            border: none;
            border-radius: 10px;
            color: white;
            font-size: 15px;
            font-weight: bold;
        }
        
        QPushButton#SearchBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #2563eb, stop:1 #7c3aed);
        }
        
        QLabel#QuickLabel {
            font-size: 13px;
            color: #64748b;
        }
        
        QPushButton#QuickDateBtn {
            background-color: #f1f5f9;
            border: none;
            border-radius: 6px;
            color: #64748b;
            font-size: 12px;
            padding: 6px 12px;
        }
        
        QPushButton#QuickDateBtn:hover {
            background-color: #e2e8f0;
            color: #1e293b;
        }
        
        /* ========== 结果区域 ========== */
        QScrollArea#FlightScrollArea {
            background-color: transparent;
            border: none;
        }
        
        QLabel#HintLabel, QLabel#EmptyLabel {
            font-size: 16px;
            color: #94a3b8;
            padding: 60px;
        }
        
        /* ========== 滚动条 ========== */
        QScrollBar:vertical {
            background: transparent;
            width: 8px;
            margin: 0;
        }
        
        QScrollBar::handle:vertical {
            background: #cbd5e1;
            border-radius: 4px;
            min-height: 30px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: #94a3b8;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        
        /* ========== 卡片 ========== */
        QWidget#FlightCard, QWidget#OrderCard {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 16px;
        }
        
        QWidget#FlightCard:hover, QWidget#OrderCard:hover {
            border-color: #3b82f6;
            background-color: #f8fafc;
        }
        
        QLabel#FlightId { font-size: 16px; font-weight: 600; color: #1e293b; }
        QLabel#TimeLabel { font-size: 26px; font-weight: 700; color: #3b82f6; }
        QLabel#CityLabel { font-size: 15px; font-weight: 500; color: #1e293b; }
        QLabel#AirportLabel { font-size: 12px; color: #64748b; }
        QLabel#PriceLabel { font-size: 22px; font-weight: 700; color: #f97316; }
        QLabel#DurationLabel { font-size: 12px; color: #64748b; }
        QLabel#SeatsLabel { font-size: 12px; color: #64748b; }
        QLabel#ArrowLabel { font-size: 20px; color: #cbd5e1; }
        
        QPushButton#BookBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3b82f6, stop:1 #8b5cf6);
            border: none;
            border-radius: 20px;
            color: white;
            font-size: 14px;
            font-weight: bold;
            padding: 10px 25px;
        }
        
        QPushButton#BookBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2563eb, stop:1 #7c3aed);
        }
        
        QPushButton#ChangeBtn {
            background-color: rgba(34, 197, 94, 0.1);
            border: 1px solid #22c55e;
            border-radius: 20px;
            color: #16a34a;
            font-size: 14px;
            font-weight: bold;
            padding: 10px 25px;
        }
        
        QPushButton#ChangeBtn:hover {
            background-color: #22c55e;
            color: white;
        }
        
        QPushButton#CancelBtn {
            background-color: rgba(239, 68, 68, 0.1);
            border: 1px solid #ef4444;
            border-radius: 20px;
            color: #dc2626;
            font-size: 14px;
            font-weight: bold;
            padding: 10px 25px;
        }
        
        QPushButton#CancelBtn:hover {
            background-color: #ef4444;
            color: white;
        }
        
        /* ========== 对话框 ========== */
        QMessageBox {
            background-color: white;
        }
        
        QMessageBox QLabel {
            color: #1e293b;
        }
        
        QMessageBox QPushButton {
            background-color: #3b82f6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 20px;
            font-weight: bold;
        }
        
        QMessageBox QPushButton:hover {
            background-color: #2563eb;
        }
    )";
    
    qApp->setStyleSheet(theme);
}

QGraphicsDropShadowEffect* MainWindow::createShadow(QColor color, int blur, int offsetY)
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(blur);
    shadow->setColor(color);
    shadow->setOffset(0, offsetY);
    return shadow;
}
