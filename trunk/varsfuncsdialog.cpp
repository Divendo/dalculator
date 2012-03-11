#include "varsfuncsdialog.h"
#include "ui_varsfuncsdialog.h"

#include <iostream>

// Public:
    varsFuncsDialog::varsFuncsDialog(QTCalc* calculator, QWidget *parent) :
    QDialog(parent), ui(new Ui::varsFuncsDialog),
    calculator(calculator),
    myVarsWidget(new varsWidget(calculator->getVars())),
    myFuncsWidget(new funcsWidget(calculator->getFuncs()))
    {
        ui->setupUi(this);

        ui->scrollVariables->setWidget(myVarsWidget);
        connect(myVarsWidget, SIGNAL(varAdded(const QString&, const calc::real&)), this, SLOT(variableAdded(const QString&, const calc::real&)));
        connect(myVarsWidget, SIGNAL(varAdded(const QString&, const calc::real&)), calculator, SLOT(setVar(const QString&, const calc::real&)));
        connect(myVarsWidget, SIGNAL(varRenamed(const QString&, const QString&)), calculator, SLOT(renameVar(const QString&, const QString&)));
        connect(myVarsWidget, SIGNAL(varValueChanged(const QString&, const calc::real&)), calculator, SLOT(setVar(const QString&, const calc::real&)));

        ui->scrollFunctions->setWidget(myFuncsWidget);
        connect(myFuncsWidget, SIGNAL(funcAdded(const QString&, const QString&)), this, SLOT(functionAdded(const QString&, const QString&)));
        connect(myFuncsWidget, SIGNAL(funcAdded(const QString&, const QString&)), calculator, SLOT(setFunc(const QString&, const QString&)));
        connect(myFuncsWidget, SIGNAL(funcRenamed(const QString&, const QString&)), calculator, SLOT(renameFunc(const QString&, const QString&)));
        connect(myFuncsWidget, SIGNAL(funcContentChanged(const QString&, const QString&)), calculator, SLOT(setFunc(const QString&, const QString&)));

        connect(this, SIGNAL(deleteVar(const QString&)), calculator, SLOT(deleteVar(const QString&)));
        connect(this, SIGNAL(deleteFunc(const QString&)), calculator, SLOT(deleteFunc(const QString&)));
    }

    varsFuncsDialog::~varsFuncsDialog()
    {
        delete myVarsWidget;
        delete myFuncsWidget;
        delete ui;
    }

// Public slots:
    void varsFuncsDialog::reload()
    {
        reloadVarList();
        reloadFuncList();
        reloadDeleteVarList();
        reloadDeleteFuncList();
    }

// Protected:
    void varsFuncsDialog::changeEvent(QEvent* e)
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

// Private slots:
    void varsFuncsDialog::variableAdded(const QString&, const calc::real&)
    {ui->scrollVariables->ensureVisible(0, myVarsWidget->height(), 0, 0);}

    void varsFuncsDialog::functionAdded(const QString&, const QString&)
    {ui->scrollVariables->ensureVisible(0, myFuncsWidget->height(), 0, 0);}

    void varsFuncsDialog::on_varsFuncsTab_currentChanged(int index)
    {
        // Only if we just switched to the delete tab we reload the list with variables and the list with functions
        if(index == 2)
        {
            reloadDeleteVarList();
            reloadDeleteFuncList();
        }
    }

    void varsFuncsDialog::on_varsList_itemSelectionChanged()
    {
        if(ui->varsList->selectedItems().size()!=0)
            ui->deleteVar->setEnabled(true);
    }

    void varsFuncsDialog::on_funcsList_itemSelectionChanged()
    {
        if(ui->funcsList->selectedItems().size()!=0)
            ui->deleteFunc->setEnabled(true);
    }

    void varsFuncsDialog::on_deleteVar_clicked()
    {
        QList<QListWidgetItem*> selectedItems=ui->varsList->selectedItems();
        for(QList<QListWidgetItem*>::const_iterator pos=selectedItems.begin(); pos!=selectedItems.end(); pos++)
            deleteVar((*pos)->text());
        ui->deleteVar->setEnabled(false);
        reloadDeleteVarList();
        reloadVarList();
    }

    void varsFuncsDialog::on_deleteFunc_clicked()
    {
        QList<QListWidgetItem*> selectedItems=ui->funcsList->selectedItems();
        for(QList<QListWidgetItem*>::const_iterator pos=selectedItems.begin(); pos!=selectedItems.end(); pos++)
            deleteFunc((*pos)->text());
        ui->deleteFunc->setEnabled(false);
        reloadDeleteFuncList();
        reloadFuncList();
    }

    void varsFuncsDialog::on_buttonClose_clicked()
    { this->accept(); }

// Private:
    void varsFuncsDialog::reloadVarList()
    { myVarsWidget->setVars(calculator->getVars()); }

    void varsFuncsDialog::reloadFuncList()
    { myFuncsWidget->setFuncs(calculator->getFuncs()); }

    void varsFuncsDialog::reloadDeleteVarList()
    {
        ui->varsList->clear();
        std::map<QString, calc::real> vars=calculator->getVars();
        for(std::map<QString, calc::real>::const_iterator pos=vars.begin(); pos!=vars.end(); pos++)
            ui->varsList->addItem(pos->first);
    }

    void varsFuncsDialog::reloadDeleteFuncList()
    {
        ui->funcsList->clear();
        std::map<QString, QString> funcs=calculator->getFuncs();
        for(std::map<QString, QString>::const_iterator pos=funcs.begin(); pos!=funcs.end(); pos++)
            ui->funcsList->addItem(pos->first);
    }
