#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
    QStringList listColumnValue;
    ui->valueTable->setColumnCount(3);
    listColumnValue << "Topic" << "Message" << "Date Time";
    ui->valueTable->setHorizontalHeaderLabels(listColumnValue);
    ui->valueTable->setColumnWidth(0, 244);
    ui->valueTable->setColumnWidth(1, 259);
    ui->valueTable->setColumnWidth(2, 259);
    ui->valueTable->insertRow(ui->valueTable->rowCount());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionConnection_triggered()
{
    Connection connection;
    connection.setModal(true);
    connection.exec();

    client = new QMqttClient(this);
    client->setHostname(connection.getHostName());
    client->setPort(connection.getPort());
    client->setUsername(connection.getUserName());
    client->setPassword(connection.getPassword());

    connect(client, &QMqttClient::stateChanged, this, &MainWindow::updateLogStateChange);
    connect(client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);
    connect(client, &QMqttClient::messageReceived, this, &MainWindow::updateMessage);
    connect(client, &QMqttClient::pingResponseReceived, this, [this](){
            const QString content = QDateTime::currentDateTime().toString()
                    +QLatin1String(" Ping Response ")
                    +QLatin1Char('\n');
            ui->pteLog->insertPlainText(content);
    });
    updateLogStateChange();
    client->connectToHost();
}

void MainWindow::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                +QLatin1String(": State Change")
                +QString::number(client->state())
                +QLatin1Char('\n');
    ui->pteLog->insertPlainText(content);
}

void MainWindow::brokerDisconnected()
{
    ui->pteLog->insertPlainText("Disconnected from server");
}

void MainWindow::updateMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    int currentRow = ui->valueTable->rowCount()-1;
    const QString content = QDateTime::currentDateTime().toString()
               +QLatin1String(" Received Topic: ")
               +topic.name()
               +QLatin1String(" Message: ")
               +message+QLatin1Char('\n')
               +QLatin1Char('\n');
    ui->pteLog->insertPlainText(content);
    ui->valueTable->insertRow(ui->valueTable->rowCount());
    ui->valueTable->setItem(currentRow, Topic, new QTableWidgetItem(topic.name()));
    ui->valueTable->setItem(currentRow, Message, new QTableWidgetItem(QString::fromStdString(message.toStdString())));
    ui->valueTable->setItem(currentRow, DateTime, new QTableWidgetItem(QDateTime::currentDateTime().toString()));

    updateMinMaxAverage();
}

void MainWindow::on_btnSubscribe_clicked()
{
    auto subscription = client->subscribe(ui->qleTopic->text(), ui->sbQoS->value());

    if(!subscription){
         QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
         return;
    }else{
        if(ui->btnSubscribe->text() == "Subscribe"){
             ui->btnSubscribe->setText(QLatin1String("Unsubscribe"));
             ui->qleTopic->setEnabled(false);
             ui->sbQoS->setEnabled(false);
        }else{
             ui->btnSubscribe->setText(QLatin1String("Subscribe"));
             client->unsubscribe(ui->qleTopic->text());
             ui->qleTopic->setEnabled(true);
             ui->sbQoS->setEnabled(true);

             while(ui->valueTable->rowCount() > 0){
                 ui->valueTable->removeRow(ui->valueTable->rowCount()-1);
             }
             ui->valueTable->insertRow(ui->valueTable->rowCount());
             ui->leMax->setText(nullptr);
             ui->leMin->setText(nullptr);
             ui->leAverage->setText(nullptr);
        }
    }
}

void MainWindow::on_actionClose_triggered()
{
    this->close();
}

void MainWindow::on_actionCreated_by_ATN_triggered()
{
    About about;
    about.setModal(true);
    about.exec();
}

void MainWindow::updateMinMaxAverage()
{
     double min = getMinValue();
     QString minValue = QString::number(min);
     ui->leMin->setText(minValue);

     double max = getMaxValue();
     QString maxValue = QString::number(max);
     ui->leMax->setText(maxValue);

     double average = getAverageValue();
     QString averageValue = QString::number(average);
     ui->leAverage->setText(averageValue);
}


double MainWindow::getMinValue()
{
    double min, currentValue;
    QString cellValue;

    min = ui->valueTable->item(0, Message)->text().toDouble();
    currentValue = 0;

    for(int i=0; i < ui->valueTable->rowCount()-1; i++)
    {
        cellValue = ui->valueTable->item(i, Message)->text();
        currentValue = cellValue.toDouble();
        if(currentValue < min){
            min = currentValue;
        }
    }
    return min;
}

