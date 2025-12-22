#ifndef SEAT_SELECTION_DIALOG_H
#define SEAT_SELECTION_DIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QMap>

class SeatSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SeatSelectionDialog(const QString& flightId, const QStringList& occupiedSeats, 
                                  int totalSeats = 180, QWidget *parent = nullptr);
    QString selectedSeat() const { return m_selectedSeat; }

private slots:
    void onSeatClicked();

private:
    void setupUI();
    void createSeatButton(int row, int col, const QString& seatId);

    QString m_flightId;
    QStringList m_occupiedSeats;
    QString m_selectedSeat;
    QMap<QString, QPushButton*> m_seatButtons;
    QPushButton* m_currentSelected = nullptr;
    QLabel* m_selectedLabel = nullptr;
    
    int m_totalSeats;
    int m_rows;
    static const int COLS = 6; 
};

#endif // SEAT_SELECTION_DIALOG_H
