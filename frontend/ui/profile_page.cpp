#include "profile_page.h"
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <QRandomGenerator>

ProfilePage::ProfilePage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void ProfilePage::setupUI()
{
    setObjectName("ProfilePage");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 滚动区域
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("ProfileScrollArea");
    scrollArea->setStyleSheet(R"(
        QScrollArea#ProfileScrollArea { background: transparent; border: none; }
        QScrollBar:vertical {
            background: #1e293b;
            width: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #475569;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: #64748b; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )");
    
    QWidget *scrollContent = new QWidget();
    scrollContent->setObjectName("ProfileContent");
    scrollContent->setStyleSheet("QWidget#ProfileContent { background: transparent; }");
    
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(30, 30, 30, 40);
    contentLayout->setSpacing(25);
    
    // ========== 顶部欢迎区域 ==========
    QWidget *headerWidget = new QWidget();
    headerWidget->setObjectName("ProfileHeader");
    headerWidget->setStyleSheet(R"(
        QWidget#ProfileHeader {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #3b82f6, stop:1 #8b5cf6);
            border-radius: 16px;
            padding: 20px;
        }
    )");
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(30, 25, 30, 25);
    headerLayout->setSpacing(20);
    
    // 头像
    m_avatarLabel = new QLabel(headerWidget);
    m_avatarLabel->setFixedSize(80, 80);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setStyleSheet(R"(
        background-color: rgba(255, 255, 255, 0.2);
        border-radius: 40px;
        font-size: 36px;
        color: white;
    )");
    m_avatarLabel->setText("👤");
    headerLayout->addWidget(m_avatarLabel);
    
    // 欢迎信息
    QVBoxLayout *welcomeLayout = new QVBoxLayout();
    welcomeLayout->setSpacing(5);
    
    m_welcomeLabel = new QLabel("欢迎回来", headerWidget);
    m_welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    
    m_memberLevelLabel = new QLabel("✨ 普通会员", headerWidget);
    m_memberLevelLabel->setStyleSheet("font-size: 13px; color: rgba(255, 255, 255, 0.8);");
    
    welcomeLayout->addWidget(m_welcomeLabel);
    welcomeLayout->addWidget(m_memberLevelLabel);
    headerLayout->addLayout(welcomeLayout);
    
    headerLayout->addStretch();
    
    // 编辑按钮
    m_editBtn = new QPushButton("编辑资料", headerWidget);
    m_editBtn->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(255, 255, 255, 0.2);
            color: white;
            border: 1px solid rgba(255, 255, 255, 0.3);
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.3);
        }
    )");
    m_editBtn->setCursor(Qt::PointingHandCursor);
    connect(m_editBtn, &QPushButton::clicked, this, [this]() {
        m_isEditing = !m_isEditing;
        m_editBtn->setText(m_isEditing ? "取消编辑" : "编辑资料");
        m_phoneEdit->setReadOnly(!m_isEditing);
        m_saveBtn->setVisible(m_isEditing);
        
        updateTheme(m_isDarkTheme);
    });
    headerLayout->addWidget(m_editBtn);
    
    contentLayout->addWidget(headerWidget);
    
    // ========== 统计卡片区域 ==========
    QWidget *statsWidget = new QWidget();
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(15);
    
    auto createStatCard = [this](const QString& icon, const QString& label, QLabel*& valueLabel) {
        QWidget *card = new QWidget();
        card->setObjectName("StatCard");
        card->setStyleSheet(R"(
            QWidget#StatCard {
                background-color: #1e293b;
                border-radius: 12px;
                border: 1px solid #334155;
            }
        )");
        
        QVBoxLayout *layout = new QVBoxLayout(card);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(8);
        layout->setAlignment(Qt::AlignCenter);
        
        QLabel *iconLabel = new QLabel(icon);
        iconLabel->setStyleSheet("font-size: 28px;");
        iconLabel->setAlignment(Qt::AlignCenter);
        
        valueLabel = new QLabel("0");
        valueLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #f1f5f9;");
        valueLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *labelLabel = new QLabel(label);
        labelLabel->setStyleSheet("font-size: 12px; color: #64748b;");
        labelLabel->setAlignment(Qt::AlignCenter);
        
        layout->addWidget(iconLabel);
        layout->addWidget(valueLabel);
        layout->addWidget(labelLabel);
        
        return card;
    };
    
    // statsLayout->addWidget(createStatCard("📅", "注册年份", m_memberSinceLabel));
    statsLayout->addStretch();
    
    contentLayout->addWidget(statsWidget);
    
    // ========== 个人信息卡片 ==========
    QWidget *infoCard = new QWidget();
    infoCard->setObjectName("InfoCard");
    infoCard->setStyleSheet(R"(
        QWidget#InfoCard {
            background-color: #1e293b;
            border-radius: 16px;
            border: 1px solid #334155;
        }
    )");
    
    QVBoxLayout *infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setContentsMargins(25, 25, 25, 25);
    infoLayout->setSpacing(5);
    
    QLabel *infoTitle = new QLabel("📋  基本信息", infoCard);
    infoTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #f1f5f9; margin-bottom: 10px;");
    infoLayout->addWidget(infoTitle);
    
    // 分隔线
    QFrame *line1 = new QFrame(infoCard);
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet("background-color: #334155;");
    line1->setFixedHeight(1);
    infoLayout->addWidget(line1);
    infoLayout->addSpacing(15);
    
    // 信息行
    auto createInfoRow = [infoCard](const QString& icon, const QString& label, QLineEdit*& edit) {
        QWidget *row = new QWidget(infoCard);
        QHBoxLayout *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 10, 0, 10);
        rowLayout->setSpacing(15);
        
        QLabel *iconLabel = new QLabel(icon, row);
        iconLabel->setStyleSheet("font-size: 18px;");
        iconLabel->setFixedWidth(25);
        
        QLabel *labelText = new QLabel(label, row);
        labelText->setStyleSheet("font-size: 13px; color: #64748b;");
        labelText->setFixedWidth(70);
        
        edit = new QLineEdit(row);
        edit->setReadOnly(true);
        edit->setStyleSheet(R"(
            QLineEdit {
                background-color: transparent;
                border: none;
                border-bottom: 1px solid #334155;
                border-radius: 0;
                padding: 10px 0;
                font-size: 14px;
                color: #f1f5f9;
            }
        )");
        
        rowLayout->addWidget(iconLabel);
        rowLayout->addWidget(labelText);
        rowLayout->addWidget(edit, 1);
        
        return row;
    };
    
    infoLayout->addWidget(createInfoRow("👤", "用户名", m_usernameEdit));
    infoLayout->addWidget(createInfoRow("📝", "真实姓名", m_realNameEdit));
    infoLayout->addWidget(createInfoRow("📱", "手机号", m_phoneEdit));
    
    // 保存按钮
    m_saveBtn = new QPushButton("💾  保存修改", infoCard);
    m_saveBtn->setVisible(false);
    m_saveBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #22c55e, stop:1 #16a34a);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 12px 30px;
            font-size: 14px;
            font-weight: bold;
            margin-top: 15px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #16a34a, stop:1 #15803d);
        }
    )");
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    connect(m_saveBtn, &QPushButton::clicked, this, [this]() {
        User user;
        user.username = m_usernameEdit->text();
        user.real_name = m_currentUser.real_name;
        user.phone = m_phoneEdit->text();
        emit updateUserInfo(user);
        
        // 保存编辑的手机号到 currentUser
        m_currentUser.phone = m_phoneEdit->text();
        
        // 退出编辑模式
        m_isEditing = false;
        m_editBtn->setText("编辑资料");
        m_phoneEdit->setReadOnly(true);
        m_saveBtn->setVisible(false);
        
        // 恢复手机号为掩码格式
        QString phone = m_currentUser.phone;
        if (phone.length() >= 7) {
            phone = phone.left(3) + "****" + phone.mid(7);
        }
        m_phoneEdit->setText(phone);
        
        updateTheme(m_isDarkTheme);
    });
    infoLayout->addWidget(m_saveBtn, 0, Qt::AlignRight);
    
    contentLayout->addWidget(infoCard);
    
    // ========== 安全设置卡片 ==========
    QWidget *securityCard = new QWidget();
    securityCard->setObjectName("SecurityCard");
    securityCard->setStyleSheet(R"(
        QWidget#SecurityCard {
            background-color: #1e293b;
            border-radius: 16px;
            border: 1px solid #334155;
        }
    )");
    
    QVBoxLayout *securityLayout = new QVBoxLayout(securityCard);
    securityLayout->setContentsMargins(25, 25, 25, 25);
    securityLayout->setSpacing(15);
    
    QLabel *securityTitle = new QLabel("🔐  安全设置", securityCard);
    securityTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #f1f5f9;");
    securityLayout->addWidget(securityTitle);
    
    QFrame *line2 = new QFrame(securityCard);
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet("background-color: #334155;");
    line2->setFixedHeight(1);
    securityLayout->addWidget(line2);
    
    QLabel *passHint = new QLabel("修改密码后需要重新登录", securityCard);
    passHint->setStyleSheet("font-size: 12px; color: #64748b; margin-top: 5px;");
    securityLayout->addWidget(passHint);
    
    QString passInputStyle = R"(
        QLineEdit {
            background-color: #0f172a;
            border: 2px solid #334155;
            border-radius: 8px;
            padding: 12px 15px;
            font-size: 14px;
            color: #f1f5f9;
        }
        QLineEdit:focus {
            border-color: #3b82f6;
        }
        QLineEdit::placeholder {
            color: #64748b;
        }
    )";
    
    m_oldPasswordEdit = new QLineEdit(securityCard);
    m_oldPasswordEdit->setPlaceholderText("当前密码");
    m_oldPasswordEdit->setEchoMode(QLineEdit::Password);
    m_oldPasswordEdit->setStyleSheet(passInputStyle);
    m_oldPasswordEdit->setMinimumHeight(45);
    securityLayout->addWidget(m_oldPasswordEdit);
    
    m_newPasswordEdit = new QLineEdit(securityCard);
    m_newPasswordEdit->setPlaceholderText("新密码（至少6位）");
    m_newPasswordEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordEdit->setStyleSheet(passInputStyle);
    m_newPasswordEdit->setMinimumHeight(45);
    securityLayout->addWidget(m_newPasswordEdit);
    
    m_confirmPasswordEdit = new QLineEdit(securityCard);
    m_confirmPasswordEdit->setPlaceholderText("确认新密码");
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setStyleSheet(passInputStyle);
    m_confirmPasswordEdit->setMinimumHeight(45);
    securityLayout->addWidget(m_confirmPasswordEdit);
    
    m_changePassBtn = new QPushButton("🔑  修改密码", securityCard);
    m_changePassBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #f59e0b, stop:1 #d97706);
            color: white;
            border: none;
            border-radius: 10px;
            padding: 12px 30px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #d97706, stop:1 #b45309);
        }
    )");
    m_changePassBtn->setCursor(Qt::PointingHandCursor);
    connect(m_changePassBtn, &QPushButton::clicked, this, [this]() {
        QString oldPass = m_oldPasswordEdit->text();
        QString newPass = m_newPasswordEdit->text();
        QString confirmPass = m_confirmPasswordEdit->text();
        
        if (oldPass.isEmpty() || newPass.isEmpty() || confirmPass.isEmpty()) {
            showMessage("提示", "请填写所有密码字段", false);
            return;
        }
        
        if (newPass.length() < 6) {
            showMessage("提示", "新密码长度至少6位", false);
            return;
        }
        
        if (newPass != confirmPass) {
            showMessage("提示", "两次输入的新密码不一致", false);
            return;
        }
        
        emit changePassword(oldPass, newPass);
        
        m_oldPasswordEdit->clear();
        m_newPasswordEdit->clear();
        m_confirmPasswordEdit->clear();
    });
    securityLayout->addWidget(m_changePassBtn, 0, Qt::AlignLeft);
    
    contentLayout->addWidget(securityCard);
    
    // ========== 快捷操作 ==========
    QWidget *actionsCard = new QWidget();
    actionsCard->setObjectName("ActionsCard");
    actionsCard->setStyleSheet(R"(
        QWidget#ActionsCard {
            background-color: #1e293b;
            border-radius: 16px;
            border: 1px solid #334155;
        }
    )");
    
    QVBoxLayout *actionsLayout = new QVBoxLayout(actionsCard);
    actionsLayout->setContentsMargins(25, 25, 25, 25);
    actionsLayout->setSpacing(15);
    
    QLabel *actionsTitle = new QLabel("⚡  快捷操作", actionsCard);
    actionsTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #f1f5f9;");
    actionsLayout->addWidget(actionsTitle);
    
    QFrame *line3 = new QFrame(actionsCard);
    line3->setFrameShape(QFrame::HLine);
    line3->setStyleSheet("background-color: #334155;");
    line3->setFixedHeight(1);
    actionsLayout->addWidget(line3);
    
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(15);
    
    auto createActionBtn = [](const QString& icon, const QString& text, const QString& color) {
        QPushButton *btn = new QPushButton(QString("%1  %2").arg(icon, text));
        btn->setStyleSheet(QString(R"(
            QPushButton {
                background-color: rgba(%1, 0.1);
                color: %2;
                border: 1px solid %2;
                border-radius: 10px;
                padding: 15px 20px;
                font-size: 13px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: rgba(%1, 0.2);
            }
        )").arg(color == "#3b82f6" ? "59, 130, 246" : 
               color == "#22c55e" ? "34, 197, 94" : "245, 158, 11", color));
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };
    
    QPushButton *historyBtn = createActionBtn("📜", "查看历史订单", "#3b82f6");
    QPushButton *feedbackBtn = createActionBtn("💬", "意见反馈", "#22c55e");
    QPushButton *helpBtn = createActionBtn("❓", "帮助中心", "#f59e0b");
    
    buttonsLayout->addWidget(historyBtn);
    buttonsLayout->addWidget(feedbackBtn);
    buttonsLayout->addWidget(helpBtn);
    
    actionsLayout->addLayout(buttonsLayout);
    
    connect(historyBtn, &QPushButton::clicked, this, [this]() {
        showMessage("历史订单", "请前往「我的订单」页面查看完整订单历史", true);
    });
    connect(feedbackBtn, &QPushButton::clicked, this, [this]() {
        showMessage("意见反馈", "感谢您的反馈！\n\n请发送邮件至：\nsupport@fuyao-air.com", true);
    });
    connect(helpBtn, &QPushButton::clicked, this, [this]() {
        showMessage("帮助中心", "常见问题：\n\n1. 如何取消订单？\n   在订单页面点击取消按钮\n\n2. 如何改签？\n   在订单页面点击改签按钮\n\n3. 联系客服\n   拨打：400-123-4567", true);
    });
    
    contentLayout->addWidget(actionsCard);
    contentLayout->addStretch();
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

