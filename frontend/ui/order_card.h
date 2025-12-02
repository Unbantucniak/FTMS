#ifndef ORDER_CARD_H
#define ORDER_CARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "data_model.h"

class OrderCard : public QWidget {
    Q_OBJECT
public:
    explicit OrderCard(const Order& order, QWidget *parent = nullptr);
    Order getOrder() const { return m_order; }

signals:
    void cancelRequested(const QString& orderId);
    void changeRequested(const QString& orderId);

private:
    Order m_order;
    void setupUI();
};

#endif // ORDER_CARD_H