#include "formprogress.h"
#include "ui_formprogress.h"

FormProgress::FormProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormProgress)
{
    ui->setupUi(this);
}

FormProgress::~FormProgress()
{
    delete ui;
}

void FormProgress::updateProgress(int pr)
{
    ui->progressBar->setValue(pr);
}

void FormProgress::slot(int p) {
    //ui->progressBar->setValue(p);
}
