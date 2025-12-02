#ifndef FLIGHT_PAGE_H
#define FLIGHT_PAGE_H

#include <QWidget>

namespace Ui {
class FlightPage;
}

class FlightPage : public QWidget
{
    Q_OBJECT

public:
    explicit FlightPage(QWidget *parent = nullptr);
    ~FlightPage();

private:
    Ui::FlightPage *ui;
};

#endif // FLIGHT_PAGE_H
