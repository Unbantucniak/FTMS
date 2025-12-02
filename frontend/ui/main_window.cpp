#include "main_window.h"
#include "ui_main_window.h"
#include "network/tcp_client.h"
#include "flight_card.h"
#include "login_page.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QScrollArea>
#include <QFrame>

// 主题样式表
const QString DARK_THEME = R"(
    QWidget { background-color: #2b2b2b; color: #e0e0e0; font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; font-size: 14px; }
    
    QLabel#MainTitle {
        font-size: 26px;
        font-weight: 600;
        letter-spacing: 3px;
        color: #f5f5f5;
    }

    QWidget#FilterPanel {
        background-color: #353535;
        border: 1px solid #444;
        border-radius: 16px;
    }

    /* 输入框 */
    QLineEdit, QDateEdit, QComboBox { 
        background-color: #3a3a3a; 
        border: 1px solid #555; 
        border-radius: 6px; 
        padding: 8px 12px; 
        color: #fff; 
        selection-background-color: #0078d7;
    }
    QLineEdit:focus, QDateEdit:focus, QComboBox:focus { border: 1px solid #0078d7; }
    
    /* 通用按钮 */
    QPushButton { 
        background-color: #3a3a3a; 
        color: #e0e0e0; 
        border: 1px solid #555; 
        border-radius: 6px; 
        padding: 8px 16px; 
        font-weight: bold;
    }
    QPushButton:hover { background-color: #444; }
    QPushButton:pressed { background-color: #222; }
    QPushButton:disabled { background-color: #444; color: #888; border: none; }

    /* 弹窗样式 */
    QMessageBox { background-color: #2b2b2b; border: 1px solid #444; }
    QMessageBox QLabel { color: #e0e0e0; }
    QMessageBox QPushButton {
        background-color: #0078d7;
        color: white;
        border: none;
    }
    QMessageBox QPushButton:hover { background-color: #1084e3; }

    /* 表格 */
    QTableWidget { 
        background-color: #3a3a3a; 
        border: 1px solid #555; 
        gridline-color: #444; 
        selection-background-color: #0078d7; 
        selection-color: white;
        alternate-background-color: #333;
    }
    QHeaderView::section { 
        background-color: #2d2d2d; 
        color: #e0e0e0; 
        padding: 8px; 
        border: none; 
        border-bottom: 2px solid #0078d7; 
        font-weight: bold;
    }
    QTableCornerButton::section { background-color: #2d2d2d; border: none; }

    /* 滚动条 */
    QScrollBar:vertical {
        border: none;
        background: #2b2b2b;
        width: 10px;
        margin: 0px;
    }
    QScrollBar::handle:vertical {
        background: #555;
        min-height: 20px;
        border-radius: 5px;
    }
    QScrollBar::handle:vertical:hover { background: #666; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }

    /* 侧边栏特定样式 */
    QWidget#Sidebar { background-color: #1e1e1e; border-right: 1px solid #333; }
    QPushButton#SidebarBtn { 
        background-color: transparent; 
        text-align: left; 
        padding: 15px 20px; 
        border-radius: 0; 
        border: none;
        font-size: 15px; 
        color: #aaa;
        border-left: 4px solid transparent;
    }
    QPushButton#SidebarBtn:hover { background-color: #252525; color: #fff; }
    QPushButton#SidebarBtn:checked { 
        background-color: #2d2d2d; 
        color: #0078d7; 
        border-left: 4px solid #0078d7;
        font-weight: bold;
    }

    /* Cards */
    QWidget#FlightCard, QWidget#OrderCard {
        background-color: transparent;
        border: none;
        padding: 4px 2px;
        margin-bottom: 26px;
    }
    QWidget#FlightCard QWidget#CardBody,
    QWidget#OrderCard QWidget#CardBody {
        background-color: #2f323a;
        border: 1px solid #3f444f;
        border-top-left-radius: 18px;
        border-top-right-radius: 18px;
        border-bottom: none;
        padding-top: 6px;
    }
    QWidget#FlightCard QWidget#InfoContainer,
    QWidget#OrderCard QWidget#InfoContainer {
        background-color: #23252b;
        border: 1px solid #3f444f;
        border-bottom-left-radius: 18px;
        border-bottom-right-radius: 18px;
        border-top: 1px solid #4a5060;
    }
    QWidget#FlightCard:hover QWidget#CardBody,
    QWidget#OrderCard:hover QWidget#CardBody {
        border-color: #6a8dff;
        background-color: #353946;
    }
    QWidget#FlightCard:hover QWidget#InfoContainer,
    QWidget#OrderCard:hover QWidget#InfoContainer {
        border-color: #6a8dff;
        border-top-color: #7a9cfe;
    }
    FlightCard QLabel, OrderCard QLabel { color: #f3f5f9; }
    QLabel#FlightId { font-size: 17px; font-weight: 600; color: #f8fbff; }
    QLabel#TimeLabel { font-size: 28px; font-weight: 600; color: #6aa8ff; }
    QLabel#CityLabel { font-size: 16px; font-weight: 500; color: #f3f5f9; }
    QLabel#AirportLabel { font-size: 13px; color: #a6adc8; }
    QLabel#PriceLabel { font-size: 24px; font-weight: 600; color: #ffb878; }
    QLabel#DurationLabel { font-size: 13px; color: #95a0be; }
    QLabel#SeatsLabel { font-size: 13px; color: #cdd3ec; }
    QLabel#ArrowLabel { color: #717da3; font-weight: bold; }
    
    /* 操作按钮样式 (统一结构，不同配色) */
    QPushButton#BookBtn,
    QPushButton#ChangeBtn,
    QPushButton#CancelBtn {
        border-radius: 18px;
        padding: 8px 28px;
        font-weight: 600;
        min-width: 120px;
        border-width: 1px;
    }
    QPushButton#BookBtn {
        background-color: rgba(90, 143, 255, 0.15);
        color: #a5c4ff;
        border-color: #7fa7ff;
    }
    QPushButton#BookBtn:hover {
        background-color: #4b7bec;
        color: #0e1833;
        border-color: #4b7bec;
    }
    QPushButton#ChangeBtn {
        background-color: rgba(61, 214, 190, 0.15);
        color: #7de5d4;
        border-color: #46cdb8;
    }
    QPushButton#ChangeBtn:hover {
        background-color: #2dd4bf;
        color: #082027;
        border-color: #2dd4bf;
    }
    QPushButton#CancelBtn {
        background-color: rgba(255, 107, 107, 0.15);
        color: #ff9c9c;
        border-color: #ff9c9c;
    }
    QPushButton#CancelBtn:hover { 
        background-color: #ff6b6b; 
        color: #1b0c0c;
        border-color: #ff6b6b;
    }

    /* 查询按钮 */
    QPushButton#queryButton {
        background-color: #0078d7;
        color: white;
        border: none;
        min-width: 100px;
        font-size: 15px;
        padding: 10px 22px;
        border-radius: 8px;
    }
    QPushButton#queryButton:hover { background-color: #1084e3; }

    QWidget#InfoContainer {
        background-color: #23252b;
        border-bottom-left-radius: 18px;
        border-bottom-right-radius: 18px;
        border-top: 1px solid #4a5060;
    }
)";

