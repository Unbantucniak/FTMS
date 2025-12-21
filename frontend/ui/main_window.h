#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QLabel>
#include <QListWidget>
#include <QCompleter>
#include <QGraphicsDropShadowEffect>
#include "orders_page.h"
#include "profile_page.h"
#include "chat_dialog.h"

struct Flight;

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
    void onOccupiedSeatsReceived(const QStringList& seats);
    void onCitiesReceived(const QStringList& cities);
    void performSearch();

private:
    void setupUI();
    void setupSidebar();
    void setupFlightPage();
    void setupConnections();
    void applyTheme();
    QGraphicsDropShadowEffect* createShadow(QColor color, int blur, int offsetY);
    
    QString m_username;
    bool m_isDarkTheme;
    
    QWidget *m_centralWidget;
    QWidget *m_sidebar;
    QStackedWidget *m_stack;
    
    ChatWidget *m_chatWidget;
    
    QPushButton *m_flightBtn;
    QPushButton *m_ordersBtn;
    QPushButton *m_profileBtn;
    QPushButton *m_chatBtn;
    QPushButton *m_themeBtn;
    QPushButton *m_logoutBtn;
    
    QWidget *m_flightPage;
    QLabel *m_mainTitleLabel;
    QComboBox *m_departureCombo;
    QComboBox *m_destinationCombo;
    QDateEdit *m_dateEdit;
    QCheckBox *m_dateLimitCheckBox; 
    QPushButton *m_searchBtn;
    QPushButton *m_swapBtn;
    QScrollArea *m_flightScrollArea;
    QVBoxLayout *m_flightLayout;
    QLabel *m_resultCountLabel;
    
    OrdersPage *m_ordersPage;
    ProfilePage *m_profilePage;
    
    QString m_changingOrderId;
    QString m_pendingFlightId;
    int m_pendingFlightSeats;
    
    QStringList m_cities;
};

#endif // MAINWINDOW_H
