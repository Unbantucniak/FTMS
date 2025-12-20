#include "chat_dialog.h"
#include "../network/tcp_client.h"
#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QGuiApplication>
#include <QPalette>

ChatWidget::ChatWidget(QWidget *parent) : QWidget(parent)
{
    buildUI();
    
    // 检测系统主题并应用
    m_isDark = detectSystemDark();
    applyTheme(m_isDark);
    
    // 连接网络信号
    connect(TcpClient::getInstance(), &TcpClient::aiChatResult, this, &ChatWidget::onAIResponse);
    
    addMessage("欢迎使用扶摇航班票务系统！AI 出行顾问已上线，专注解答各类出行相关疑问～ 航班查询功能请您自行通过系统查询入口操作，有其他出行问题随时告诉我！", false);
}

void ChatWidget::buildUI()
{
    setObjectName("ChatWidgetRoot");
    
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    
    // ========== 顶部标题栏 ==========
    auto *topBar = new QFrame(this);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(60);
    
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(24, 0, 24, 0);
    
    m_titleLabel = new QLabel("出行助手", this);
    m_titleLabel->setObjectName("TitleLabel");
    
    topLayout->addWidget(m_titleLabel);
    topLayout->addStretch();
    
    rootLayout->addWidget(topBar);
    
    // ========== 中部消息区域 ==========
    m_messagesContainer = new QWidget(this);
    m_messagesContainer->setObjectName("MessagesContainer");
    
    m_messagesLayout = new QVBoxLayout(m_messagesContainer);
    m_messagesLayout->setContentsMargins(24, 20, 24, 20);
    m_messagesLayout->setSpacing(16);
    m_messagesLayout->addStretch(); // 消息从底部开始堆叠
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName("ScrollArea");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_messagesContainer);
    
    rootLayout->addWidget(m_scrollArea, 1);
    
    // ========== 底部输入区域 ==========
    m_inputFrame = new QFrame(this);
    m_inputFrame->setObjectName("InputFrame");
    
    auto *inputLayout = new QHBoxLayout(m_inputFrame);
    inputLayout->setContentsMargins(24, 16, 24, 16);
    inputLayout->setSpacing(12);
    
    m_inputEdit = new QTextEdit(this);
    m_inputEdit->setObjectName("InputEdit");
    m_inputEdit->setPlaceholderText("输入消息，Ctrl+Enter 发送...");
    m_inputEdit->setFixedHeight(80);
    m_inputEdit->installEventFilter(this);  // 安装事件过滤器以捕获快捷键
    
    m_sendBtn = new QPushButton("发送", this);
    m_sendBtn->setObjectName("SendBtn");
    m_sendBtn->setCursor(Qt::PointingHandCursor);
    m_sendBtn->setFixedSize(80, 40);
    connect(m_sendBtn, &QPushButton::clicked, this, &ChatWidget::onSendClicked);
    
    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName("StatusLabel");
    m_statusLabel->hide();
    
    // 创建思考状态指示器（更醒目的动画效果）
    m_thinkingWidget = new QWidget(this);
    m_thinkingWidget->setObjectName("ThinkingWidget");
    m_thinkingWidget->setFixedHeight(50);
    m_thinkingWidget->hide();
    
    auto *thinkingLayout = new QHBoxLayout(m_thinkingWidget);
    thinkingLayout->setContentsMargins(24, 8, 24, 8);
    thinkingLayout->setSpacing(12);
    
    m_thinkingLabel = new QLabel("🤔 AI 正在思考中", this);
    m_thinkingLabel->setObjectName("ThinkingLabel");
    
    m_thinkingDots = new QLabel("", this);
    m_thinkingDots->setObjectName("ThinkingDots");
    m_thinkingDots->setFixedWidth(30);
    
    thinkingLayout->addWidget(m_thinkingLabel);
    thinkingLayout->addWidget(m_thinkingDots);
    thinkingLayout->addStretch();
    
    // 创建动画定时器
    m_thinkingTimer = new QTimer(this);
    connect(m_thinkingTimer, &QTimer::timeout, this, &ChatWidget::updateThinkingAnimation);
    
    inputLayout->addWidget(m_inputEdit, 1);
    inputLayout->addWidget(m_sendBtn, 0, Qt::AlignBottom);
    
    rootLayout->addWidget(m_thinkingWidget);  // 思考状态显示在输入框上方
    rootLayout->addWidget(m_inputFrame);
}

