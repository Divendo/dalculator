#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>

namespace Ui {
    class dialogAbout;
}

class dialogAbout : public QDialog
{
    Q_OBJECT

    public:
        explicit dialogAbout(QWidget *parent = 0);
        ~dialogAbout();

    protected:
        void changeEvent(QEvent *e);

    private:
        Ui::dialogAbout *ui;

    private slots:
        void on_pushButton_clicked();
        void on_aboutQt_clicked();
};

#endif // DIALOGABOUT_H
