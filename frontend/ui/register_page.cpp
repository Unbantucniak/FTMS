#include "register_page.h"
#include "network/tcp_client.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QGraphicsDropShadowEffect>

RegisterPage::RegisterPage(QWidget *parent) : QWidget(parent) {
    setupUI();

    connect(registerBtn, &QPushButton::clicked, this, [this](){
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        QString confirm = confirmPasswordEdit->text();
        QString realName = realNameEdit->text();
        QString phone = phoneEdit->text();

        if (username.isEmpty() || password.isEmpty() || realName.isEmpty() || phone.isEmpty()) {
            showMessage("提示", "请填写所有信息", false);
            return;
        }
        if (password != confirm) {
            showMessage("提示", "两次密码输入不一致", false);
            return;
        }
        if (password.length() < 6) {
            showMessage("提示", "密码长度至少6位", false);
            return;
        }

        User user;
        user.username = username;
        user.password = password;
        user.real_name = realName;
        user.phone = phone;

        TcpClient::getInstance()->registerUser(user);
    });

    connect(backBtn, &QPushButton::clicked, this, [this](){
        this->close();
    });

    connect(TcpClient::getInstance(), &TcpClient::registerResult, this, [this](bool success){
        if (success) {
            showMessage("注册成功", "🎉 注册成功，请登录", true);
            this->close();
        } else {
            showMessage("注册失败", "用户名可能已存在", false);
        }
    });

    connect(TcpClient::getInstance(), &TcpClient::checkUsernameResult, this, [this](bool exist){
        if (exist) {
            usernameStatusLabel->setText("✗ 用户名已存在");
            usernameStatusLabel->setStyleSheet("color: #ef4444; font-size: 12px; font-weight: 500;");
        } else {
            usernameStatusLabel->setText("✓ 用户名可用");
            usernameStatusLabel->setStyleSheet("color: #22c55e; font-size: 12px; font-weight: 500;");
        }
    });

    connect(usernameEdit, &QLineEdit::editingFinished, this, [this](){
        if(!usernameEdit->text().isEmpty()) {
            TcpClient::getInstance()->checkUsername(usernameEdit->text());
        }
    });

    auto checkPassword = [this](){
        QString p1 = passwordEdit->text();
        QString p2 = confirmPasswordEdit->text();
        if (p1.isEmpty() || p2.isEmpty()) {
            passwordStatusLabel->setText("");
            return;
        }
        if (p1 == p2) {
            passwordStatusLabel->setText("✓ 密码一致");
            passwordStatusLabel->setStyleSheet("color: #22c55e; font-size: 12px; font-weight: 500;");
        } else {
            passwordStatusLabel->setText("✗ 密码不一致");
            passwordStatusLabel->setStyleSheet("color: #ef4444; font-size: 12px; font-weight: 500;");
        }
    };
    connect(passwordEdit, &QLineEdit::textChanged, this, checkPassword);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, checkPassword);
}

