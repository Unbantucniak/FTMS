#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QString>
#include <QApplication>

// 全局主题样式定义
namespace Theme {

// 深色主题
inline const QString DARK_THEME = R"(
    QWidget { background-color: #2b2b2b; color: #e0e0e0; font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; font-size: 14px; }
    
    QLabel#MainTitle {
        font-size: 26px;
        font-weight: 600;
        letter-spacing: 3px;
        color: #f5f5f5;
    }

    QWidget#FilterPanel {
        background-color: #353535;
        border: 1px solid #444;
        border-radius: 16px;
    }

    /* 输入框 */
    QLineEdit, QDateEdit, QComboBox { 
        background-color: #3a3a3a; 
        border: 1px solid #555; 
        border-radius: 6px; 
        padding: 8px 12px; 
        color: #fff; 
        selection-background-color: #0078d7;
    }
    QLineEdit:focus, QDateEdit:focus, QComboBox:focus { border: 1px solid #0078d7; }
    
    /* 通用按钮 */
    QPushButton { 
        background-color: #3a3a3a; 
        color: #e0e0e0; 
        border: 1px solid #555; 
        border-radius: 6px; 
        padding: 8px 16px; 
        font-weight: bold;
    }
    QPushButton:hover { background-color: #444; }
    QPushButton:pressed { background-color: #222; }
    QPushButton:disabled { background-color: #444; color: #888; border: none; }

    /* 主要按钮 */
    QPushButton#PrimaryBtn {
        background-color: #0078d7;
        color: white;
        border: none;
    }
    QPushButton#PrimaryBtn:hover { background-color: #1084e3; }
    
    /* 成功按钮 */
    QPushButton#SuccessBtn {
        background-color: #28a745;
        color: white;
        border: none;
    }
    QPushButton#SuccessBtn:hover { background-color: #218838; }

    /* 弹窗样式 */
    QMessageBox { background-color: #2b2b2b; border: 1px solid #444; }
    QMessageBox QLabel { color: #e0e0e0; }
    QMessageBox QPushButton {
        background-color: #0078d7;
        color: white;
        border: none;
    }
    QMessageBox QPushButton:hover { background-color: #1084e3; }

    /* 表格 */
    QTableWidget { 
        background-color: #3a3a3a; 
        border: 1px solid #555; 
        gridline-color: #444; 
        selection-background-color: #0078d7; 
        selection-color: white;
        alternate-background-color: #333;
    }
    QHeaderView::section { 
        background-color: #2d2d2d; 
        color: #e0e0e0; 
        padding: 8px; 
        border: none; 
        border-bottom: 2px solid #0078d7; 
        font-weight: bold;
    }
    QTableCornerButton::section { background-color: #2d2d2d; border: none; }

    /* 滚动条 */
    QScrollBar:vertical {
        border: none;
        background: #2b2b2b;
        width: 10px;
        margin: 0px;
    }
    QScrollBar::handle:vertical {
        background: #555;
        min-height: 20px;
        border-radius: 5px;
    }
    QScrollBar::handle:vertical:hover { background: #666; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }

    /* 侧边栏特定样式 */
    QWidget#Sidebar { background-color: #1e1e1e; border-right: 1px solid #333; }
    QPushButton#SidebarBtn { 
        background-color: transparent; 
        text-align: left; 
        padding: 15px 20px; 
        border-radius: 0; 
        border: none;
        font-size: 15px; 
        color: #aaa;
        border-left: 4px solid transparent;
    }
    QPushButton#SidebarBtn:hover { background-color: #252525; color: #fff; }
    QPushButton#SidebarBtn:checked { 
        background-color: #2d2d2d; 
        color: #0078d7; 
        border-left: 4px solid #0078d7;
        font-weight: bold;
    }

    /* Cards */
    QWidget#FlightCard, QWidget#OrderCard {
        background-color: transparent;
        border: none;
        padding: 4px 2px;
        margin-bottom: 26px;
    }
    QWidget#FlightCard QWidget#CardBody,
    QWidget#OrderCard QWidget#CardBody {
        background-color: #2f323a;
        border: 1px solid #3f444f;
        border-top-left-radius: 18px;
        border-top-right-radius: 18px;
        border-bottom: none;
        padding-top: 6px;
    }
    QWidget#FlightCard QWidget#InfoContainer,
    QWidget#OrderCard QWidget#InfoContainer {
        background-color: #23252b;
        border: 1px solid #3f444f;
        border-bottom-left-radius: 18px;
        border-bottom-right-radius: 18px;
        border-top: 1px solid #4a5060;
    }
    QWidget#FlightCard:hover QWidget#CardBody,
    QWidget#OrderCard:hover QWidget#CardBody {
        border-color: #6a8dff;
        background-color: #353946;
    }
    QWidget#FlightCard:hover QWidget#InfoContainer,
    QWidget#OrderCard:hover QWidget#InfoContainer {
        border-color: #6a8dff;
        border-top-color: #7a9cfe;
    }
    FlightCard QLabel, OrderCard QLabel { color: #f3f5f9; }
    QLabel#FlightId { font-size: 17px; font-weight: 600; color: #f8fbff; }
    QLabel#TimeLabel { font-size: 28px; font-weight: 600; color: #6aa8ff; }
    QLabel#CityLabel { font-size: 16px; font-weight: 500; color: #f3f5f9; }
    QLabel#AirportLabel { font-size: 13px; color: #a6adc8; }
    QLabel#PriceLabel { font-size: 24px; font-weight: 600; color: #ffb878; }
    QLabel#DurationLabel { font-size: 13px; color: #95a0be; }
    QLabel#SeatsLabel { font-size: 13px; color: #cdd3ec; }
    QLabel#ArrowLabel { color: #717da3; font-weight: bold; }
    
    /* 操作按钮样式 (统一结构，不同配色) */
    QPushButton#BookBtn,
    QPushButton#ChangeBtn,
    QPushButton#CancelBtn {
        border-radius: 18px;
        padding: 8px 28px;
        font-weight: 600;
        min-width: 120px;
        border-width: 1px;
    }
    QPushButton#BookBtn {
        background-color: rgba(90, 143, 255, 0.15);
        color: #a5c4ff;
        border-color: #7fa7ff;
    }
    QPushButton#BookBtn:hover {
        background-color: #4b7bec;
        color: #0e1833;
        border-color: #4b7bec;
    }
    QPushButton#ChangeBtn {
        background-color: rgba(61, 214, 190, 0.15);
        color: #7de5d4;
        border-color: #46cdb8;
    }
    QPushButton#ChangeBtn:hover {
        background-color: #2dd4bf;
        color: #082027;
        border-color: #2dd4bf;
    }
    QPushButton#CancelBtn {
        background-color: rgba(255, 107, 107, 0.15);
        color: #ff9c9c;
        border-color: #ff9c9c;
    }
    QPushButton#CancelBtn:hover { 
        background-color: #ff6b6b; 
        color: #1b0c0c;
        border-color: #ff6b6b;
    }

    /* 查询按钮 */
    QPushButton#queryButton {
        background-color: #0078d7;
        color: white;
        border: none;
        min-width: 100px;
        font-size: 15px;
        padding: 10px 22px;
        border-radius: 8px;
    }
    QPushButton#queryButton:hover { background-color: #1084e3; }

    QWidget#InfoContainer {
        background-color: #23252b;
        border-bottom-left-radius: 18px;
        border-bottom-right-radius: 18px;
        border-top: 1px solid #4a5060;
    }

    /* 对话框样式 */
    QDialog {
        background-color: #2b2b2b;
    }
)";