void ChatWidget::setUsername(const QString &username)
{
    m_username = username;
}

void ChatWidget::setTheme(bool isDark)
{
    m_isDark = isDark;
    applyTheme(isDark);
}

bool ChatWidget::detectSystemDark() const
{
    QColor windowColor = QGuiApplication::palette().color(QPalette::Window);
    return windowColor.lightness() < 128;
}

ChatTheme ChatWidget::getTheme(bool isDark) const
{
    if (isDark) {
        return ChatTheme{
            "#0f172a",      // bg
            "#1e293b",      // surface
            "#f1f5f9",      // text
            "#94a3b8",      // subText
            "#334155",      // border
            "#3b82f6",      // primary
            "#2563eb",      // primaryDark
            "#3b82f6",      // bubbleUser
            "#2563eb",      // bubbleUserEnd
            "#1e293b",      // bubbleAI
            "#334155"       // bubbleAIBorder
        };
    } else {
        return ChatTheme{
            "#ffffff",      // bg
            "#f8fafc",      // surface
            "#0f172a",      // text
            "#64748b",      // subText
            "#e2e8f0",      // border
            "#3b82f6",      // primary
            "#2563eb",      // primaryDark
            "#3b82f6",      // bubbleUser
            "#2563eb",      // bubbleUserEnd
            "#f1f5f9",      // bubbleAI
            "#e2e8f0"       // bubbleAIBorder
        };
    }
}

void ChatWidget::applyTheme(bool isDark)
{
    m_currentTheme = getTheme(isDark);
    const ChatTheme &t = m_currentTheme;
    
    // 主背景
    setStyleSheet(QString(
        "QWidget#ChatWidgetRoot { background: %1; }"
    ).arg(t.bg));
    
    // 顶部标题栏
    if (auto *topBar = findChild<QFrame*>("TopBar")) {
        topBar->setStyleSheet(QString(
            "QFrame#TopBar {"
            "   background: %1;"
            "   border-bottom: 1px solid %2;"
            "}"
        ).arg(t.surface, t.border));
    }
    
    m_titleLabel->setStyleSheet(QString(
        "QLabel#TitleLabel {"
        "   font-size: 18px;"
        "   font-weight: 700;"
        "   color: %1;"
        "   font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
        "}"
    ).arg(t.text));
    
    // 消息区域
    m_messagesContainer->setStyleSheet(QString(
        "QWidget#MessagesContainer { background: transparent; }"
    ));
    
    m_scrollArea->setStyleSheet(QString(
        "QScrollArea#ScrollArea {"
        "   background: %1;"
        "   border: none;"
        "}"
        "QScrollBar:vertical {"
        "   background: transparent;"
        "   width: 8px;"
        "   margin: 0;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: %2;"
        "   border-radius: 4px;"
        "   min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: %3;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0;"
        "}"
    ).arg(t.bg, t.border, t.subText));
    
    // 底部输入区域
    m_inputFrame->setStyleSheet(QString(
        "QFrame#InputFrame {"
        "   background: %1;"
        "   border-top: 1px solid %2;"
        "}"
    ).arg(t.surface, t.border));
    
    m_inputEdit->setStyleSheet(QString(
        "QTextEdit#InputEdit {"
        "   background: %1;"
        "   border: 1px solid %2;"
        "   border-radius: 12px;"
        "   padding: 12px 16px;"
        "   font-size: 14px;"
        "   color: %3;"
        "   font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
        "}"
        "QTextEdit#InputEdit:focus {"
        "   border-color: %4;"
        "}"
    ).arg(t.bg, t.border, t.text, t.primary));
    
    m_sendBtn->setStyleSheet(QString(
        "QPushButton#SendBtn {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %1, stop:1 %2);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   font-size: 14px;"
        "   font-weight: 600;"
        "   font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
        "}"
        "QPushButton#SendBtn:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %2, stop:1 %1);"
        "}"
        "QPushButton#SendBtn:pressed {"
        "   background: %2;"
        "}"
        "QPushButton#SendBtn:disabled {"
        "   background: %3;"
        "   color: %4;"
        "}"
    ).arg(t.primary, t.primaryDark, t.border, t.subText));
    
    m_statusLabel->setStyleSheet(QString(
        "QLabel#StatusLabel {"
        "   color: %1;"
        "   font-size: 12px;"
        "   font-style: italic;"
        "}"
    ).arg(t.subText));
    
    // 思考状态指示器样式
    m_thinkingWidget->setStyleSheet(QString(
        "QWidget#ThinkingWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %1, stop:1 %2);"
        "   border-top: 2px solid %3;"
        "   border-bottom: 1px solid %4;"
        "}"
    ).arg(isDark ? "#1e3a5f" : "#e0f2fe", isDark ? "#172554" : "#bae6fd", t.primary, t.border));
    
    m_thinkingLabel->setStyleSheet(QString(
        "QLabel#ThinkingLabel {"
        "   color: %1;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "   font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
        "   background: transparent;"
        "}"
    ).arg(t.primary));
    
    m_thinkingDots->setStyleSheet(QString(
        "QLabel#ThinkingDots {"
        "   color: %1;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   background: transparent;"
        "}"
    ).arg(t.primary));
    
    m_isDark = isDark;
    
    // 更新已有气泡样式
    updateExistingBubbles();
}

