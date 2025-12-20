#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTextEdit>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QFrame>
#include <QEvent>
#include <QScrollBar>
#include <QTimer>

// 主题配色结构
struct ChatTheme {
    QString bg;             // 主背景色
    QString surface;        // 卡片/气泡背景色
    QString text;           // 主文本色
    QString subText;        // 次要文本色
    QString border;         // 边框色
    QString primary;        // 主色调
    QString primaryDark;    // 主色调深色
    QString bubbleUser;     // 用户气泡背景
    QString bubbleUserEnd;  // 用户气泡渐变终点
    QString bubbleAI;       // AI气泡背景
    QString bubbleAIBorder; // AI气泡边框
};

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    void setUsername(const QString &username);
    void setTheme(bool isDark);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    bool event(QEvent *e) override;

private slots:
    void onSendClicked();
    void onAIResponse(bool success, const QString &response);

private:
    void buildUI();
    void applyTheme(bool isDark);
    void updateExistingBubbles();
    bool detectSystemDark() const;
    void addMessage(const QString &text, bool fromUser);
    void scrollToBottom();
    ChatTheme getTheme(bool isDark) const;
    void showThinkingIndicator(bool show);
    void updateThinkingAnimation();

    // UI 组件
    QLabel *m_titleLabel{};
    QScrollArea *m_scrollArea{};
    QWidget *m_messagesContainer{};
    QVBoxLayout *m_messagesLayout{};
    QTextEdit *m_inputEdit{};
    QPushButton *m_sendBtn{};
    QLabel *m_statusLabel{};
    QFrame *m_inputFrame{};
    
    // 思考状态指示器组件
    QWidget *m_thinkingWidget{};
    QLabel *m_thinkingLabel{};
    QLabel *m_thinkingDots{};
    QTimer *m_thinkingTimer{};
    int m_dotCount{0};

    // 状态
    bool m_isDark{false};
    QString m_username;
    ChatTheme m_currentTheme;
};

#endif // CHAT_WIDGET_H
