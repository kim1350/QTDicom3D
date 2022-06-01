#include "dialog.h"
#include "ui_dialog.h"
#include <string>
using namespace std;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
   
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::addlist(std::vector<std::string>& str)
{
    ui->listWidget->clear();
    for (int i = 0; i < str.size(); i++) {
        ui->listWidget->addItem(QString::fromUtf8(str[i].c_str()));
    }
}



void Dialog::on_pushButtonDialog_clicked()
{
    QString item = ui->listWidget->currentItem()->text();
    number_s = atoi(QString(item[0]).toStdString().c_str());
    accept();
}

