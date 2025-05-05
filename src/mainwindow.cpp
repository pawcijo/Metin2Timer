#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QStringList>
#include <QTimer>
#include <QDateTimeEdit>
#include <QDebug>
#include <QSoundEffect>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDateTime now = QDateTime::currentDateTime();
    ui->buttonSetTime->setText(now.toString("HH:mm:ss\ndd/MM/yyyy"));
    ui->labelCurrentTime->setText("Ustawiony czas: " + now.toString("HH:mm:ss"));

    // Timer to update current time and check matches
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        QDateTime currentTime = QDateTime::currentDateTime();
        ui->CurrentTimeDisplay->setText("Godzina: " + currentTime.toString("HH:mm:ss"));
        checkTimeMatch(currentTime);
    });
    timer->start(1000);

    // Setup button to open time dialog
    connect(ui->buttonSetTime, &QPushButton::clicked, this, [this]() {
        QDialog *timeDialog = new QDialog(this);
        timeDialog->setWindowTitle("Set Time");

        QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), timeDialog);
        dateTimeEdit->setDisplayFormat("HH:mm:ss\ndd/MM/yyyy");
        dateTimeEdit->setCalendarPopup(true);

        QVBoxLayout *layout = new QVBoxLayout(timeDialog);
        layout->addWidget(dateTimeEdit);

        QPushButton *okButton = new QPushButton("Ok", timeDialog);
        layout->addWidget(okButton);
        connect(okButton, &QPushButton::clicked, timeDialog, &QDialog::accept);

        if (timeDialog->exec() == QDialog::Accepted) {
            QDateTime newTime = dateTimeEdit->dateTime();
            ui->buttonSetTime->setText(newTime.toString("HH:mm:ss\ndd/MM/yyyy"));
            ui->labelCurrentTime->setText("Ustawiony czas: " + newTime.toString("HH:mm:ss"));
            updateAllTimes(newTime);
        }

        delete timeDialog;
    });

    // Checkbox connections
    auto connectCheckbox = [this](QCheckBox *cb) {
        connect(cb, &QCheckBox::toggled, this, [this]() {
            updateAllTimesBasedOnCheckboxes();
        });
    };

    connect(ui->pushButtonSelectSound, &QPushButton::clicked, this, &MainWindow::onSelectSoundClicked);

    connectCheckbox(ui->checkBox_2min);
    connectCheckbox(ui->checkBox_3min);
    connectCheckbox(ui->checkBox_5min);
    connectCheckbox(ui->checkBox_10min);
    connectCheckbox(ui->checkBox_15min);
    connectCheckbox(ui->checkBox_20min);
    connectCheckbox(ui->checkBox_30min);
    connectCheckbox(ui->checkBox_1h);
    connectCheckbox(ui->checkBox_2h);
    connectCheckbox(ui->checkBox_6h);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateAllTimes(const QDateTime &baseTime)
{
    timeSlots.clear(); // Reset

    addTimeSlots(ui->checkBox_2min, ui->label_2min, 2 * 60, baseTime);
    addTimeSlots(ui->checkBox_3min, ui->label_3min, 3 * 60, baseTime);
    addTimeSlots(ui->checkBox_5min, ui->label_5min, 5 * 60, baseTime);
    addTimeSlots(ui->checkBox_10min, ui->label_10min, 10 * 60, baseTime);
    addTimeSlots(ui->checkBox_15min, ui->label_15min, 15 * 60, baseTime);
    addTimeSlots(ui->checkBox_20min, ui->label_20min, 20 * 60, baseTime);
    addTimeSlots(ui->checkBox_30min, ui->label_30min, 30 * 60, baseTime);
    addTimeSlots(ui->checkBox_1h, ui->label_1h, 60 * 60, baseTime);
    addTimeSlots(ui->checkBox_2h, ui->label_2h, 2 * 60 * 60, baseTime);
    addTimeSlots(ui->checkBox_6h, ui->label_6h, 6 * 60 * 60, baseTime);
}

void MainWindow::addTimeSlots(QCheckBox *checkbox, QLabel *label, int intervalSeconds, const QDateTime &baseTime)
{
    if (!checkbox->isChecked()) {
        label->clear();
        return;
    }

    QStringList timeStrings;
    for (int i = 1; i <= 5; ++i) {
        QDateTime t = baseTime.addSecs(i * intervalSeconds);
        QString timeStr = t.toString("HH:mm:ss");
        timeStrings << timeStr;
        timeSlots.append(TimeSlot(t, label));
    }
    label->setText(timeStrings.join("  "));
}

void MainWindow::updateAllTimesBasedOnCheckboxes()
{
    QDateTime newTime = QDateTime::fromString(ui->buttonSetTime->text(), "HH:mm:ss\ndd/MM/yyyy");
    updateAllTimes(newTime);
}

void MainWindow::checkTimeMatch(const QDateTime &currentTime)
{
    QString nowStr = currentTime.toString("HH:mm:ss");

    for (auto &slot : timeSlots) {
        if (slot.time.toString("HH:mm:ss") == nowStr && !slot.isHighlighted) {
            playSound();
            slot.isHighlighted = true;
            QString originalText = slot.label->text();
            QStringList parts = originalText.split("  ");

            // Wrap the matching time in a span with initial color
            for (int i = 0; i < parts.size(); ++i) {
                if (parts[i] == nowStr) {
                    parts[i] = QString("<span id='timeMatch'>%1</span>").arg(parts[i]);
                    break;
                }
            }

            QString modifiedText = parts.join("  ");
            slot.label->setText(modifiedText);

            // Start blinking color change
            auto label = slot.label;
            int durationMs = 10000;
            int intervalMs = 1000;
            int *counter = new int(0); // manage lifetime

            QTimer *blinkTimer = new QTimer(this);
            connect(blinkTimer, &QTimer::timeout, this, [label, nowStr, originalText, counter, blinkTimer, durationMs, intervalMs]() {
                QString text = label->text();
                QString color = (*counter % 2 == 0) ? "red" : "blue";

                // Replace the span color
                QRegularExpression regex(R"(<span[^>]*>(\Q)" + nowStr + R"(\E)</span>)");
                text.replace(regex, QString("<span style='color:%1;'>\\1</span>").arg(color));
                label->setText(text);

                (*counter)++;
                if (*counter >= durationMs / intervalMs) {
                    blinkTimer->stop();
                    label->setText(originalText);
                    label->update();
                    delete counter;
                    blinkTimer->deleteLater();
                }
            });

            blinkTimer->start(intervalMs);
        }
    }
}


void MainWindow::playSound()
{
    if (!selectedSoundPath.isEmpty()) {
        soundEffect.setSource(QUrl::fromLocalFile(selectedSoundPath));
        soundEffect.setVolume(0.8f);
        soundEffect.play();
        qDebug() << "Playing sound:" << selectedSoundPath;
    } else {
        qDebug() << "No sound selected.";
    }
}

void MainWindow::onSelectSoundClicked()
{
    // Open a file dialog to select a sound file
    QString filter = "WAV files (*.wav);;All files (*)";
    QString file = QFileDialog::getOpenFileName(this, "Select Sound", QString(), filter);

    if (!file.isEmpty()) {
        // If a file is selected, store its path in the member variable
        selectedSoundPath = file;
        qDebug() << "Selected sound:" << selectedSoundPath;
    }
}
