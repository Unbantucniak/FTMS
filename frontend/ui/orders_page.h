#ifndef ORDERS_PAGE_H
#define ORDERS_PAGE_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include "../../common/include/data_model.h"

class OrdersPage : public QWidget
{
    Q_OBJECT
public:
    explicit OrdersPage(QWidget *parent = nullptr);
    void setOrders(const QList<Order>& orders);

signals:
    void cancelOrder(const QString& orderId);
    void changeOrder(const QString& orderId);

private:
    QScrollArea *m_scrollArea;
    QVBoxLayout *m_layout;
};

#endif // ORDERS_PAGE_H