// 浅色主题
inline const QString LIGHT_THEME = R"(
    QWidget { background-color: #f5f7fa; color: #333; font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; font-size: 14px; }
    
    QLabel#MainTitle {
        font-size: 26px;
        font-weight: 600;
        letter-spacing: 2px;
        color: #1f2c3d;
    }

    QWidget#FilterPanel {
        background-color: #ffffff;
        border: 1px solid #e3e8f0;
        border-radius: 16px;
    }
    
    /* 输入框 */
    QLineEdit, QDateEdit, QComboBox { 
        background-color: #fff; 
        border: 1px solid #dcdfe6; 
        border-radius: 6px; 
        padding: 8px 12px; 
        color: #333; 
        selection-background-color: #0078d7;
    }
    QLineEdit:focus, QDateEdit:focus, QComboBox:focus { border: 1px solid #0078d7; }

    /* 通用按钮 */
    QPushButton { 
        background-color: #fff; 
        color: #606266; 
        border: 1px solid #dcdfe6; 
        border-radius: 6px; 
        padding: 8px 16px; 
        font-weight: bold;
    }
    QPushButton:hover { background-color: #ecf5ff; color: #409eff; border-color: #c6e2ff; }
    QPushButton:pressed { background-color: #ecf5ff; color: #3a8ee6; border-color: #3a8ee6; }
    QPushButton:disabled { background-color: #e0e0e0; color: #aaa; border-color: #ddd; }

    /* 主要按钮 */
    QPushButton#PrimaryBtn {
        background-color: #0078d7;
        color: white;
        border: none;
    }
    QPushButton#PrimaryBtn:hover { background-color: #1084e3; }
    
    /* 成功按钮 */
    QPushButton#SuccessBtn {
        background-color: #28a745;
        color: white;
        border: none;
    }
    QPushButton#SuccessBtn:hover { background-color: #218838; }

    /* 弹窗样式 */
    QMessageBox { background-color: #fff; }
    QMessageBox QLabel { color: #333; }
    QMessageBox QPushButton {
        background-color: #0078d7;
        color: white;
        border: none;
    }
    QMessageBox QPushButton:hover { background-color: #1084e3; }

    /* 表格 */
    QTableWidget { 
        background-color: #fff; 
        border: 1px solid #e0e0e0; 
        gridline-color: #f0f0f0; 
        selection-background-color: #e6f2ff; 
        selection-color: #0078d7;
        alternate-background-color: #fafafa;
    }
    QHeaderView::section { 
        background-color: #fff; 
        color: #333; 
        padding: 8px; 
        border: none; 
        border-bottom: 2px solid #0078d7; 
        font-weight: bold;
    }
    QTableCornerButton::section { background-color: #fff; border: none; }

    /* 滚动条 */
    QScrollBar:vertical {
        border: none;
        background: #f5f7fa;
        width: 10px;
        margin: 0px;
    }
    QScrollBar::handle:vertical {
        background: #ccc;
        min-height: 20px;
        border-radius: 5px;
    }
    QScrollBar::handle:vertical:hover { background: #bbb; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }

    /* 侧边栏特定样式 */
    QWidget#Sidebar { background-color: #ffffff; border-right: 1px solid #e0e0e0; }
    QPushButton#SidebarBtn { 
        background-color: transparent; 
        text-align: left; 
        padding: 15px 20px; 
        border-radius: 0; 
        border: none;
        color: #666; 
        font-size: 15px; 
        border-left: 4px solid transparent;
    }
    QPushButton#SidebarBtn:hover { background-color: #f5f7fa; color: #333; }
    QPushButton#SidebarBtn:checked { 
        background-color: #e6f2ff; 
        color: #0078d7; 
        font-weight: bold; 
        border-left: 4px solid #0078d7; 
    }

    /* Cards */
    QWidget#FlightCard, QWidget#OrderCard {
        background-color: transparent;
        border: none;
        padding: 4px 2px;
        margin-bottom: 26px;
    }
    QWidget#FlightCard QWidget#CardBody,
    QWidget#OrderCard QWidget#CardBody {
        background-color: #ffffff;
        border: 1px solid #dfe5ef;
        border-top-left-radius: 18px;
        border-top-right-radius: 18px;
        border-bottom: none;
        padding-top: 6px;
    }
    QWidget#FlightCard QWidget#InfoContainer,
    QWidget#OrderCard QWidget#InfoContainer {
        background-color: #f5f7fb;
        border: 1px solid #dfe5ef;
        border-top: 1px solid #edf1f7;
        border-bottom-left-radius: 18px;
        border-bottom-right-radius: 18px;
    }
    QWidget#FlightCard:hover QWidget#CardBody,
    QWidget#OrderCard:hover QWidget#CardBody {
        border-color: #7ab5ff;
        background-color: #f5f9ff;
    }
    QWidget#FlightCard:hover QWidget#InfoContainer,
    QWidget#OrderCard:hover QWidget#InfoContainer {
        border-color: #7ab5ff;
        border-top-color: #b3d6ff;
    }
    FlightCard QLabel, OrderCard QLabel { color: #2f3343; }
    QLabel#FlightId { font-size: 17px; font-weight: 600; color: #1f2c3d; }
    QLabel#TimeLabel { font-size: 28px; font-weight: 600; color: #2b79ff; }
    QLabel#CityLabel { font-size: 16px; font-weight: 500; color: #2f3343; }
    QLabel#AirportLabel { font-size: 13px; color: #7a8095; }
    QLabel#PriceLabel { font-size: 24px; font-weight: 600; color: #ff7b45; }
    QLabel#DurationLabel { font-size: 13px; color: #98a1b2; }
    QLabel#SeatsLabel { font-size: 13px; color: #4b556b; }
    QLabel#ArrowLabel { color: #a9b2c4; font-weight: bold; }

    /* 操作按钮样式 (统一结构，不同配色) */
    QPushButton#BookBtn,
    QPushButton#ChangeBtn,
    QPushButton#CancelBtn {
        border-radius: 18px;
        padding: 8px 28px;
        font-weight: 600;
        min-width: 120px;
        border-width: 1px;
    }
    QPushButton#BookBtn {
        background-color: rgba(71, 137, 255, 0.12);
        color: #2b63c7;
        border-color: #4b7bec;
    }
    QPushButton#BookBtn:hover {
        background-color: #4b7bec;
        color: #ffffff;
        border-color: #4b7bec;
    }
    QPushButton#ChangeBtn {
        background-color: rgba(45, 212, 191, 0.12);
        color: #0f766e;
        border-color: #10b981;
    }
    QPushButton#ChangeBtn:hover {
        background-color: #10b981;
        color: #ffffff;
        border-color: #10b981;
    }
    QPushButton#CancelBtn {
        background-color: rgba(239, 68, 68, 0.12);
        color: #d14343;
        border-color: #f19999;
    }
    QPushButton#CancelBtn:hover { 
        background-color: #ef4444; 
        color: #fff;
        border-color: #ef4444;
    }

    /* 查询按钮 */
    QPushButton#queryButton {
        background-color: #0078d7;
        color: white;
        border: none;
        min-width: 100px;
        font-size: 15px;
        padding: 10px 22px;
        border-radius: 8px;
    }
    QPushButton#queryButton:hover { background-color: #1084e3; }

    QWidget#InfoContainer {
        background-color: #f5f7fb;
        border-bottom-left-radius: 18px;
        border-bottom-right-radius: 18px;
        border-top: 1px solid #edf1f7;
    }

    /* 对话框样式 */
    QDialog {
        background-color: #f5f7fa;
    }
)";

