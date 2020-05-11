#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>
#include <QtMqtt/QtMqtt>
#include <QMessageBox>

namespace Ui {
class Connection;
}

class Connection : public QDialog
{
    Q_OBJECT

public:
    explicit Connection(QWidget *parent = nullptr);
    QString getHostName();
    int getPort();
    QString getUserName();
    QString getPassword();
    ~Connection();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Connection *ui;
    QString str_hostname;
    int port;
    QString str_username;
    QString str_password;
};

#endif // CONNECTION_H
