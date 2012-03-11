#ifndef VARSFUNCSDIALOG_H
#define VARSFUNCSDIALOG_H

#include <QDialog>
#include "qtcalc.h"
#include "calc/calc.h"
#include "varswidget.h"
#include "funcswidget.h"

namespace Ui
{
    class varsFuncsDialog;
}

class varsFuncsDialog : public QDialog
{
    Q_OBJECT

    public:
        varsFuncsDialog(QTCalc* calculator, QWidget *parent = 0);
        ~varsFuncsDialog();

    public slots:
        void reload();

    protected:
        void changeEvent(QEvent* e);

    signals:
        void deleteVar(const QString& var);
        void deleteFunc(const QString& func);

    private slots:
        void on_buttonClose_clicked();
        void on_deleteFunc_clicked();
        void on_deleteVar_clicked();
        void on_funcsList_itemSelectionChanged();
        void on_varsList_itemSelectionChanged();
        void on_varsFuncsTab_currentChanged(int index);
        void variableAdded(const QString& name, const calc::real& value);
        void functionAdded(const QString& name, const QString& content);

    private:
        void reloadVarList();
        void reloadFuncList();
        void reloadDeleteVarList();
        void reloadDeleteFuncList();

        Ui::varsFuncsDialog* ui;

        QTCalc* calculator;

        varsWidget* myVarsWidget;
        funcsWidget* myFuncsWidget;
};

#endif // VARSFUNCSDIALOG_H