void RegisterPage::showMessage(const QString& title, const QString& text, bool success) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(success ? QMessageBox::Information : QMessageBox::Warning);
    msgBox.setStyleSheet(R"(
        QMessageBox { background-color: #1e293b; }
        QMessageBox QLabel { color: #f1f5f9; font-size: 14px; }
        QPushButton { 
            background-color: #3b82f6; 
            color: white;
            border: none; 
            border-radius: 6px; 
            padding: 8px 24px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #2563eb; }
    )");
    msgBox.exec();
}

void RegisterPage::setupUI() {
    this->setWindowTitle("扶摇航空 - 注册");
    this->setMinimumSize(500, 680);
    this->resize(500, 720);
    
    // 深色背景
    this->setStyleSheet(R"(
        QWidget#RegisterPage { 
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #0f172a, stop:1 #1e293b);
        }
    )");
    this->setObjectName("RegisterPage");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 40, 60, 40);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 注册卡片
    QWidget *card = new QWidget(this);
    card->setObjectName("RegisterCard");
    card->setFixedSize(380, 620);
    card->setStyleSheet(R"(
        QWidget#RegisterCard {
            background-color: #1e293b;
            border-radius: 16px;
            border: 1px solid #334155;
        }
    )");
    
    // 阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(40);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 10);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(35, 35, 35, 30);
    cardLayout->setSpacing(0);


    QLabel *logoLabel = new QLabel("✈", card);
    logoLabel->setStyleSheet("font-size: 40px;");
    logoLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(logoLabel);
    
    cardLayout->addSpacing(8);

    // 标题
    QLabel *titleLabel = new QLabel("创建新账户", card);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #f1f5f9;");
    titleLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("加入扶摇航空，开启您的旅程", card);
    subtitleLabel->setStyleSheet("font-size: 13px; color: #64748b; margin-top: 5px;");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(subtitleLabel);

    cardLayout->addSpacing(25);

    // 表单样式
    QString inputStyle = R"(
        QLineEdit {
            padding: 12px 16px;
            border: 2px solid #334155;
            border-radius: 10px;
            font-size: 14px;
            color: #f1f5f9;
            background-color: #0f172a;
        }
        QLineEdit:focus {
            border-color: #3b82f6;
            background-color: #1e293b;
        }
        QLineEdit::placeholder {
            color: #64748b;
        }
    )";
    
    QString labelStyle = "font-size: 12px; font-weight: 600; color: #94a3b8; margin-bottom: 4px;";

    // 用户名
    QLabel *userLabel = new QLabel("用户名", card);
    userLabel->setStyleSheet(labelStyle);
    cardLayout->addWidget(userLabel);
    
    usernameEdit = new QLineEdit(card);
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setMinimumHeight(44);
    usernameEdit->setStyleSheet(inputStyle);
    cardLayout->addWidget(usernameEdit);
    
    usernameStatusLabel = new QLabel("", card);
    usernameStatusLabel->setFixedHeight(18);
    cardLayout->addWidget(usernameStatusLabel);

    // 真实姓名
    QLabel *nameLabel = new QLabel("真实姓名", card);
    nameLabel->setStyleSheet(labelStyle);
    cardLayout->addWidget(nameLabel);
    
    realNameEdit = new QLineEdit(card);
    realNameEdit->setPlaceholderText("请输入真实姓名");
    realNameEdit->setMinimumHeight(44);
    realNameEdit->setStyleSheet(inputStyle);
    cardLayout->addWidget(realNameEdit);
    
    cardLayout->addSpacing(12);

    // 手机号
    QLabel *phoneLabel = new QLabel("手机号", card);
    phoneLabel->setStyleSheet(labelStyle);
    cardLayout->addWidget(phoneLabel);
    
    phoneEdit = new QLineEdit(card);
    phoneEdit->setPlaceholderText("请输入手机号");
    phoneEdit->setMinimumHeight(44);
    phoneEdit->setStyleSheet(inputStyle);
    cardLayout->addWidget(phoneEdit);
    
    cardLayout->addSpacing(12);

    // 密码
    QLabel *passLabel = new QLabel("密码", card);
    passLabel->setStyleSheet(labelStyle);
    cardLayout->addWidget(passLabel);
    
    passwordEdit = new QLineEdit(card);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入密码（至少6位）");
    passwordEdit->setMinimumHeight(44);
    passwordEdit->setStyleSheet(inputStyle);
    cardLayout->addWidget(passwordEdit);
    
    cardLayout->addSpacing(12);

    // 确认密码
    QLabel *confirmLabel = new QLabel("确认密码", card);
    confirmLabel->setStyleSheet(labelStyle);
    cardLayout->addWidget(confirmLabel);
    
    confirmPasswordEdit = new QLineEdit(card);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setPlaceholderText("请再次输入密码");
    confirmPasswordEdit->setMinimumHeight(44);
    confirmPasswordEdit->setStyleSheet(inputStyle);
    cardLayout->addWidget(confirmPasswordEdit);
    
    passwordStatusLabel = new QLabel("", card);
    passwordStatusLabel->setFixedHeight(18);
    cardLayout->addWidget(passwordStatusLabel);

    cardLayout->addSpacing(10);

    // 注册按钮
    registerBtn = new QPushButton("注 册", card);
    registerBtn->setMinimumHeight(48);
    registerBtn->setCursor(Qt::PointingHandCursor);
    registerBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #22c55e, stop:1 #16a34a);
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #16a34a, stop:1 #15803d);
        }
        QPushButton:pressed {
            background-color: #15803d;
        }
    )");
    cardLayout->addWidget(registerBtn);

    cardLayout->addSpacing(12);

    // 返回登录
    QHBoxLayout *backLayout = new QHBoxLayout();
    QLabel *hintLabel = new QLabel("已有账户？", card);
    hintLabel->setStyleSheet("font-size: 13px; color: #64748b;");
    
    backBtn = new QPushButton("返回登录", card);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: #3b82f6;
            border: none;
            font-size: 13px;
            font-weight: bold;
            text-decoration: underline;
            padding: 0;
        }
        QPushButton:hover {
            color: #60a5fa;
        }
    )");
    
    backLayout->addStretch();
    backLayout->addWidget(hintLabel);
    backLayout->addWidget(backBtn);
    backLayout->addStretch();
    cardLayout->addLayout(backLayout);

    cardLayout->addStretch();

    mainLayout->addWidget(card, 0, Qt::AlignCenter);
}
