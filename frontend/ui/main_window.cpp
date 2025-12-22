#include "main_window.h"
#include "network/tcp_client.h"
#include "flight_card.h"
#include "login_page.h"
#include "seat_selection_dialog.h"
#include "chat_dialog.h"
#include "theme_manager.h"
#include <QMessageBox>
#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>
#include <QCalendarWidget>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_isDarkTheme(true),
      m_pendingFlightSeats(180)
{
    setupUI();
    applyTheme();
    setupConnections();
    
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
    
    // 左侧是固定导航，右侧是业务页面
    setupSidebar();
    mainLayout->addWidget(m_sidebar);
    
    m_stack = new QStackedWidget(m_centralWidget);
    m_stack->setObjectName("MainStack");
    
    setupFlightPage();
    m_stack->addWidget(m_flightPage);
    
    m_ordersPage = new OrdersPage();
    m_stack->addWidget(m_ordersPage);
    
    m_profilePage = new ProfilePage();
    m_stack->addWidget(m_profilePage);
    
    m_chatWidget = new ChatWidget();
    m_stack->addWidget(m_chatWidget);

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

    m_chatBtn = createNavBtn("🤖", "出行助手");
    sidebarLayout->addWidget(m_chatBtn);
    
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
    pageLayout->setContentsMargins(48, 32, 48, 32);
    pageLayout->setSpacing(28);
    
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_mainTitleLabel = new QLabel("航班查询");
    m_mainTitleLabel->setObjectName("PageTitle");
    headerLayout->addWidget(m_mainTitleLabel);
    headerLayout->addStretch();
    
    m_resultCountLabel = new QLabel("");
    m_resultCountLabel->setObjectName("ResultCount");
    headerLayout->addWidget(m_resultCountLabel);
    
    pageLayout->addLayout(headerLayout);
    
    QWidget *searchPanel = new QWidget(m_flightPage);
    searchPanel->setObjectName("SearchPanel");
    searchPanel->setGraphicsEffect(createShadow(QColor(0, 0, 0, 25), 24, 6));
    
    QVBoxLayout *searchLayout = new QVBoxLayout(searchPanel);
    searchLayout->setContentsMargins(32, 28, 32, 28);
    searchLayout->setSpacing(22);
    
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(15);
    
    QVBoxLayout *depLayout = new QVBoxLayout();
    depLayout->setSpacing(8);
    QLabel *depLabel = new QLabel("✈ 出发城市");
    depLabel->setObjectName("FieldLabel");
    m_departureCombo = new QComboBox();
    m_departureCombo->setObjectName("CityCombo");
    m_departureCombo->setEditable(true);
    m_departureCombo->setMinimumHeight(52);
    m_departureCombo->setMinimumWidth(220);
    m_departureCombo->lineEdit()->setPlaceholderText("请选择或输入城市");
    m_departureCombo->setInsertPolicy(QComboBox::NoInsert);
    depLayout->addWidget(depLabel);
    depLayout->addWidget(m_departureCombo);
    inputLayout->addLayout(depLayout);
    
    m_swapBtn = new QPushButton("⇄");
    m_swapBtn->setObjectName("SwapBtn");
    m_swapBtn->setFixedSize(45, 45);
    m_swapBtn->setCursor(Qt::PointingHandCursor);
    m_swapBtn->setToolTip("交换出发地和目的地");
    inputLayout->addWidget(m_swapBtn, 0, Qt::AlignBottom);
    
    QVBoxLayout *destLayout = new QVBoxLayout();
    destLayout->setSpacing(8);
    QLabel *destLabel = new QLabel("📍 到达城市");
    destLabel->setObjectName("FieldLabel");
    m_destinationCombo = new QComboBox();
    m_destinationCombo->setObjectName("CityCombo");
    m_destinationCombo->setEditable(true);
    m_destinationCombo->setMinimumHeight(52);
    m_destinationCombo->setMinimumWidth(220);
    m_destinationCombo->lineEdit()->setPlaceholderText("请选择或输入城市");
    m_destinationCombo->setInsertPolicy(QComboBox::NoInsert);
    destLayout->addWidget(destLabel);
    destLayout->addWidget(m_destinationCombo);
    inputLayout->addLayout(destLayout);
    
    inputLayout->addSpacing(20);
    
    QVBoxLayout *dateLayout = new QVBoxLayout();
    dateLayout->setSpacing(8);
    QLabel *dateLabel = new QLabel("📅 出发日期");
    dateLabel->setObjectName("FieldLabel");
    
    m_dateLimitCheckBox = new QCheckBox("开启日期查询（选定日期前后三天）");
    m_dateLimitCheckBox->setChecked(true);
    m_dateLimitCheckBox->setCursor(Qt::PointingHandCursor);
    m_dateLimitCheckBox->setObjectName("DateLimitCheckBox");
    
    m_dateEdit = new QDateEdit();
    m_dateEdit->setObjectName("DateEdit");
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDate(QDate::currentDate());
    m_dateEdit->setMinimumDate(QDate::currentDate());
    m_dateEdit->setDisplayFormat("yyyy年MM月dd日 ddd");
    m_dateEdit->setMinimumHeight(52);
    m_dateEdit->setMinimumWidth(200);
    m_dateEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_dateEdit->setKeyboardTracking(false);
    m_dateEdit->setCursor(Qt::PointingHandCursor);
    m_dateEdit->setFocusPolicy(Qt::ClickFocus);
    
    if (auto *le = m_dateEdit->findChild<QLineEdit*>()) {
        le->setReadOnly(true);
        le->setCursor(Qt::PointingHandCursor);
    }
    
    QCalendarWidget *calendar = m_dateEdit->calendarWidget();
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    calendar->setGridVisible(false);
    calendar->setMinimumSize(350, 300);
    
    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(m_dateLimitCheckBox);
    dateLayout->addWidget(m_dateEdit);
    inputLayout->addLayout(dateLayout);
    
    inputLayout->addStretch();
    
    m_searchBtn = new QPushButton("🔍 搜索航班");
    m_searchBtn->setObjectName("SearchBtn");
    m_searchBtn->setMinimumSize(150, 52);
    m_searchBtn->setCursor(Qt::PointingHandCursor);
    inputLayout->addWidget(m_searchBtn, 0, Qt::AlignBottom);
    
    searchLayout->addLayout(inputLayout);
    
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
            QDate newDate = QDate::currentDate().addDays(daysOffset);
            m_dateEdit->setDate(newDate);
            m_dateLimitCheckBox->setChecked(true);  // 自动勾选日期限制
        });
        return btn;
    };
    
    quickDateLayout->addWidget(createQuickBtn("今天", 0));
    quickDateLayout->addWidget(createQuickBtn("明天", 1));
    quickDateLayout->addWidget(createQuickBtn("后天", 2));
    quickDateLayout->addWidget(createQuickBtn("本周末", (6 - QDate::currentDate().dayOfWeek() + 7) % 7 + 1));
    quickDateLayout->addWidget(createQuickBtn("下周", 7));
    quickDateLayout->addStretch();
    
    searchLayout->addLayout(quickDateLayout);
    
    pageLayout->addWidget(searchPanel);
    
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
    connect(m_chatBtn, &QPushButton::clicked, [this](){ navigateTo(3); });
    connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::switchTheme);
    
    connect(m_logoutBtn, &QPushButton::clicked, [this](){
        LoginPage *login = new LoginPage();
        login->setAttribute(Qt::WA_DeleteOnClose);
        login->show();
        this->close();
        this->deleteLater();
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
                        // 改签也走同一个选座弹窗
                        m_pendingFlightId = flight.flight_id;
                        m_pendingFlightSeats = flight.rest_seats;
                        TcpClient::getInstance()->getOccupiedSeats(flight.flight_id);
                    } else {
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
    
    connect(TcpClient::getInstance(), &TcpClient::occupiedSeatsResult, this, &MainWindow::onOccupiedSeatsReceived);
    
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
            msgBox.setText("改签失败！\n\n可能原因：\n• 只能改签到相同出发地和目的地的航班\n• 目标航班无余票");
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
    
    // 修改密码
    connect(m_profilePage, &ProfilePage::changePassword, this, [this](const QString& oldPass, const QString& newPass){
        TcpClient::getInstance()->changePassword(m_username, oldPass, newPass);
    });
    connect(TcpClient::getInstance(), &TcpClient::changePasswordResult, this, [this](bool success){
        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok, "确定");
        if (success) {
            msgBox.setWindowTitle("修改成功");
            msgBox.setText("密码已修改，请重新登录");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
            // 退出登录
            LoginPage *login = new LoginPage();
            login->setAttribute(Qt::WA_DeleteOnClose);
            login->show();
            this->close();
            this->deleteLater();
        } else {
            msgBox.setWindowTitle("修改失败");
            msgBox.setText("当前密码错误或修改失败");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
    });
}

void MainWindow::performSearch()
{
    QString dep = m_departureCombo->currentText().trimmed();
    QString dest = m_destinationCombo->currentText().trimmed();
    
    if (dep.isEmpty() || dest.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入出发地和目的地");
        return;
    }
    
    QDate date = m_dateLimitCheckBox->isChecked() ? m_dateEdit->date() : QDate();
    
    TcpClient::getInstance()->queryFlights(dep, dest, date);
}

void MainWindow::onCitiesReceived(const QStringList& cities)
{
    m_cities = cities;
    m_departureCombo->clear();
    m_destinationCombo->clear();
    
    m_departureCombo->addItem(""); 
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
        if (!m_changingOrderId.isEmpty()) {
            TcpClient::getInstance()->changeTicket(m_changingOrderId, m_pendingFlightId, selectedSeat);
            m_changingOrderId.clear();
        } else {
            TcpClient::getInstance()->bookTicket(m_username, m_pendingFlightId, selectedSeat);
        }
    }
    m_pendingFlightId.clear();
}


