#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMqtt/QtMqtt>
#include <connection.h>
#include <QDebug>
#include <QtXlsx>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPrinter>
#include "about.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateLogStateChange();
    void brokerDisconnected();
    void updateMessage(const QByteArray &message, const QMqttTopicName &topic);
    void on_actionConnection_triggered();

    void on_btnSubscribe_clicked();

    void on_actionClose_triggered();

    void on_actionCreated_by_ATN_triggered();

    void on_actionExport_Excel_Data_triggered();

    void on_actionExport_PDF_Data_triggered();

private:
    Ui::MainWindow *ui;
    QMqttClient *client;
    double getMinValue();
    double getMaxValue();
    double getAverageValue();
    enum Column{
        Topic,
        Message,
        DateTime
    };
    const int columnNumber = DateTime-Topic+1;
    void updateMinMaxAverage();
};
#endif // MAINWINDOW_H
