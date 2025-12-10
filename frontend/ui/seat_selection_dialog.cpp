#include "seat_selection_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QtMath>

SeatSelectionDialog::SeatSelectionDialog(const QString& flightId, const QStringList& occupiedSeats, 
                                          int totalSeats, QWidget *parent)
    : QDialog(parent), m_flightId(flightId), m_occupiedSeats(occupiedSeats), m_totalSeats(totalSeats)
{
    m_rows = qCeil(totalSeats / 6.0);
    if (m_rows < 1) m_rows = 1;
    if (m_rows > 50) m_rows = 50;
    
    setWindowTitle(QString("选择座位 - 航班 %1").arg(flightId));
    setMinimumSize(600, 720);
    resize(620, 780);
    setupUI();
}

void SeatSelectionDialog::setupUI()
{
    // 应用整体样式
    setStyleSheet(R"(
        QDialog {
            background-color: #0f172a;
        }
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: #1e293b;
            width: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #475569;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #64748b;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ========== 顶部信息栏 ==========
    QWidget *headerWidget = new QWidget(this);
    headerWidget->setStyleSheet("background-color: #1e293b;");
    headerWidget->setFixedHeight(100);
    
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(25, 15, 25, 15);
    headerLayout->setSpacing(8);
    
    // 航班信息标题
    QLabel *flightLabel = new QLabel(QString("✈  航班 %1").arg(m_flightId), headerWidget);
    flightLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #f1f5f9;");
    
    QLabel *infoLabel = new QLabel(QString("共 %1 个座位 · %2 排 · 请选择您的座位")
                                    .arg(m_totalSeats).arg(m_rows), headerWidget);
    infoLabel->setStyleSheet("font-size: 13px; color: #94a3b8;");
    
    headerLayout->addWidget(flightLabel);
    headerLayout->addWidget(infoLabel);
    
    mainLayout->addWidget(headerWidget);

    // ========== 图例和已选显示 ==========
    QWidget *legendWidget = new QWidget(this);
    legendWidget->setStyleSheet("background-color: #1e293b; border-top: 1px solid #334155;");
    legendWidget->setFixedHeight(55);
    
    QHBoxLayout *legendLayout = new QHBoxLayout(legendWidget);
    legendLayout->setContentsMargins(25, 10, 25, 10);
    legendLayout->setSpacing(25);
    
    auto createLegend = [](const QString& bgColor, const QString& borderColor, const QString& text) {
        QWidget *w = new QWidget();
        QHBoxLayout *l = new QHBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(8);
        QLabel *dot = new QLabel();
        dot->setFixedSize(20, 20);
        dot->setStyleSheet(QString("background-color: %1; border: 2px solid %2; border-radius: 4px;")
                          .arg(bgColor, borderColor));
        QLabel *label = new QLabel(text);
        label->setStyleSheet("font-size: 12px; color: #94a3b8;");
        l->addWidget(dot);
        l->addWidget(label);
        return w;
    };
    
    legendLayout->addWidget(createLegend("#22c55e", "#16a34a", "可选"));
    legendLayout->addWidget(createLegend("#ef4444", "#dc2626", "已售"));
    legendLayout->addWidget(createLegend("#3b82f6", "#2563eb", "已选"));
    legendLayout->addStretch();
    
    // 已选座位显示
    m_selectedLabel = new QLabel("未选择座位", legendWidget);
    m_selectedLabel->setStyleSheet(R"(
        font-size: 14px; 
        font-weight: bold; 
        color: #f1f5f9;
        background-color: #334155;
        padding: 6px 16px;
        border-radius: 6px;
    )");
    legendLayout->addWidget(m_selectedLabel);
    
    mainLayout->addWidget(legendWidget);

    // ========== 座位区域 ==========
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *seatContainer = new QWidget();
    seatContainer->setStyleSheet("background-color: #0f172a;");
    
    QVBoxLayout *seatContainerLayout = new QVBoxLayout(seatContainer);
    seatContainerLayout->setContentsMargins(20, 20, 20, 30);
    seatContainerLayout->setSpacing(0);
    
    // 飞机机头装饰
    QWidget *noseWidget = new QWidget(seatContainer);
    noseWidget->setFixedHeight(60);
    noseWidget->setStyleSheet(R"(
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #334155, stop:1 #1e293b);
        border-radius: 80px 80px 0 0;
        margin: 0 80px;
    )");
    seatContainerLayout->addWidget(noseWidget, 0, Qt::AlignCenter);
    
    // 座位网格容器
    QWidget *gridContainer = new QWidget(seatContainer);
    gridContainer->setStyleSheet(R"(
        background-color: #1e293b;
        border-left: 3px solid #475569;
        border-right: 3px solid #475569;
        padding: 15px;
    )");
    
    QVBoxLayout *gridOuterLayout = new QVBoxLayout(gridContainer);
    gridOuterLayout->setContentsMargins(15, 10, 15, 10);
    gridOuterLayout->setSpacing(0);
    
    // 列标题（座位类型提示）
    QWidget *colHeaderWidget = new QWidget();
    QHBoxLayout *colHeaderLayout = new QHBoxLayout(colHeaderWidget);
    colHeaderLayout->setContentsMargins(35, 0, 0, 10);
    colHeaderLayout->setSpacing(0);
    
    auto createColHeader = [](const QString& text, const QString& hint) {
        QWidget *w = new QWidget();
        w->setFixedWidth(70);
        QVBoxLayout *l = new QVBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(2);
        QLabel *mainLabel = new QLabel(text);
        mainLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e2e8f0;");
        mainLabel->setAlignment(Qt::AlignCenter);
        QLabel *hintLabel = new QLabel(hint);
        hintLabel->setStyleSheet("font-size: 10px; color: #64748b;");
        hintLabel->setAlignment(Qt::AlignCenter);
        l->addWidget(mainLabel);
        l->addWidget(hintLabel);
        return w;
    };
    
    colHeaderLayout->addWidget(createColHeader("A", "靠窗"));
    colHeaderLayout->addWidget(createColHeader("B", "中间"));
    colHeaderLayout->addWidget(createColHeader("C", "过道"));
    
    // 过道间隔
    QWidget *aisleHeader = new QWidget();
    aisleHeader->setFixedWidth(60);
    QLabel *aisleLabel = new QLabel("过道");
    aisleLabel->setStyleSheet("font-size: 11px; color: #475569;");
    aisleLabel->setAlignment(Qt::AlignCenter);
    QVBoxLayout *al = new QVBoxLayout(aisleHeader);
    al->setContentsMargins(0, 0, 0, 0);
    al->addWidget(aisleLabel);
    colHeaderLayout->addWidget(aisleHeader);
    
    colHeaderLayout->addWidget(createColHeader("D", "过道"));
    colHeaderLayout->addWidget(createColHeader("E", "中间"));
    colHeaderLayout->addWidget(createColHeader("F", "靠窗"));
    
    gridOuterLayout->addWidget(colHeaderWidget);
    
    // 座位网格
    QWidget *seatGridWidget = new QWidget();
    QGridLayout *seatLayout = new QGridLayout(seatGridWidget);
    seatLayout->setSpacing(10);
    seatLayout->setContentsMargins(0, 0, 0, 0);

    int seatCount = 0;
    for (int row = 1; row <= m_rows && seatCount < m_totalSeats; ++row) {
        // 行号
        QLabel *rowLabel = new QLabel(QString::number(row));
        rowLabel->setAlignment(Qt::AlignCenter);
        rowLabel->setStyleSheet("font-size: 12px; font-weight: bold; color: #64748b;");
        rowLabel->setFixedWidth(30);
        seatLayout->addWidget(rowLabel, row - 1, 0);

        int colIndex = 1;
        for (int col = 0; col < COLS && seatCount < m_totalSeats; ++col) {
            QString seatId = QString("%1%2").arg(row).arg(QChar('A' + col));
            createSeatButton(row, colIndex, seatId);
            seatLayout->addWidget(m_seatButtons[seatId], row - 1, colIndex);
            colIndex++;
            seatCount++;
            
            // 过道（C和D之间）
            if (col == 2) {
                QWidget *aisle = new QWidget();
                aisle->setFixedWidth(60);
                seatLayout->addWidget(aisle, row - 1, colIndex);
                colIndex++;
            }
        }
    }
    
    gridOuterLayout->addWidget(seatGridWidget);
    seatContainerLayout->addWidget(gridContainer);
    
    // 飞机机尾装饰
    QWidget *tailWidget = new QWidget(seatContainer);
    tailWidget->setFixedHeight(40);
    tailWidget->setStyleSheet(R"(
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #1e293b, stop:1 #334155);
        border-radius: 0 0 40px 40px;
        margin: 0 100px;
    )");
    seatContainerLayout->addWidget(tailWidget, 0, Qt::AlignCenter);
    
    scrollArea->setWidget(seatContainer);
    mainLayout->addWidget(scrollArea, 1);

    // ========== 底部按钮栏 ==========
    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setStyleSheet("background-color: #1e293b; border-top: 1px solid #334155;");
    bottomWidget->setFixedHeight(80);
    
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(25, 15, 25, 15);
    bottomLayout->setSpacing(15);
    
    // 座位提示
    QLabel *tipLabel = new QLabel("💡 建议：靠窗座位视野更好，过道座位进出方便", bottomWidget);
    tipLabel->setStyleSheet("font-size: 12px; color: #64748b;");
    bottomLayout->addWidget(tipLabel);
    
    bottomLayout->addStretch();
    
    // 取消按钮
    QPushButton *cancelBtn = new QPushButton("取消", bottomWidget);
    cancelBtn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #94a3b8;
            border: 1px solid #475569;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #334155;
            color: #f1f5f9;
        }
    )");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    bottomLayout->addWidget(cancelBtn);
    
    // 确认按钮
    QPushButton *confirmBtn = new QPushButton("确认选座", bottomWidget);
    confirmBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #3b82f6, stop:1 #8b5cf6);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #2563eb, stop:1 #7c3aed);
        }
        QPushButton:disabled {
            background: #475569;
            color: #94a3b8;
        }
    )");
    confirmBtn->setCursor(Qt::PointingHandCursor);
    connect(confirmBtn, &QPushButton::clicked, this, [this]() {
        if (m_selectedSeat.isEmpty()) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("提示");
            msgBox.setText("请先选择一个座位");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet(R"(
                QMessageBox { background-color: #1e293b; }
                QMessageBox QLabel { color: #f1f5f9; }
                QPushButton { 
                    background-color: #3b82f6; color: white;
                    border: none; border-radius: 6px; padding: 8px 20px;
                }
            )");
            msgBox.exec();
            return;
        }
        accept();
    });
    bottomLayout->addWidget(confirmBtn);
    
    mainLayout->addWidget(bottomWidget);
}

