#include "flight_page.h"
#include "ui_flight_page.h"

FlightPage::FlightPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightPage)
{
    ui->setupUi(this);
}

FlightPage::~FlightPage()
{
    delete ui;
}
