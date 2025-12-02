#include <QApplication>
#include "ui/login_page.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    LoginPage w;
    w.show();
    return a.exec();
}
