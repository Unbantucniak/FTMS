#ifndef FLIGHT_CARD_H
#define FLIGHT_CARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "data_model.h"

class FlightCard : public QWidget {
    Q_OBJECT
public:
    explicit FlightCard(const Flight& flight, QWidget *parent = nullptr);
    Flight getFlight() const { return m_flight; }

signals:
    void bookRequested(const Flight& flight);

private:
    Flight m_flight;
    void setupUI();
};

#endif // FLIGHT_CARD_H