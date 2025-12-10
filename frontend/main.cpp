#include <QApplication>
#include <QIcon>
#include "ui/login_page.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序图标
    a.setWindowIcon(QIcon(":/resources/app_icon.png"));
    
    LoginPage w;
    w.show();
    return a.exec();
}
