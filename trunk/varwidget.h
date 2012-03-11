#ifndef VARWIDGET_H
#define VARWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include "calc/calc.h"

class varWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit varWidget(const QString& name, const calc::real& value, QWidget *parent = 0, const int& minWidth=350);

        QString varName() const;
        calc::real varValue() const;

        static QString checkName(const QString& name);

    public slots:
        void rename(QString newName, const QString& forceOldName = "");
        void changeValue(const calc::real& newValue);

    signals:
        void renamed(const QString& oldName, const QString& newName, varWidget* self);
        void valueChanged(const QString& name, const calc::real& newValue);

    private slots:
        void nameEdited(const QString& newName);
        void valueHasChanged(const double& newValue);

    private:
        QLineEdit* nameEdit;
        QDoubleSpinBox* valueEdit;
        QString oldName;
};

#endif // VARWIDGET_H