void ProfilePage::setUserInfo(const User& user)
{
    m_currentUser = user;
    
    m_usernameEdit->setText(user.username);
    m_welcomeLabel->setText(QString("欢迎回来，%1").arg(user.username));
    
    // 根据用户名首字设置头像
    if (!user.real_name.isEmpty()) {
        m_avatarLabel->setText(user.real_name.left(1));
        m_avatarLabel->setStyleSheet(R"(
            background-color: rgba(255, 255, 255, 0.2);
            border-radius: 40px;
            font-size: 32px;
            color: white;
            font-weight: bold;
        )");
    }
    
    // 隐藏真实姓名中间字符
    QString maskedName = user.real_name;
    if (maskedName.length() == 2) {
        maskedName.replace(1, 1, "*");
    } else if (maskedName.length() >= 3) {
        maskedName.replace(1, maskedName.length() - 2, QString(maskedName.length() - 2, '*'));
    }
    m_realNameEdit->setText(maskedName);
    
    // 隐藏手机号中间四位
    QString maskedPhone = user.phone;
    if (maskedPhone.length() >= 11) {
        maskedPhone.replace(3, 4, "****");
    }
    m_phoneEdit->setText(maskedPhone);
    
    // if (m_memberSinceLabel) m_memberSinceLabel->setText("2025");
}

