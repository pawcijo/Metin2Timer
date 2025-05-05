#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>
#include <QSoundEffect>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct TimeSlot {
    QDateTime time;
    QLabel* label;
    bool isHighlighted = false;

    TimeSlot(const QDateTime &t, QLabel* l) : time(t), label(l) {}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QVector<TimeSlot> timeSlots;
    QString selectedSoundPath; // Stores the selected sound file path
    QSoundEffect soundEffect;

    void updateAllTimes(const QDateTime &baseTime);
    void updateAllTimesBasedOnCheckboxes();
    void addTimeSlots(QCheckBox *checkbox, QLabel *label, int intervalSeconds, const QDateTime &baseTime);
    void checkTimeMatch(const QDateTime &currentTime);
    void playSound();
    void onSelectSoundClicked(); // New slot for handling sound selection
};

#endif // MAINWINDOW_H
