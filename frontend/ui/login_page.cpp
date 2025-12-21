#include "login_page.h"
#include "network/tcp_client.h"
#include "main_window.h"
#include "register_page.h"
#include <QMessageBox>
#include <QTimer>

LoginPage::LoginPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    applyStyles();
    setupConnections();
    
    TcpClient::getInstance()->connectToServer("127.0.0.1", 12345);
}

LoginPage::~LoginPage()
{
    disconnect(TcpClient::getInstance(), nullptr, this, nullptr);
}

void LoginPage::setupUI()
{
    setWindowTitle("扶摇航空 - 登录");
    setMinimumSize(900, 600);
    resize(1000, 700);
    
    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    QWidget *brandPanel = new QWidget(this);
    brandPanel->setObjectName("BrandPanel");
    brandPanel->setMinimumWidth(400);
    
    QVBoxLayout *brandLayout = new QVBoxLayout(brandPanel);
    brandLayout->setContentsMargins(50, 60, 50, 60);
    brandLayout->setSpacing(20);
    

    m_logoLabel = new QLabel("✈", brandPanel);
    m_logoLabel->setObjectName("LogoLabel");
    m_logoLabel->setAlignment(Qt::AlignLeft);
    brandLayout->addWidget(m_logoLabel);
    
    brandLayout->addStretch(1);
    
    // 品牌标语
    QLabel *sloganLabel = new QLabel("扶摇直上\n畅行天下", brandPanel);
    sloganLabel->setObjectName("SloganLabel");
    brandLayout->addWidget(sloganLabel);
    
    QLabel *descLabel = new QLabel("专业的航班票务管理系统\n为您提供便捷、高效的出行服务", brandPanel);
    descLabel->setObjectName("DescLabel");
    brandLayout->addWidget(descLabel);
    
    brandLayout->addStretch(3);
    
    mainLayout->addWidget(brandPanel, 4);
    
    QWidget *loginPanel = new QWidget(this);
    loginPanel->setObjectName("LoginPanel");
    
    QVBoxLayout *loginPanelLayout = new QVBoxLayout(loginPanel);
    loginPanelLayout->setContentsMargins(60, 60, 60, 60);
    loginPanelLayout->setAlignment(Qt::AlignCenter);
    
    // 登录卡片
    m_loginCard = new QWidget(loginPanel);
    m_loginCard->setObjectName("LoginCard");
    m_loginCard->setFixedSize(380, 480);
    
    // 添加阴影效果
    m_loginCard->setGraphicsEffect(createShadow(QColor(0, 0, 0, 40), 30, 10));
    
    QVBoxLayout *cardLayout = new QVBoxLayout(m_loginCard);
    cardLayout->setContentsMargins(40, 45, 40, 40);
    cardLayout->setSpacing(0);
    
    // 标题
    m_titleLabel = new QLabel("欢迎回来", m_loginCard);
    m_titleLabel->setObjectName("TitleLabel");
    cardLayout->addWidget(m_titleLabel);
    
    m_subtitleLabel = new QLabel("登录您的账户以继续", m_loginCard);
    m_subtitleLabel->setObjectName("SubtitleLabel");
    cardLayout->addWidget(m_subtitleLabel);
    
    cardLayout->addSpacing(35);
    
    // 用户名输入框
    QLabel *userLabel = new QLabel("用户名", m_loginCard);
    userLabel->setObjectName("InputLabel");
    cardLayout->addWidget(userLabel);
    cardLayout->addSpacing(8);
    
    m_usernameEdit = new QLineEdit(m_loginCard);
    m_usernameEdit->setObjectName("InputField");
    m_usernameEdit->setPlaceholderText("请输入用户名");
    m_usernameEdit->setMinimumHeight(48);
    cardLayout->addWidget(m_usernameEdit);
    
    cardLayout->addSpacing(20);
    
    // 密码输入框
    QLabel *passLabel = new QLabel("密码", m_loginCard);
    passLabel->setObjectName("InputLabel");
    cardLayout->addWidget(passLabel);
    cardLayout->addSpacing(8);
    
    m_passwordEdit = new QLineEdit(m_loginCard);
    m_passwordEdit->setObjectName("InputField");
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumHeight(48);
    cardLayout->addWidget(m_passwordEdit);
    
    cardLayout->addSpacing(30);
    
    // 登录按钮
    m_loginBtn = new QPushButton("登  录", m_loginCard);
    m_loginBtn->setObjectName("LoginBtn");
    m_loginBtn->setMinimumHeight(50);
    m_loginBtn->setCursor(Qt::PointingHandCursor);
    cardLayout->addWidget(m_loginBtn);
    
    cardLayout->addSpacing(15);
    
    // 注册链接
    QHBoxLayout *registerLayout = new QHBoxLayout();
    QLabel *noAccountLabel = new QLabel("还没有账户？", m_loginCard);
    noAccountLabel->setObjectName("HintLabel");
    m_registerBtn = new QPushButton("立即注册", m_loginCard);
    m_registerBtn->setObjectName("LinkBtn");
    m_registerBtn->setCursor(Qt::PointingHandCursor);
    registerLayout->addStretch();
    registerLayout->addWidget(noAccountLabel);
    registerLayout->addWidget(m_registerBtn);
    registerLayout->addStretch();
    cardLayout->addLayout(registerLayout);
    
    cardLayout->addStretch();
    
    loginPanelLayout->addWidget(m_loginCard, 0, Qt::AlignCenter);
    
    // 版权信息
    m_copyrightLabel = new QLabel("© 2025 扶摇航空 All Rights Reserved", loginPanel);
    m_copyrightLabel->setObjectName("CopyrightLabel");
    m_copyrightLabel->setAlignment(Qt::AlignCenter);
    loginPanelLayout->addWidget(m_copyrightLabel);
    
    mainLayout->addWidget(loginPanel, 5);
}