// 登录页面专用样式 - 浅色
inline const QString LOGIN_LIGHT_STYLE = R"(
    QWidget { 
        background-color: #f0f2f5; 
        font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; 
    }
    
    QLineEdit { 
        padding: 12px; 
        border: 1px solid #dcdfe6; 
        border-radius: 8px; 
        font-size: 14px;
        background-color: white;
        color: #333333;
        selection-background-color: #0078d7;
    }
    QLineEdit:focus { border: 1px solid #0078d7; background-color: #fff; }
    
    QPushButton { 
        background-color: #0078d7; 
        color: white; 
        border: none; 
        border-radius: 8px; 
        padding: 12px; 
        font-size: 16px; 
        font-weight: bold;
    }
    QPushButton:hover { background-color: #1084e3; }
    QPushButton:pressed { background-color: #006cc1; }
    
    QLabel { color: #333; font-size: 14px; font-weight: 500; }
)";

// 登录页面专用样式 - 深色
inline const QString LOGIN_DARK_STYLE = R"(
    QWidget { 
        background-color: #1e1e1e; 
        font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; 
    }
    
    QLineEdit { 
        padding: 12px; 
        border: 1px solid #444; 
        border-radius: 8px; 
        font-size: 14px;
        background-color: #2d2d2d;
        color: #e0e0e0;
        selection-background-color: #0078d7;
    }
    QLineEdit:focus { border: 1px solid #0078d7; background-color: #333; }
    
    QPushButton { 
        background-color: #0078d7; 
        color: white; 
        border: none; 
        border-radius: 8px; 
        padding: 12px; 
        font-size: 16px; 
        font-weight: bold;
    }
    QPushButton:hover { background-color: #1084e3; }
    QPushButton:pressed { background-color: #006cc1; }
    
    QLabel { color: #e0e0e0; font-size: 14px; font-weight: 500; }
)";

} 

#endif // THEME_MANAGER_H
