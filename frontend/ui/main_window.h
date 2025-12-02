#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QScrollArea>
#include "orders_page.h"
#include "profile_page.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setUsername(const QString& username);

private slots:
    void switchTheme();
    void navigateTo(int index);

private:
    Ui::MainWindow *ui;
    QString m_username;
    
    QStackedWidget *m_stack;
    OrdersPage *m_ordersPage;
    ProfilePage *m_profilePage;
    bool m_isDarkTheme;
    
    QScrollArea *m_flightScrollArea;
    QWidget *m_flightContainer;
    QVBoxLayout *m_flightLayout;

    QString m_changingOrderId;

    void setupSidebar();
    void applyTheme();
};
#endif // MAINWINDOW_H
