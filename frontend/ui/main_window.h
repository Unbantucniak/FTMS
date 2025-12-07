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
#include <QLabel>
#include <QListWidget>
#include <QCompleter>
#include <QGraphicsDropShadowEffect>
#include "orders_page.h"
#include "profile_page.h"

struct Flight;  // Forward declaration

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
    
    // Main layout components
    QWidget *m_centralWidget;
    QWidget *m_sidebar;
    QStackedWidget *m_stack;
    
    // Sidebar buttons
    QPushButton *m_flightBtn;
    QPushButton *m_ordersBtn;
    QPushButton *m_profileBtn;
    QPushButton *m_themeBtn;
    QPushButton *m_logoutBtn;
    
    // Flight search page components
    QWidget *m_flightPage;
    QLabel *m_mainTitleLabel;
    QComboBox *m_departureCombo;
    QComboBox *m_destinationCombo;
    QDateEdit *m_dateEdit;
    QPushButton *m_searchBtn;
    QPushButton *m_swapBtn;
    QScrollArea *m_flightScrollArea;
    QVBoxLayout *m_flightLayout;
    QLabel *m_resultCountLabel;
    
    // Other pages
    OrdersPage *m_ordersPage;
    ProfilePage *m_profilePage;
    
    // Booking state
    QString m_changingOrderId;
    QString m_pendingFlightId;
    int m_pendingFlightSeats;
    
    // Cities data
    QStringList m_cities;
};

#endif // MAINWINDOW_H
