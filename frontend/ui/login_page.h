#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage() = default;

private:
    void setupUI();
    void setupConnections();
    void applyStyles();
    QGraphicsDropShadowEffect* createShadow(QColor color, int blur, int offsetY);

    // UI Components
    QWidget *m_loginCard;
    QLabel *m_logoLabel;
    QLabel *m_titleLabel;
    QLabel *m_subtitleLabel;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_registerBtn;
    QLabel *m_copyrightLabel;
};

#endif // LOGIN_PAGE_H
