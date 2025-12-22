#include "orders_page.h"
#include "order_card.h"
#include <QPushButton>
#include <QMessageBox>

OrdersPage::OrdersPage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet("QScrollArea { background: transparent; }");
    
    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    m_layout = new QVBoxLayout(scrollContent);
    m_layout->setContentsMargins(20, 20, 20, 20);
    m_layout->setSpacing(15);
    m_layout->addStretch();
    
    m_scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(m_scrollArea);
}

void OrdersPage::setOrders(const QList<Order>& orders)
{
    QLayoutItem *item;
    while ((item = m_layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    
    for(const Order& order : orders) {
        OrderCard *card = new OrderCard(order);
        connect(card, &OrderCard::cancelRequested, this, [this](const QString& orderId){
             QMessageBox::StandardButton reply;
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("取消订单");
            msgBox.setText("您确定要取消该订单吗？");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setButtonText(QMessageBox::Yes, "是");
            msgBox.setButtonText(QMessageBox::No, "否");
            reply = (QMessageBox::StandardButton)msgBox.exec();

            if (reply == QMessageBox::Yes) {
                emit cancelOrder(orderId);
            }
        });
        connect(card, &OrderCard::changeRequested, this, &OrdersPage::changeOrder);
        
        m_layout->addWidget(card);
    }
    m_layout->addStretch();
}
