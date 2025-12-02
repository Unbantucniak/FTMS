#include "register_page.h"
#include "network/tcp_client.h"
#include <QMessageBox>
#include <QRegularExpression>

RegisterPage::RegisterPage(QWidget *parent) : QWidget(parent) {
    setupUI();

    connect(registerBtn, &QPushButton::clicked, this, [this](){
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        QString confirm = confirmPasswordEdit->text();
        QString realName = realNameEdit->text();
        QString phone = phoneEdit->text();

        if (username.isEmpty() || password.isEmpty() || realName.isEmpty() || phone.isEmpty()) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("提示");
            msgBox.setText("请填写所有信息");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok, "确定");
            msgBox.exec();
            return;
        }
        if (password != confirm) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("提示");
            msgBox.setText("两次密码输入不一致");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok, "确定");
            msgBox.exec();
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
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("注册成功");
            msgBox.setText("注册成功，请登录");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok, "确定");
            msgBox.exec();
            this->close();
        } else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("注册失败");
            msgBox.setText("用户名可能已存在");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok, "确定");
            msgBox.exec();
        }
    });

    connect(TcpClient::getInstance(), &TcpClient::checkUsernameResult, this, [this](bool exist){
        if (exist) {
            usernameStatusLabel->setText("用户名已存在");
            usernameStatusLabel->setStyleSheet("color: red; font-size: 12px;");
        } else {
            usernameStatusLabel->setText("用户名可用");
            usernameStatusLabel->setStyleSheet("color: green; font-size: 12px;");
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
            passwordStatusLabel->setText("密码一致");
            passwordStatusLabel->setStyleSheet("color: green; font-size: 12px;");
        } else {
            passwordStatusLabel->setText("密码不一致");
            passwordStatusLabel->setStyleSheet("color: red; font-size: 12px;");
        }
    };
    connect(passwordEdit, &QLineEdit::textChanged, this, checkPassword);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, checkPassword);
}

void RegisterPage::setupUI() {
    this->setWindowTitle("用户注册");
    this->setMinimumSize(450, 600);
    this->resize(450, 600);
    this->setStyleSheet(R"(
        QWidget { background-color: #f0f2f5; font-family: 'Segoe UI', 'Microsoft YaHei'; }
        QLineEdit { 
            padding: 10px; 
            border: 1px solid #dcdfe6; 
            border-radius: 5px; 
            font-size: 14px;
            background-color: white;
            color: #333;
        }
        QLineEdit:focus { border: 1px solid #0078d7; }
        QPushButton { 
            padding: 10px; 
            border-radius: 5px; 
            font-size: 16px; 
            font-weight: bold;
        }
        QLabel { font-size: 14px; color: #333; }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel("新用户注册", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #333; margin-bottom: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("用户名");
    usernameStatusLabel = new QLabel("", this);
    QVBoxLayout *userLayout = new QVBoxLayout();
    userLayout->addWidget(usernameEdit);
    userLayout->addWidget(usernameStatusLabel);
    userLayout->setSpacing(2);
    userLayout->setContentsMargins(0,0,0,0);
    // formLayout->addRow("用户名:", usernameEdit); // Replace this
    formLayout->addRow("用户名:", userLayout);

    realNameEdit = new QLineEdit(this);
    realNameEdit->setPlaceholderText("真实姓名");
    formLayout->addRow("真实姓名:", realNameEdit);

    phoneEdit = new QLineEdit(this);
    phoneEdit->setPlaceholderText("联系电话");
    formLayout->addRow("联系电话:", phoneEdit);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("密码");
    formLayout->addRow("密码:", passwordEdit);

    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setPlaceholderText("确认密码");
    
    passwordStatusLabel = new QLabel("", this);
    QVBoxLayout *passLayout = new QVBoxLayout();
    passLayout->addWidget(confirmPasswordEdit);
    passLayout->addWidget(passwordStatusLabel);
    passLayout->setSpacing(2);
    passLayout->setContentsMargins(0,0,0,0);
    
    formLayout->addRow("确认密码:", passLayout);

    mainLayout->addLayout(formLayout);

    registerBtn = new QPushButton("立即注册", this);
    registerBtn->setStyleSheet("background-color: #28a745; color: white; border: none;");
    registerBtn->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(registerBtn);

    backBtn = new QPushButton("返回登录", this);
    backBtn->setStyleSheet("background-color: transparent; color: #0078d7; border: none; text-decoration: underline;");
    backBtn->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(backBtn);
}