void ChatWidget::updateExistingBubbles()
{
    const ChatTheme &t = m_currentTheme;
    
    // 更新用户消息气泡
    for (auto *bubble : m_messagesContainer->findChildren<QFrame*>("UserBubble")) {
        bubble->setStyleSheet(QString(
            "QFrame#UserBubble {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %1, stop:1 %2);"
            "   border-radius: 18px;"
            "   border-bottom-right-radius: 6px;"
            "}"
        ).arg(t.bubbleUser, t.bubbleUserEnd));
    }
    
    // 更新 AI 消息气泡
    for (auto *bubble : m_messagesContainer->findChildren<QFrame*>("AIBubble")) {
        bubble->setStyleSheet(QString(
            "QFrame#AIBubble {"
            "   background: %1;"
            "   border: 1px solid %2;"
            "   border-radius: 18px;"
            "   border-bottom-left-radius: 6px;"
            "}"
        ).arg(t.bubbleAI, t.bubbleAIBorder));
        
        // 更新气泡内的文本颜色
        for (auto *label : bubble->findChildren<QLabel*>()) {
            label->setStyleSheet(QString(
                "QLabel {"
                "   color: %1;"
                "   font-size: 14px;"
                "   line-height: 1.5;"
                "   font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
                "   background: transparent;"
                "}"
            ).arg(t.text));
        }
    }
}

void ChatWidget::addMessage(const QString &text, bool fromUser)
{
    const ChatTheme &t = m_currentTheme;
    
    // 创建消息行容器
    auto *row = new QWidget();
    row->setObjectName(fromUser ? "UserMessageRow" : "AIMessageRow");
    
    auto *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(12);
    
    // 创建气泡容器（AI消息更宽以适配内容）
    auto *bubble = new QFrame();
    bubble->setObjectName(fromUser ? "UserBubble" : "AIBubble");
    int maxBubbleWidth = fromUser ? 500 : qMax(600, static_cast<int>(this->width() * 0.75));
    bubble->setMaximumWidth(maxBubbleWidth);
    
    auto *bubbleLayout = new QVBoxLayout(bubble);
    bubbleLayout->setContentsMargins(16, 12, 16, 12);
    bubbleLayout->setSpacing(0);
    
    // 创建消息文本标签
    auto *msgLabel = new QLabel();
    msgLabel->setWordWrap(true);               // 自动换行
    msgLabel->setTextFormat(Qt::PlainText);    // 纯文本格式
    msgLabel->setText(text);
    msgLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);  // 允许鼠标选择文本
    
    bubbleLayout->addWidget(msgLabel);
    
    // 根据消息来源设置不同样式
    QString bubbleStyle;
    QString textStyle;
    
    if (fromUser) {
        // 用户消息：右对齐，蓝色渐变背景
        bubbleStyle = QString(
            "QFrame#UserBubble {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %1, stop:1 %2);"
            "   border-radius: 18px;"
            "   border-bottom-right-radius: 6px;"
            "}"
        ).arg(t.bubbleUser, t.bubbleUserEnd);
        
        textStyle = QString(
            "QLabel {"
            "   color: white;"
            "   font-size: 14px;"
            "   line-height: 1.5;"
            "   font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "   background: transparent;"
            "}"
        );
        
        rowLayout->addStretch();  // 左侧弹性空间，使气泡靠右
        rowLayout->addWidget(bubble);
    } else {
        // AI消息：左对齐，中性背景色
        bubbleStyle = QString(
            "QFrame#AIBubble {"
            "   background: %1;"
            "   border: 1px solid %2;"
            "   border-radius: 18px;"
            "   border-bottom-left-radius: 6px;"
            "}"
        ).arg(t.bubbleAI, t.bubbleAIBorder);
        
        textStyle = QString(
            "QLabel {"
            "   color: %1;"
            "   font-size: 14px;"
            "   line-height: 1.5;"
            "   font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "   background: transparent;"
            "}"
        ).arg(t.text);
        
        rowLayout->addWidget(bubble);
        rowLayout->addStretch();  // 右侧弹性空间，使气泡靠左
    }
    
    // 应用样式
    bubble->setStyleSheet(bubbleStyle);
    msgLabel->setStyleSheet(textStyle);
    
    // 插入到消息布局（在底部 stretch 之前，保持消息从底部向上堆叠）
    int insertIndex = m_messagesLayout->count() - 1;
    m_messagesLayout->insertWidget(insertIndex, row);
    
    // 滚动到底部显示最新消息
    scrollToBottom();
}