void ProfilePage::showMessage(const QString& title, const QString& text, bool success) {
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

void ProfilePage::updateTheme(bool isDark)
{
    m_isDarkTheme = isDark;
    
    // Colors
    QString bgColor = isDark ? "#1e293b" : "#ffffff";
    QString inputBg = isDark ? "#0f172a" : "#f8fafc";
    QString borderColor = isDark ? "#334155" : "#e2e8f0";
    QString textColor = isDark ? "#f1f5f9" : "#1e293b";
    QString subTextColor = isDark ? "#64748b" : "#64748b";
    QString scrollBarBg = isDark ? "#1e293b" : "#f1f5f9";
    QString scrollHandle = isDark ? "#475569" : "#cbd5e1";
    QString scrollHandleHover = isDark ? "#64748b" : "#94a3b8";

    // ScrollArea
    QScrollArea *scrollArea = findChild<QScrollArea*>("ProfileScrollArea");
    if (scrollArea) {
        scrollArea->setStyleSheet(QString(R"(
            QScrollArea#ProfileScrollArea { background: transparent; border: none; }
            QScrollBar:vertical {
                background: %1;
                width: 8px;
                border-radius: 4px;
            }
            QScrollBar::handle:vertical {
                background: %2;
                border-radius: 4px;
                min-height: 30px;
            }
            QScrollBar::handle:vertical:hover { background: %3; }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        )").arg(scrollBarBg, scrollHandle, scrollHandleHover));
    }

    // Cards
    QString cardStyle = QString(R"(
        QWidget#InfoCard, QWidget#SecurityCard, QWidget#ActionsCard, QWidget#StatCard {
            background-color: %1;
            border-radius: 16px;
            border: 1px solid %2;
        }
        QWidget#StatCard { border-radius: 12px; }
    )").arg(bgColor, borderColor);
    
    QList<QWidget*> cards = findChildren<QWidget*>();
    for(QWidget* w : cards) {
        QString objName = w->objectName();
        if(objName == "InfoCard" || objName == "SecurityCard" || objName == "ActionsCard" || objName == "StatCard") {
            w->setStyleSheet(cardStyle);
        }
    }
    
    // Inputs
    QString inputStyle = QString(R"(
        QLineEdit {
            background-color: %1;
            border: 2px solid %2;
            border-radius: 8px;
            padding: 10px 15px;
            font-size: 14px;
            color: %3;
        }
        QLineEdit:focus { border-color: #3b82f6; }
        QLineEdit::placeholder { color: %4; }
    )").arg(inputBg, borderColor, textColor, subTextColor);
    
    QString readOnlyStyle = QString(R"(
        QLineEdit {
            background-color: transparent;
            border: none;
            border-bottom: 1px solid %1;
            border-radius: 0;
            padding: 10px 0;
            font-size: 14px;
            color: %2;
        }
    )").arg(borderColor, textColor);
    
    if (m_isEditing) {
        m_phoneEdit->setStyleSheet(inputStyle);
    } else {
        m_phoneEdit->setStyleSheet(readOnlyStyle);
    }
    m_usernameEdit->setStyleSheet(readOnlyStyle);
    m_realNameEdit->setStyleSheet(readOnlyStyle);
    
    m_oldPasswordEdit->setStyleSheet(inputStyle);
    m_newPasswordEdit->setStyleSheet(inputStyle);
    m_confirmPasswordEdit->setStyleSheet(inputStyle);
    
    // Labels
    QList<QLabel*> labels = findChildren<QLabel*>();
    for (QLabel* label : labels) {
        QString text = label->text();
        if (text.contains("基本信息") || text.contains("安全设置") || text.contains("快捷操作")) {
            label->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; margin-bottom: 10px;").arg(textColor));
        } else if (text.contains("修改密码后")) {
             label->setStyleSheet(QString("font-size: 12px; color: %1; margin-top: 5px;").arg(subTextColor));
        } else if (text == "用户名" || text == "真实姓名" || text == "手机号") {
             label->setStyleSheet(QString("font-size: 13px; color: %1;").arg(subTextColor));
        }
    }
    
    // Separator lines
    QList<QFrame*> frames = findChildren<QFrame*>();
    for (QFrame* frame : frames) {
        if (frame->frameShape() == QFrame::HLine && frame->objectName() != "SidebarLine") {
             frame->setStyleSheet(QString("background-color: %1;").arg(borderColor));
        }
    }
}
