#ifndef FORMPROGRESS_H
#define FORMPROGRESS_H

#include <QWidget>

namespace Ui {
class FormProgress;
}

class FormProgress : public QWidget
{
    Q_OBJECT

public:
    explicit FormProgress(QWidget *parent = nullptr);
    ~FormProgress();

    void updateProgress(int pr);
public slots:
    void slot(int p);
private:
    Ui::FormProgress *ui;
signals:
    void progtWindow();
};


#endif // FORMPROGRESS_H