void ChatWidget::scrollToBottom()
{
    // 延迟执行，确保布局更新完成后再滚动
    QTimer::singleShot(10, this, [this]() {
        QScrollBar *vbar = m_scrollArea->verticalScrollBar();
        vbar->setValue(vbar->maximum());
    });
}

void ChatWidget::onSendClicked()
{
    QString text = m_inputEdit->toPlainText().trimmed();
    if (text.isEmpty()) return;
    
    // 添加用户消息到界面
    addMessage(text, true);
    m_inputEdit->clear();
    
    // 禁用输入控件，显示等待状态
    m_sendBtn->setEnabled(false);
    m_inputEdit->setEnabled(false);
    showThinkingIndicator(true);
    
    // 发送消息到服务器
    TcpClient::getInstance()->sendAIChatMessage(m_username, text);
}

void ChatWidget::onAIResponse(bool success, const QString &response)
{
    // 恢复输入控件状态
    m_sendBtn->setEnabled(true);
    m_inputEdit->setEnabled(true);
    m_inputEdit->setFocus();
    showThinkingIndicator(false);
    
    // 显示AI回复或错误信息
    if (success) {
        addMessage(response, false);
    } else {
        addMessage("抱歉，服务暂时不可用：" + response, false);
    }
}

bool ChatWidget::eventFilter(QObject *obj, QEvent *event)
{
    // 捕获输入框的键盘事件，实现 Ctrl+Enter 发送
    if (obj == m_inputEdit && event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent*>(event);
        if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) 
            && (keyEvent->modifiers() & Qt::ControlModifier)) {
            onSendClicked();
            return true;  // 事件已处理，不再传递
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool ChatWidget::event(QEvent *e)
{
    // 监听系统主题变化，自动切换明暗主题
    if (e->type() == QEvent::ApplicationPaletteChange) {
        bool sysDark = detectSystemDark();
        if (sysDark != m_isDark) {
            m_isDark = sysDark;
            applyTheme(m_isDark);
        }
    }
    return QWidget::event(e);
}

void ChatWidget::showThinkingIndicator(bool show)
{
    if (show) {
        m_dotCount = 0;
        m_thinkingDots->setText("");
        m_thinkingWidget->show();
        m_thinkingTimer->start(400);  // 每 400ms 更新一次动画
    } else {
        m_thinkingTimer->stop();
        m_thinkingWidget->hide();
    }
}

void ChatWidget::updateThinkingAnimation()
{
    m_dotCount = (m_dotCount + 1) % 4;
    QString dots;
    for (int i = 0; i < m_dotCount; ++i) {
        dots += "●";
    }
    m_thinkingDots->setText(dots);
}