void MainWindow::setUsername(const QString& username)
{
    m_username = username;
    m_chatWidget->setUsername(username);
}

void MainWindow::navigateTo(int index)
{
    m_stack->setCurrentIndex(index);
    
    m_flightBtn->setChecked(index == 0);
    m_ordersBtn->setChecked(index == 1);
    m_profileBtn->setChecked(index == 2);
    m_chatBtn->setChecked(index == 3);

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
    if (m_chatWidget) {
        m_chatWidget->setTheme(m_isDarkTheme);
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
            border-radius: 12px;
            padding: 12px 20px;
            font-size: 15px;
            color: #f1f5f9;
            font-weight: 500;
        }
        
        QDateEdit#DateEdit:hover {
            border-color: #475569;
            background-color: #1e293b;
        }
        
        QDateEdit#DateEdit:focus {
            border-color: #3b82f6;
            background-color: #1e293b;
        }
        
        QDateEdit#DateEdit::drop-down {
            border: none;
            width: 40px;
            subcontrol-origin: padding;
            subcontrol-position: right center;
            background: transparent;
        }
        
        /* ========== 日期限制复选框 ========== */
        QCheckBox#DateLimitCheckBox {
            color: #94a3b8;
            font-size: 13px;
            spacing: 8px;
            background-color: transparent;
            border: none;
        }
        
        QCheckBox#DateLimitCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #475569;
            background-color: #0f172a;
        }
        
        QCheckBox#DateLimitCheckBox::indicator:hover {
            border-color: #3b82f6;
            background-color: #1e293b;
        }
        
        QCheckBox#DateLimitCheckBox::indicator:checked {
            background-color: #3b82f6;
            border-color: #3b82f6;
        }
        
        /* 日历弹出框样式 */
        QCalendarWidget {
            background-color: #1e293b;
            border: 2px solid #334155;
            border-radius: 16px;
        }
        
        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background-color: #0f172a;
            border-top-left-radius: 14px;
            border-top-right-radius: 14px;
            padding: 12px 16px;
            min-height: 50px;
        }
        
        QCalendarWidget QToolButton {
            color: #f1f5f9;
            background-color: transparent;
            border: none;
            border-radius: 8px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: bold;
            margin: 2px;
        }
        
        QCalendarWidget QToolButton:hover {
            background-color: #334155;
        }
        
        QCalendarWidget QToolButton:pressed {
            background-color: #3b82f6;
        }
        
        QCalendarWidget QToolButton#qt_calendar_prevmonth,
        QCalendarWidget QToolButton#qt_calendar_nextmonth {
            min-width: 36px;
            min-height: 36px;
            border-radius: 18px;
            font-size: 18px;
        }
        
        QCalendarWidget QMenu {
            background-color: #1e293b;
            border: 1px solid #334155;
            border-radius: 8px;
            color: #f1f5f9;
            padding: 4px;
        }
        
        QCalendarWidget QMenu::item {
            padding: 8px 20px;
            border-radius: 4px;
        }
        
        QCalendarWidget QMenu::item:selected {
            background-color: #3b82f6;
        }
        
        QCalendarWidget QSpinBox {
            background-color: #0f172a;
            border: 1px solid #334155;
            border-radius: 8px;
            color: #f1f5f9;
            padding: 6px 12px;
            font-size: 14px;
            font-weight: bold;
            min-width: 80px;
        }
        
        QCalendarWidget QSpinBox::up-button,
        QCalendarWidget QSpinBox::down-button {
            width: 20px;
            border: none;
            background: transparent;
        }
        
        /* 星期标题 */
        QCalendarWidget QWidget { alternate-background-color: #1e293b; }
        
        QCalendarWidget QAbstractItemView:enabled {
            background-color: #1e293b;
            color: #e2e8f0;
            selection-background-color: transparent;
            selection-color: white;
            outline: none;
            font-size: 14px;
        }
        
        QCalendarWidget QAbstractItemView:disabled {
            color: #475569;
        }
        
        QCalendarWidget QTableView {
            border: none;
            border-bottom-left-radius: 14px;
            border-bottom-right-radius: 14px;
            padding: 8px;
        }
        
        QCalendarWidget QTableView::item {
            padding: 0px;
        }
        
        QCalendarWidget QTableView::item:selected {
            background-color: #3b82f6;
            border-radius: 20px;
            color: white;
        }
        
        QCalendarWidget QTableView::item:hover {
            background-color: #334155;
            border-radius: 20px;
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
        QLabel#TimeLabel { font-size: 28px; font-weight: 700; color: #60a5fa; }
        QLabel#CityLabel { font-size: 16px; font-weight: 500; color: #f1f5f9; }
        QLabel#AirportLabel { font-size: 12px; color: #64748b; }
        QLabel#PriceLabel { font-size: 24px; font-weight: 700; color: #fb923c; }
        QLabel#DurationLabel { font-size: 12px; color: #64748b; }
        QLabel#SeatsLabel { font-size: 13px; color: #94a3b8; }
        QLabel#SeatsLabelLow { font-size: 13px; color: #f87171; font-weight: 600; }
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
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            padding: 12px 20px;
            font-size: 15px;
            color: #1e293b;
            font-weight: 500;
        }
        
        QDateEdit#DateEdit:hover {
            border-color: #cbd5e1;
            background-color: #f8fafc;
        }
        
        QDateEdit#DateEdit:focus {
            border-color: #3b82f6;
            background-color: #f8fafc;
        }
        
        QDateEdit#DateEdit::drop-down {
            border: none;
            width: 40px;
            subcontrol-origin: padding;
            subcontrol-position: right center;
            background: transparent;
        }
        
        /* ========== 日期限制复选框 ========== */
        QCheckBox#DateLimitCheckBox {
            color: #64748b;
            font-size: 13px;
            spacing: 8px;
            background-color: transparent;
            border: none;
        }
        
        QCheckBox#DateLimitCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #cbd5e1;
            background-color: #f8fafc;
        }
        
        QCheckBox#DateLimitCheckBox::indicator:hover {
            border-color: #3b82f6;
            background-color: white;
        }
        
        QCheckBox#DateLimitCheckBox::indicator:checked {
            background-color: #3b82f6;
            border-color: #3b82f6;
        }
        
        /* 日历弹出框样式 */
        QCalendarWidget {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 16px;
        }
        
        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background-color: #f8fafc;
            border-top-left-radius: 14px;
            border-top-right-radius: 14px;
            padding: 12px 16px;
            min-height: 50px;
        }
        
        QCalendarWidget QToolButton {
            color: #1e293b;
            background-color: transparent;
            border: none;
            border-radius: 8px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: bold;
            margin: 2px;
        }
        
        QCalendarWidget QToolButton:hover {
            background-color: #e2e8f0;
        }
        
        QCalendarWidget QToolButton:pressed {
            background-color: #3b82f6;
            color: white;
        }
        
        QCalendarWidget QToolButton#qt_calendar_prevmonth,
        QCalendarWidget QToolButton#qt_calendar_nextmonth {
            min-width: 36px;
            min-height: 36px;
            border-radius: 18px;
            font-size: 18px;
        }
        
        QCalendarWidget QMenu {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            color: #1e293b;
            padding: 4px;
        }
        
        QCalendarWidget QMenu::item {
            padding: 8px 20px;
            border-radius: 4px;
        }
        
        QCalendarWidget QMenu::item:selected {
            background-color: #3b82f6;
            color: white;
        }
        
        QCalendarWidget QSpinBox {
            background-color: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            color: #1e293b;
            padding: 6px 12px;
            font-size: 14px;
            font-weight: bold;
            min-width: 80px;
        }
        
        QCalendarWidget QSpinBox::up-button,
        QCalendarWidget QSpinBox::down-button {
            width: 20px;
            border: none;
            background: transparent;
        }
        
        /* 星期标题 */
        QCalendarWidget QWidget { alternate-background-color: #ffffff; }
        
        QCalendarWidget QAbstractItemView:enabled {
            background-color: #ffffff;
            color: #1e293b;
            selection-background-color: transparent;
            selection-color: white;
            outline: none;
            font-size: 14px;
        }
        
        QCalendarWidget QAbstractItemView:disabled {
            color: #94a3b8;
        }
        
        QCalendarWidget QTableView {
            border: none;
            border-bottom-left-radius: 14px;
            border-bottom-right-radius: 14px;
            padding: 8px;
        }
        
        QCalendarWidget QTableView::item {
            padding: 0px;
        }
        
        QCalendarWidget QTableView::item:selected {
            background-color: #3b82f6;
            border-radius: 20px;
            color: white;
        }
        
        QCalendarWidget QTableView::item:hover {
            background-color: #f1f5f9;
            border-radius: 20px;
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
        QLabel#TimeLabel { font-size: 28px; font-weight: 700; color: #3b82f6; }
        QLabel#CityLabel { font-size: 16px; font-weight: 500; color: #1e293b; }
        QLabel#AirportLabel { font-size: 12px; color: #64748b; }
        QLabel#PriceLabel { font-size: 24px; font-weight: 700; color: #f97316; }
        QLabel#DurationLabel { font-size: 12px; color: #64748b; }
        QLabel#SeatsLabel { font-size: 13px; color: #64748b; }
        QLabel#SeatsLabelLow { font-size: 13px; color: #ef4444; font-weight: 600; }
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
    
    if (m_chatWidget) {
        m_chatWidget->setTheme(m_isDarkTheme);
    }
}

QGraphicsDropShadowEffect* MainWindow::createShadow(QColor color, int blur, int offsetY)
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(blur);
    shadow->setColor(color);
    shadow->setOffset(0, offsetY);
    return shadow;
}