double MainWindow::getMaxValue()
{
    double max, currentValue;
    QString cellValue;

    max = ui->valueTable->item(0, Message)->text().toDouble();
    currentValue = 0;

    for(int i=0; i < ui->valueTable->rowCount()-1; i++)
    {
        cellValue = ui->valueTable->item(i, Message)->text();
        currentValue = cellValue.toDouble();
        if(currentValue > max){
            max = currentValue;
        }
    }
    return max;
}

double MainWindow::getAverageValue()
{
    double sum, currentValue, countItem;
    QString cellValue;

    countItem = 0;
    sum = 0;
    currentValue = 0;

    for(int i=0; i < ui->valueTable->rowCount()-1; i++)
    {
        cellValue = ui->valueTable->item(i, Message)->text();
        if(!cellValue.isNull())
        {
            currentValue = cellValue.toDouble();
            sum += currentValue;
            countItem++;
        }
    }

    return (sum/countItem);
}

void MainWindow::on_actionExport_Excel_Data_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save this value table", "", tr("Excel File (*.xlsx);;All Files (*)"));

    if(fileName.isEmpty()){
        return;
    }else{
        QXlsx::Document excelFie;
        int excelCol = 0;
        excelFie.addSheet("Topic: "+ui->qleTopic->text());
        excelFie.write("A1", "Topic");
        excelFie.setColumnWidth(1, 20);
        excelFie.write("B1", "Message");
        excelFie.setColumnWidth(2, 30);
        excelFie.write("C1", "Date Time");
        excelFie.setColumnWidth(3, 30);

        for(int i=0;i < ui->valueTable->rowCount()-1; i++)
        {
            excelCol = i + 2;
            excelFie.write("A"+QString::number(excelCol), ui->valueTable->item(i, Topic)->text());
            excelFie.write("B"+QString::number(excelCol), ui->valueTable->item(i, Message)->text());
            excelFie.write("C"+QString::number(excelCol), ui->valueTable->item(i, DateTime)->text());
        }
        excelFie.saveAs(fileName);
    }
}

void MainWindow::on_actionExport_PDF_Data_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save this value table", "", tr("PDF File (*.pdf);;All Files (*)"));

    if(fileName.isEmpty()){
        return;
    }else{
       const int columns = ui->valueTable->columnCount();
       const int rows = ui->valueTable->rowCount();
       QTextDocument doc;
       QTextCursor cursor(&doc);
       QTextTableFormat tableFormat;
       tableFormat.setHeaderRowCount(1);
       tableFormat.setAlignment(Qt::AlignHCenter);
       tableFormat.setCellPadding(0);
       tableFormat.setCellSpacing(0);
       tableFormat.setBorder(1);
       tableFormat.setBorderBrush(QBrush(Qt::SolidPattern));
       tableFormat.clearColumnWidthConstraints();
       QTextTable *textTable = cursor.insertTable(rows+1, columns, tableFormat);
       QTextCharFormat tableHeaderFormat;
       tableHeaderFormat.setBackground(QColor("#DADADA"));

       for(int i = 0; i < columns; i++){
            QTextTableCell cell = textTable->cellAt(0,i);
            cell.setFormat(tableHeaderFormat);
            QTextCursor cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(ui->valueTable->horizontalHeaderItem(i)->data(Qt::DisplayRole).toString());
       }

       for (int i = 0; i < rows; i++) {
           for (int j = 0; j < columns; j++) {
               QTableWidgetItem *item = ui->valueTable->item(i, j);
               if (!item || item->text().isEmpty()) {
                  ui->valueTable->setItem(i, j, new QTableWidgetItem(""));
               }
               QTextTableCell cell = textTable->cellAt(i+1, j);
               QTextCursor cellCursor = cell.firstCursorPosition();
               cellCursor.insertText(ui->valueTable->item(i, j)->text());
           }
       }

       cursor.movePosition(QTextCursor::End);
       QPrinter printer(QPrinter::PrinterResolution);
       printer.setOutputFormat(QPrinter::PdfFormat);
       printer.setPaperSize(QPrinter::A4);
       printer.setOrientation(QPrinter::Landscape);
       printer.setOutputFileName(fileName);
       doc.setDocumentMargin(0);
       doc.setTextWidth(5);
       doc.print(&printer);
    }
}
