#ifndef PROFILE_PAGE_H
#define PROFILE_PAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include "../../common/include/data_model.h"

class ProfilePage : public QWidget
{
    Q_OBJECT
public:
    explicit ProfilePage(QWidget *parent = nullptr);
    void setUserInfo(const User& user);
    void updateTheme(bool isDark);

signals:
    void updateUserInfo(const User& user);
    void changePassword(const QString& oldPass, const QString& newPass);

private:
    void setupUI();
    void showMessage(const QString& title, const QString& text, bool success);
    
    // 用户信息
    QLabel *m_avatarLabel;
    QLabel *m_welcomeLabel;
    QLabel *m_memberLevelLabel;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_realNameEdit;
    QLineEdit *m_phoneEdit;
    QPushButton *m_saveBtn;
    QPushButton *m_editBtn;
    
    // 统计信息
    // QLabel *m_memberSinceLabel;
    
    // 安全设置
    QWidget *m_passwordFieldsWidget;
    QLineEdit *m_oldPasswordEdit;
    QLineEdit *m_newPasswordEdit;
    QLineEdit *m_confirmPasswordEdit;
    QPushButton *m_changePassBtn;
    QPushButton *m_submitPassBtn;
    QPushButton *m_cancelPassBtn;
    
    bool m_isEditing = false;
    bool m_isPasswordExpanded = false;
    bool m_isDarkTheme = true;
    User m_currentUser;
};

#endif // PROFILE_PAGE_H
