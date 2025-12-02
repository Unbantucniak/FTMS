#include "profile_page.h"
#include <QMessageBox>

ProfilePage::ProfilePage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(40, 40, 40, 40); // 增加边距
    mainLayout->setSpacing(20);
    
    QLabel *title = new QLabel("个人信息", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #0078d7; margin-bottom: 20px;");
    mainLayout->addWidget(title);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(20);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    auto createStyledEdit = [this](bool readOnly) {
        QLineEdit *edit = new QLineEdit(this);
        edit->setReadOnly(readOnly);
        edit->setMinimumHeight(35);
        if(readOnly) {
            edit->setStyleSheet("background-color: transparent; border: none; border-bottom: 1px solid #ccc; color: #888;");
        }
        return edit;
    };

    m_usernameEdit = createStyledEdit(true);
    m_realNameEdit = createStyledEdit(true);
    
    m_phoneEdit = new QLineEdit(this);
    m_phoneEdit->setReadOnly(true); // Default read-only
    m_phoneEdit->setMinimumHeight(35);
    m_phoneEdit->setStyleSheet("background-color: transparent; border: none; border-bottom: 1px solid #ccc; color: #888;");
    
    QPushButton *editPhoneBtn = new QPushButton("修改", this);
    editPhoneBtn->setCursor(Qt::PointingHandCursor);
    editPhoneBtn->setFixedWidth(60);
    connect(editPhoneBtn, &QPushButton::clicked, this, [this](){
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("确认修改");
        msgBox.setText("您确定要修改联系方式吗？");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setButtonText(QMessageBox::Yes, "是");
        msgBox.setButtonText(QMessageBox::No, "否");
        if (msgBox.exec() == QMessageBox::Yes) {
            m_phoneEdit->setReadOnly(false);
            m_phoneEdit->setStyleSheet(""); // Reset style to default editable
            m_phoneEdit->setFocus();
        }
    });

    formLayout->addRow("用户名:", m_usernameEdit);
    formLayout->addRow("真实姓名:", m_realNameEdit);
    
    QHBoxLayout *phoneLayout = new QHBoxLayout();
    phoneLayout->addWidget(m_phoneEdit);
    phoneLayout->addWidget(editPhoneBtn);
    formLayout->addRow("电话:", phoneLayout);
    
    mainLayout->addLayout(formLayout);

    mainLayout->addSpacing(20);

    m_saveBtn = new QPushButton("保存修改", this);
    m_saveBtn->setFixedWidth(150);
    m_saveBtn->setMinimumHeight(40);
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    connect(m_saveBtn, &QPushButton::clicked, this, [this](){
        User user;
        user.username = m_usernameEdit->text();
        user.real_name = m_realNameEdit->text();
        user.phone = m_phoneEdit->text();
        emit updateUserInfo(user);
    });
    mainLayout->addWidget(m_saveBtn, 0, Qt::AlignRight);
}

void ProfilePage::setUserInfo(const User& user)
{
    m_usernameEdit->setText(user.username);
    
    // Mask real name (e.g., 张三 -> 张*)
    QString maskedName = user.real_name;
    if (maskedName.length() > 1) {
        maskedName.replace(1, 1, "*");
    }
    m_realNameEdit->setText(maskedName);
    
    m_phoneEdit->setText(user.phone);
}