void LoginPage::applyStyles()
{
    setStyleSheet(R"(
        /* 品牌面板 */
        QWidget#BrandPanel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #667eea, stop:0.5 #764ba2, stop:1 #f093fb);
        }
        
        QLabel#LogoLabel {
            font-size: 64px;
            color: white;
        }
        
        QLabel#SloganLabel {
            font-size: 42px;
            font-weight: bold;
            color: white;
            line-height: 1.3;
        }
        
        QLabel#DescLabel {
            font-size: 15px;
            color: rgba(255, 255, 255, 0.85);
            line-height: 1.6;
        }
        
        /* 登录面板 */
        QWidget#LoginPanel {
            background-color: #f8fafc;
        }
        
        QWidget#LoginCard {
            background-color: white;
            border-radius: 16px;
        }
        
        QLabel#TitleLabel {
            font-size: 28px;
            font-weight: bold;
            color: #1e293b;
        }
        
        QLabel#SubtitleLabel {
            font-size: 14px;
            color: #64748b;
            margin-top: 8px;
        }
        
        QLabel#InputLabel {
            font-size: 13px;
            font-weight: 600;
            color: #475569;
        }
        
        QLineEdit#InputField {
            padding: 12px 16px;
            border: 2px solid #e2e8f0;
            border-radius: 10px;
            font-size: 14px;
            color: #334155;
            background-color: #f8fafc;
        }
        
        QLineEdit#InputField:focus {
            border-color: #667eea;
            background-color: white;
        }
        
        QLineEdit#InputField::placeholder {
            color: #94a3b8;
        }
        
        QPushButton#LoginBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #667eea, stop:1 #764ba2);
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 16px;
            font-weight: bold;
            letter-spacing: 2px;
        }
        
        QPushButton#LoginBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #5a67d8, stop:1 #6b46c1);
        }
        
        QPushButton#LoginBtn:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #4c51bf, stop:1 #553c9a);
        }
        
        QLabel#HintLabel {
            font-size: 13px;
            color: #64748b;
        }
        
        QPushButton#LinkBtn {
            background: transparent;
            border: none;
            color: #667eea;
            font-size: 13px;
            font-weight: bold;
            padding: 0;
        }
        
        QPushButton#LinkBtn:hover {
            color: #5a67d8;
            text-decoration: underline;
        }
        
        QLabel#CopyrightLabel {
            font-size: 12px;
            color: #94a3b8;
            margin-top: 20px;
        }
    )");
}

void LoginPage::setupConnections()
{
    connect(m_loginBtn, &QPushButton::clicked, this, [this](){
        QString username = m_usernameEdit->text().trimmed();
        QString password = m_passwordEdit->text();
        
        if (username.isEmpty() || password.isEmpty()) {
            // 输入框抖动效果
            QPropertyAnimation *anim = new QPropertyAnimation(m_loginCard, "pos");
            QPoint startPos = m_loginCard->pos();
            anim->setDuration(50);
            anim->setLoopCount(4);
            anim->setStartValue(startPos + QPoint(-5, 0));
            anim->setEndValue(startPos + QPoint(5, 0));
            connect(anim, &QPropertyAnimation::finished, [this, startPos](){
                m_loginCard->move(startPos);
            });
            anim->start(QAbstractAnimation::DeleteWhenStopped);
            
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("提示");
            msgBox.setText("请输入用户名和密码");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok, "确定");
            msgBox.exec();
            return;
        }
        
        TcpClient::getInstance()->login(username, password);
    });
    
    // 回车登录
    connect(m_passwordEdit, &QLineEdit::returnPressed, m_loginBtn, &QPushButton::click);
    connect(m_usernameEdit, &QLineEdit::returnPressed, [this](){
        m_passwordEdit->setFocus();
    });
    
    connect(m_registerBtn, &QPushButton::clicked, this, [this](){
        RegisterPage *registerPage = new RegisterPage();
        registerPage->setAttribute(Qt::WA_DeleteOnClose);
        registerPage->setWindowModality(Qt::ApplicationModal);
        registerPage->show();
    });
    
    connect(TcpClient::getInstance(), &TcpClient::loginResult, this, [this](ResponseStatus status){
        if (status == Success) {
            // 先断开连接，防止重复触发
            disconnect(TcpClient::getInstance(), &TcpClient::loginResult, this, nullptr);
            MainWindow *w = new MainWindow();
            w->setUsername(m_usernameEdit->text());
            w->setAttribute(Qt::WA_DeleteOnClose);
            w->showMaximized();
            this->close();
        } else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("登录失败");
            msgBox.setText("用户名或密码错误");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok, "确定");
            msgBox.exec();
        }
    });
}

QGraphicsDropShadowEffect* LoginPage::createShadow(QColor color, int blur, int offsetY)
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(blur);
    shadow->setColor(color);
    shadow->setOffset(0, offsetY);
    return shadow;
}
