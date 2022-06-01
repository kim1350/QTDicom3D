#ifndef DIALOG_H
#define DIALOG_H

#include <string>
#include <QDialog>
#include <vector>
#include <string>
#include <iostream>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void addlist(std::vector <std::string> &str);
    int number_s = -1;
private slots:
    void on_pushButtonDialog_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
