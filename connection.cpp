#include "connection.h"
#include "ui_connection.h"

Connection::Connection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection)
{
    ui->setupUi(this);
}

Connection::~Connection()
{
    delete ui;
}

void Connection::on_buttonBox_accepted()
{
    if(ui->qleHost->text().isEmpty()){
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Hostname is empty!"));
    }else if(ui->qleUsername->text().isEmpty()){
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Username is empty!"));
    }else if(ui->qlePassword->text().isEmpty()){
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Password is empty!"));
    }else{
        str_hostname = ui->qleHost->text();
        port = ui->sbPort->value();
        str_username = ui->qleUsername->text();
        str_password = ui->qlePassword->text();
    }
}

QString Connection::getHostName()
{
    return str_hostname;
}

int Connection::getPort()
{
    return port;
}

QString Connection::getUserName()
{
    return str_username;
}

QString Connection::getPassword()
{
    return str_password;
}

