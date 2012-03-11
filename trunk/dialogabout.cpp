#include "dialogabout.h"
#include "ui_dialogabout.h"
#include <QMessageBox>

// Public
    dialogAbout::dialogAbout(QWidget *parent) :
    QDialog(parent), ui(new Ui::dialogAbout)
    {
        ui->setupUi(this);
    }
    dialogAbout::~dialogAbout()
    {
        delete ui;
    }

// Protected:
    void dialogAbout::changeEvent(QEvent *e)
    {
        QDialog::changeEvent(e);
        switch (e->type())
        {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
            break;

            default:
            break;
        }
    }

// Private slots
    void dialogAbout::on_aboutQt_clicked()
    {qApp->aboutQt();}

    void dialogAbout::on_pushButton_clicked()
    {this->hide();}