const QString LIGHT_THEME = R"(
    QWidget { background-color: #f5f7fa; color: #333; font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; font-size: 14px; }
    
    QLabel#MainTitle {
        font-size: 26px;
        font-weight: 600;
        letter-spacing: 2px;
        color: #1f2c3d;
    }

    QWidget#FilterPanel {
        background-color: #ffffff;
        border: 1px solid #e3e8f0;
        border-radius: 16px;
    }
    
    /* 输入框 */
    QLineEdit, QDateEdit, QComboBox { 
        background-color: #fff; 
        border: 1px solid #dcdfe6; 
        border-radius: 6px; 
        padding: 8px 12px; 
        color: #333; 
        selection-background-color: #0078d7;
    }
    QLineEdit:focus, QDateEdit:focus, QComboBox:focus { border: 1px solid #0078d7; }

    /* 通用按钮 */
    QPushButton { 
        background-color: #fff; 
        color: #606266; 
        border: 1px solid #dcdfe6; 
        border-radius: 6px; 
        padding: 8px 16px; 
        font-weight: bold;
    }
    QPushButton:hover { background-color: #ecf5ff; color: #409eff; border-color: #c6e2ff; }
    QPushButton:pressed { background-color: #ecf5ff; color: #3a8ee6; border-color: #3a8ee6; }
    QPushButton:disabled { background-color: #e0e0e0; color: #aaa; border-color: #ddd; }

    /* 弹窗样式 */
    QMessageBox { background-color: #fff; }
    QMessageBox QLabel { color: #333; }
    QMessageBox QPushButton {
        background-color: #0078d7;
        color: white;
        border: none;
    }
    QMessageBox QPushButton:hover { background-color: #1084e3; }

    /* 表格 */
    QTableWidget { 
        background-color: #fff; 
        border: 1px solid #e0e0e0; 
        gridline-color: #f0f0f0; 
        selection-background-color: #e6f2ff; 
        selection-color: #0078d7;
        alternate-background-color: #fafafa;
    }
    QHeaderView::section { 
        background-color: #fff; 
        color: #333; 
        padding: 8px; 
        border: none; 
        border-bottom: 2px solid #0078d7; 
        font-weight: bold;
    }
    QTableCornerButton::section { background-color: #fff; border: none; }

    /* 滚动条 */
    QScrollBar:vertical {
        border: none;
        background: #f5f7fa;
        width: 10px;
        margin: 0px;
    }
    QScrollBar::handle:vertical {
        background: #ccc;
        min-height: 20px;
        border-radius: 5px;
    }
    QScrollBar::handle:vertical:hover { background: #bbb; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }

    /* 侧边栏特定样式 */
    QWidget#Sidebar { background-color: #ffffff; border-right: 1px solid #e0e0e0; }
    QPushButton#SidebarBtn { 
        background-color: transparent; 
        text-align: left; 
        padding: 15px 20px; 
        border-radius: 0; 
        border: none;
        color: #666; 
        font-size: 15px; 
        border-left: 4px solid transparent;
    }
    QPushButton#SidebarBtn:hover { background-color: #f5f7fa; color: #333; }
    QPushButton#SidebarBtn:checked { 
        background-color: #e6f2ff; 
        color: #0078d7; 
        font-weight: bold; 
        border-left: 4px solid #0078d7; 
    }

    /* Cards */
    QWidget#FlightCard, QWidget#OrderCard {
        background-color: transparent;
        border: none;
        padding: 4px 2px;
        margin-bottom: 26px;
    }
    QWidget#FlightCard QWidget#CardBody,
    QWidget#OrderCard QWidget#CardBody {
        background-color: #ffffff;
        border: 1px solid #dfe5ef;
        border-top-left-radius: 18px;
        border-top-right-radius: 18px;
        border-bottom: none;
        padding-top: 6px;
    }
    QWidget#FlightCard QWidget#InfoContainer,
    QWidget#OrderCard QWidget#InfoContainer {
        background-color: #f5f7fb;
        border: 1px solid #dfe5ef;
        border-top: 1px solid #edf1f7;
        border-bottom-left-radius: 18px;
        border-bottom-right-radius: 18px;
    }
    QWidget#FlightCard:hover QWidget#CardBody,
    QWidget#OrderCard:hover QWidget#CardBody {
        border-color: #7ab5ff;
        background-color: #f5f9ff;
    }
    QWidget#FlightCard:hover QWidget#InfoContainer,
    QWidget#OrderCard:hover QWidget#InfoContainer {
        border-color: #7ab5ff;
        border-top-color: #b3d6ff;
    }
    FlightCard QLabel, OrderCard QLabel { color: #2f3343; }
    QLabel#FlightId { font-size: 17px; font-weight: 600; color: #1f2c3d; }
    QLabel#TimeLabel { font-size: 28px; font-weight: 600; color: #2b79ff; }
    QLabel#CityLabel { font-size: 16px; font-weight: 500; color: #2f3343; }
    QLabel#AirportLabel { font-size: 13px; color: #7a8095; }
    QLabel#PriceLabel { font-size: 24px; font-weight: 600; color: #ff7b45; }
    QLabel#DurationLabel { font-size: 13px; color: #98a1b2; }
    QLabel#SeatsLabel { font-size: 13px; color: #4b556b; }
    QLabel#ArrowLabel { color: #a9b2c4; font-weight: bold; }

    /* 操作按钮样式 (统一结构，不同配色) */
    QPushButton#BookBtn,
    QPushButton#ChangeBtn,
    QPushButton#CancelBtn {
        border-radius: 18px;
        padding: 8px 28px;
        font-weight: 600;
        min-width: 120px;
        border-width: 1px;
    }
    QPushButton#BookBtn {
        background-color: rgba(71, 137, 255, 0.12);
        color: #2b63c7;
        border-color: #4b7bec;
    }
    QPushButton#BookBtn:hover {
        background-color: #4b7bec;
        color: #ffffff;
        border-color: #4b7bec;
    }
    QPushButton#ChangeBtn {
        background-color: rgba(45, 212, 191, 0.12);
        color: #0f766e;
        border-color: #10b981;
    }
    QPushButton#ChangeBtn:hover {
        background-color: #10b981;
        color: #ffffff;
        border-color: #10b981;
    }
    QPushButton#CancelBtn {
        background-color: rgba(239, 68, 68, 0.12);
        color: #d14343;
        border-color: #f19999;
    }
    QPushButton#CancelBtn:hover { 
        background-color: #ef4444; 
        color: #fff;
        border-color: #ef4444;
    }

    /* 查询按钮 */
    QPushButton#queryButton {
        background-color: #0078d7;
        color: white;
        border: none;
        min-width: 100px;
        font-size: 15px;
        padding: 10px 22px;
        border-radius: 8px;
    }
    QPushButton#queryButton:hover { background-color: #1084e3; }

    QWidget#InfoContainer {
        background-color: #f5f7fb;
        border-bottom-left-radius: 18px;
        border-bottom-right-radius: 18px;
        border-top: 1px solid #edf1f7;
    }
)";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_isDarkTheme(true)
{
    ui->setupUi(this);
    setWindowTitle("扶摇航空票务系统");
    // setWindowIcon(QIcon(":/icons/app_icon.png")); // 添加图标后取消注释

    // 强化标题样式
    ui->mainTitleLabel->setObjectName("MainTitle");
    ui->mainTitleLabel->setText("扶摇 · 航班查询");

    // 优化时间选择器
    ui->dateEdit->setCalendarPopup(true);
    ui->dateEdit->setDisplayFormat("yyyy-MM-dd");
    // 设置只读以防止误触滚动，强制使用日历弹窗
    if(QLineEdit *le = ui->dateEdit->findChild<QLineEdit*>()) {
        le->setReadOnly(true);
    }

    // 将原来的筛选控件包裹在 FilterPanel 中，获得统一底色和圆角
    QWidget *filterPanel = new QWidget(this);
    filterPanel->setObjectName("FilterPanel");
    filterPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *filterLayout = new QHBoxLayout(filterPanel);
    filterLayout->setContentsMargins(20, 15, 20, 15);
    filterLayout->setSpacing(12);

    auto reparentFilterWidget = [&](QWidget *w) {
        ui->horizontalLayout->removeWidget(w);
        w->setParent(filterPanel);
        filterLayout->addWidget(w);
    };
    reparentFilterWidget(ui->departureEdit);
    reparentFilterWidget(ui->destinationEdit);
    reparentFilterWidget(ui->dateEdit);
    reparentFilterWidget(ui->queryButton);

    int filterIndex = ui->verticalLayout->indexOf(ui->horizontalLayout);
    ui->verticalLayout->insertWidget(filterIndex, filterPanel);
    delete ui->horizontalLayout;
    ui->horizontalLayout = nullptr;

    // 隐藏旧的表格和按钮
    ui->flightTable->setVisible(false);
    ui->bookButton->setVisible(false);

    // 初始化滚动区域
    m_flightScrollArea = new QScrollArea();
    m_flightScrollArea->setWidgetResizable(true);
    m_flightScrollArea->setFrameShape(QFrame::NoFrame);
    m_flightScrollArea->setStyleSheet("QScrollArea { background: transparent; }");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    m_flightLayout = new QVBoxLayout(scrollContent);
    m_flightLayout->setContentsMargins(30, 10, 30, 20);
    m_flightLayout->setSpacing(25);
    m_flightLayout->addStretch();

    m_flightScrollArea->setWidget(scrollContent);
    
    // 添加到布局 (替换表格位置)
    ui->verticalLayout->insertWidget(2, m_flightScrollArea);
    
    // 设置主布局
    QWidget *central = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 侧边栏
    QWidget *sidebar = new QWidget(central);
    sidebar->setObjectName("Sidebar");
    sidebar->setFixedWidth(250);
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 20, 0, 20);
    sidebarLayout->setSpacing(5);
    
    auto createBtn = [&](const QString& text, int index) {
        QPushButton *btn = new QPushButton(text, sidebar);
        btn->setObjectName("SidebarBtn");
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        connect(btn, &QPushButton::clicked, this, [this, index](){ navigateTo(index); });
        sidebarLayout->addWidget(btn);
        if(index == 0) btn->setChecked(true);
        return btn;
    };
    
    createBtn("✈  航班查询", 0);
    createBtn("📋  我的订单", 1);
    createBtn("👤  个人中心", 2);
    
    sidebarLayout->addStretch();
    
    QPushButton *themeBtn = new QPushButton("🌗  切换主题", sidebar);
    themeBtn->setObjectName("SidebarBtn");
    connect(themeBtn, &QPushButton::clicked, this, &MainWindow::switchTheme);
    sidebarLayout->addWidget(themeBtn);

    QPushButton *logoutBtn = new QPushButton("🚪  退出登录", sidebar);
    logoutBtn->setObjectName("SidebarBtn");
    connect(logoutBtn, &QPushButton::clicked, this, [this](){
        this->close();
        LoginPage *login = new LoginPage();
        login->show();
    });
    sidebarLayout->addWidget(logoutBtn);
    
    mainLayout->addWidget(sidebar);
    
    // 堆叠窗口
    m_stack = new QStackedWidget(central);
    
    // 页面 0: 航班查询 (重新父级化现有UI)
    QWidget *flightPage = ui->centralwidget;
    // 我们需要先将其从MainWindow中移除，但setupUi已经设置了它。
    // 我们可以直接将其添加到堆栈中，QStackedWidget会重新父级化它。
    // 但是等等，ui->centralwidget已经被设置为'this'的中心部件。
    // 我们正在设置'central'为新的中心部件。
    // 所以我们可以直接获取ui->centralwidget。
    m_stack->addWidget(flightPage);
    
    // 页面 1: 订单
    m_ordersPage = new OrdersPage();
    m_stack->addWidget(m_ordersPage);
    
    // 页面 2: 个人中心
    m_profilePage = new ProfilePage();
    m_stack->addWidget(m_profilePage);
    
    mainLayout->addWidget(m_stack);
    
    setCentralWidget(central);
    
    applyTheme();
    
    // 连接信号槽
    connect(ui->queryButton, &QPushButton::clicked, this, [this](){
        QString dep = ui->departureEdit->text();
        QString dest = ui->destinationEdit->text();
        QDate date = ui->dateEdit->date();
        TcpClient::getInstance()->queryFlights(dep, dest, date);
    });
    
    connect(TcpClient::getInstance(), &TcpClient::flightQueryResults, this, [this](const QList<Flight>& flights){
        // 清空现有卡片
        QLayoutItem *item;
        while ((item = m_flightLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
        
        // 添加新卡片
        for(const Flight& f : flights) {
            FlightCard *card = new FlightCard(f);
            connect(card, &FlightCard::bookRequested, this, [this](const Flight& flight){
                QString flightId = flight.flight_id;
                if (m_username.isEmpty()) m_username = "test"; 
                
                QString info = QString("航班号: %1\n出发: %2 %3\n到达: %4 %5\n时间: %6 - %7\n价格: ¥%8")
                               .arg(flight.flight_id)
                               .arg(flight.departure).arg(flight.departure_airport)
                               .arg(flight.destination).arg(flight.arrival_airport)
                               .arg(flight.depart_time.toString("yyyy-MM-dd HH:mm"))
                               .arg(flight.arrive_time.toString("yyyy-MM-dd HH:mm"))
                               .arg(flight.price);

                if (!m_changingOrderId.isEmpty()) {
                    QMessageBox msgBox(this);
                    msgBox.setWindowTitle("确认改签");
                    msgBox.setText("确定要改签到以下航班吗?\n\n" + info);
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setButtonText(QMessageBox::Yes, "是");
                    msgBox.setButtonText(QMessageBox::No, "否");
                    if (msgBox.exec() == QMessageBox::Yes) {
                        TcpClient::getInstance()->changeTicket(m_changingOrderId, flightId);
                        m_changingOrderId.clear(); 
                    }
                } else {
                    QMessageBox msgBox(this);
                    msgBox.setWindowTitle("确认预订");
                    msgBox.setText("确定要预订以下航班吗?\n\n" + info);
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setButtonText(QMessageBox::Yes, "是");
                    msgBox.setButtonText(QMessageBox::No, "否");
                    if (msgBox.exec() == QMessageBox::Yes) {
                        TcpClient::getInstance()->bookTicket(m_username, flightId);
                    }
                }
            });
            m_flightLayout->addWidget(card);
        }
        m_flightLayout->addStretch(); // 底部弹簧
    });
    

    
    connect(TcpClient::getInstance(), &TcpClient::bookTicketResult, this, [this](bool success, const QString& msg){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("预订");
            msgBox.setText("预订成功！订单号：" + msg);
            msgBox.setIcon(QMessageBox::Information);
        } else {
            msgBox.setWindowTitle("预订");
            msgBox.setText("预订失败。");
            msgBox.setIcon(QMessageBox::Warning);
        }
        msgBox.exec();
    });
    
    connect(m_ordersPage, &OrdersPage::cancelOrder, TcpClient::getInstance(), &TcpClient::cancelTicket);

    connect(TcpClient::getInstance(), &TcpClient::cancelTicketResult, this, [this](bool success){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("取消订单");
            msgBox.setText("订单取消成功！");
            msgBox.setIcon(QMessageBox::Information);
            TcpClient::getInstance()->queryOrders(m_username); // 刷新列表
        } else {
            msgBox.setWindowTitle("取消订单");
            msgBox.setText("订单取消失败。");
            msgBox.setIcon(QMessageBox::Warning);
        }
        msgBox.exec();
    });

    connect(TcpClient::getInstance(), &TcpClient::myOrdersResults, this, [this](const QList<Order>& orders){
        m_ordersPage->setOrders(orders);
    });
    
    connect(m_profilePage, &ProfilePage::updateUserInfo, TcpClient::getInstance(), &TcpClient::updateUserInfo);

    connect(TcpClient::getInstance(), &TcpClient::updateUserInfoResult, this, [this](bool success){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("个人信息");
            msgBox.setText("更新成功！");
            msgBox.setIcon(QMessageBox::Information);
            TcpClient::getInstance()->getUserInfo(m_username); // 刷新
        } else {
            msgBox.setWindowTitle("个人信息");
            msgBox.setText("更新失败。");
            msgBox.setIcon(QMessageBox::Warning);
        }
        msgBox.exec();
    });
    
    connect(TcpClient::getInstance(), &TcpClient::userInfoResult, this, [this](const User& user){
        m_profilePage->setUserInfo(user);
    });
    
    connect(m_ordersPage, &OrdersPage::changeOrder, this, [this](const QString& orderId){
        m_changingOrderId = orderId;
        navigateTo(0); // Go to flight query
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("改签");
        msgBox.setText("请查询并选择您想要改签的新航班。");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    });

    connect(TcpClient::getInstance(), &TcpClient::changeTicketResult, this, [this](bool success){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("改签");
            msgBox.setText("改签成功！");
            msgBox.setIcon(QMessageBox::Information);
            navigateTo(1); // Go back to orders
        } else {
            msgBox.setWindowTitle("改签");
            msgBox.setText("改签失败。");
            msgBox.setIcon(QMessageBox::Warning);
        }
        msgBox.exec();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUsername(const QString& username) {
    m_username = username;
}

void MainWindow::navigateTo(int index) {
    m_stack->setCurrentIndex(index);
    if (index == 1) { // Orders
        TcpClient::getInstance()->queryOrders(m_username);
    } else if (index == 2) { // Profile
        TcpClient::getInstance()->getUserInfo(m_username);
    }
}

void MainWindow::switchTheme() {
    m_isDarkTheme = !m_isDarkTheme;
    applyTheme();
}

void MainWindow::applyTheme() {
    qApp->setStyleSheet(m_isDarkTheme ? DARK_THEME : LIGHT_THEME);
}
