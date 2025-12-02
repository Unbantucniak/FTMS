#include "login_page.h"
#include "ui_login_page.h"
#include "network/tcp_client.h"
#include "main_window.h"
#include "register_page.h"
#include <QMessageBox>

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    // 删除UI文件中自带的标题标签，防止重叠
    delete ui->titleLabel;

    this->setWindowTitle("扶摇 - 登录");
    this->setMinimumSize(400, 500); // 设置最小尺寸防止压缩
    
    // 重构布局以添加标签
    if (this->layout()) {
        delete this->layout();
    }
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel("扶摇航空票务系统", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #0078d7; margin-bottom: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true); // 防止文字过长被截断
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(20);
    formLayout->setLabelAlignment(Qt::AlignLeft);
    
    // 设置标签样式
    QString labelStyle = "font-size: 15px; font-weight: bold; color: #555;";
    QLabel *userLabel = new QLabel("用户名:", this);
    userLabel->setStyleSheet(labelStyle);
    QLabel *passLabel = new QLabel("密  码:", this);
    passLabel->setStyleSheet(labelStyle);

    formLayout->addRow(userLabel, ui->usernameEdit);
    formLayout->addRow(passLabel, ui->passwordEdit);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(ui->loginButton);
    mainLayout->addWidget(ui->registerButton);

    // UI美化
    this->setStyleSheet(R"(
        QWidget { background-color: #f0f2f5; font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; }
        
        QLineEdit { 
            padding: 12px; 
            border: 1px solid #dcdfe6; 
            border-radius: 8px; 
            font-size: 14px;
            background-color: white;
            color: #333333;
            selection-background-color: #0078d7;
        }
        QLineEdit:focus { border: 1px solid #0078d7; background-color: #fff; }
        
        QPushButton { 
            background-color: #0078d7; 
            color: white; 
            border: none; 
            border-radius: 8px; 
            padding: 12px; 
            font-size: 16px; 
            font-weight: bold;
        }
        QPushButton:hover { background-color: #1084e3; }
        QPushButton:pressed { background-color: #006cc1; }
        
        QLabel { color: #333; font-size: 14px; font-weight: 500; }
    )");

    ui->loginButton->setCursor(Qt::PointingHandCursor);
    ui->registerButton->setCursor(Qt::PointingHandCursor);
    ui->registerButton->setStyleSheet("background-color: #28a745;"); // 绿色注册按钮

    // 连接到服务器
    TcpClient::getInstance()->connectToServer("127.0.0.1", 12345);
    
    connect(ui->loginButton, &QPushButton::clicked, this, [this](){
        QString username = ui->usernameEdit->text();
        QString password = ui->passwordEdit->text();
        if(username.isEmpty() || password.isEmpty()) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("提示");
            msgBox.setText("请输入用户名和密码");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok, "确定");
            msgBox.exec();
            return;
        }
        TcpClient::getInstance()->login(username, password);
    });

    connect(ui->registerButton, &QPushButton::clicked, this, [this](){
        RegisterPage *registerPage = new RegisterPage();
        registerPage->setAttribute(Qt::WA_DeleteOnClose);
        registerPage->setWindowModality(Qt::ApplicationModal);
        registerPage->show();
    });
    
    /* 
       注册结果现在由 RegisterPage 处理
    connect(TcpClient::getInstance(), &TcpClient::registerResult, this, [this](bool success){
        if (success) {
            QMessageBox::information(this, "注册成功", "注册成功，请登录。");
        } else {
            QMessageBox::warning(this, "注册失败", "注册失败，用户名可能已存在。");
        }
    });
    */
    
    connect(TcpClient::getInstance(), &TcpClient::loginResult, this, [this](ResponseStatus status){
        if (status == Success) {
            MainWindow *w = new MainWindow();
            w->setUsername(ui->usernameEdit->text());
            w->setAttribute(Qt::WA_DeleteOnClose);
            w->show();
            this->close();
        } else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("登录失败");
            msgBox.setText("用户名或密码错误。");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok, "确定");
            msgBox.exec();
        }
    });
}

LoginPage::~LoginPage()
{
    delete ui;
}
