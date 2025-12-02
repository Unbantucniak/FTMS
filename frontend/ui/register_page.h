#ifndef REGISTER_PAGE_H
#define REGISTER_PAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>

class RegisterPage : public QWidget {
    Q_OBJECT
public:
    explicit RegisterPage(QWidget *parent = nullptr);

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLineEdit *realNameEdit;
    QLineEdit *phoneEdit;
    QLabel *usernameStatusLabel;
    QLabel *passwordStatusLabel;
    QPushButton *registerBtn;
    QPushButton *backBtn;

    void setupUI();
};

#endif // REGISTER_PAGE_H