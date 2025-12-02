#ifndef PROFILE_PAGE_H
#define PROFILE_PAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include "../../common/include/data_model.h"

class ProfilePage : public QWidget
{
    Q_OBJECT
public:
    explicit ProfilePage(QWidget *parent = nullptr);
    void setUserInfo(const User& user);

signals:
    void updateUserInfo(const User& user);

private:
    QLineEdit *m_usernameEdit;
    QLineEdit *m_realNameEdit;
    QLineEdit *m_phoneEdit;
    QPushButton *m_saveBtn;
};

#endif // PROFILE_PAGE_H