void SeatSelectionDialog::createSeatButton(int row, int col, const QString& seatId)
{
    QPushButton *btn = new QPushButton(seatId);
    btn->setFixedSize(68, 46);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("seatId", seatId);

    bool isOccupied = m_occupiedSeats.contains(seatId);
    
    if (isOccupied) {
        btn->setEnabled(false);
        btn->setCursor(Qt::ForbiddenCursor);
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: #7f1d1d;
                color: #fca5a5;
                border: 2px solid #dc2626;
                border-radius: 8px;
                font-size: 14px;
                font-weight: bold;
            }
        )");
    } else {
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: #14532d;
                color: #86efac;
                border: 2px solid #22c55e;
                border-radius: 8px;
                font-size: 14px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #166534;
                border-color: #4ade80;
                transform: scale(1.05);
            }
        )");
        connect(btn, &QPushButton::clicked, this, &SeatSelectionDialog::onSeatClicked);
    }

    m_seatButtons[seatId] = btn;
}

void SeatSelectionDialog::onSeatClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString seatId = btn->property("seatId").toString();

    // 恢复上一个选中的座位
    if (m_currentSelected && m_currentSelected != btn) {
        m_currentSelected->setStyleSheet(R"(
            QPushButton {
                background-color: #14532d;
                color: #86efac;
                border: 2px solid #22c55e;
                border-radius: 8px;
                font-size: 14px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #166534;
                border-color: #4ade80;
            }
        )");
    }

    // 设置当前选中
    m_selectedSeat = seatId;
    m_currentSelected = btn;
    btn->setStyleSheet(R"(
        QPushButton {
            background-color: #1d4ed8;
            color: white;
            border: 2px solid #3b82f6;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
        }
    )");
    
    // 更新已选座位显示
    QString seatType;
    QChar colChar = seatId.at(seatId.length() - 1);
    if (colChar == 'A' || colChar == 'F') {
        seatType = "靠窗";
    } else if (colChar == 'C' || colChar == 'D') {
        seatType = "过道";
    } else {
        seatType = "中间";
    }
    
    m_selectedLabel->setText(QString("已选: %1 (%2)").arg(seatId, seatType));
    m_selectedLabel->setStyleSheet(R"(
        font-size: 14px; 
        font-weight: bold; 
        color: #22c55e;
        background-color: rgba(34, 197, 94, 0.15);
        border: 1px solid #22c55e;
        padding: 6px 16px;
        border-radius: 6px;
    )");
}
