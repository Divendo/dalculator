#include "varwidget.h"
#include <QHBoxLayout>

#include <limits>
#include <iostream>
#include <iomanip>
#include <cmath>

// Public:
    varWidget::varWidget(const QString& name, const calc::real& value, QWidget *parent, const int& minWidth) :
    QWidget(parent), nameEdit(new QLineEdit(varWidget::checkName(name), this)), valueEdit(new QDoubleSpinBox(this)), oldName(varWidget::checkName(name))
    {
        QHBoxLayout* layout=new QHBoxLayout(this);

        nameEdit->setToolTip(tr("Name of the variable"));
        connect(nameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(nameEdited(const QString&)));
        layout->addWidget(nameEdit);

        valueEdit->setRange(-(pow(256, sizeof(double))/2), pow(256, sizeof(double))/2);
        valueEdit->setDecimals(12);
        valueEdit->setToolTip(tr("Value of the variable"));
        valueEdit->setValue(value);
        connect(valueEdit, SIGNAL(valueChanged(const double&)), this, SLOT(valueHasChanged(const double&)));
        layout->addWidget(valueEdit);

        layout->setSizeConstraint(QLayout::SetFixedSize);
        this->setLayout(layout);
        this->setMinimumWidth(minWidth);
    }

    QString varWidget::varName() const
    {return nameEdit->text();}
    calc::real varWidget::varValue() const
    {return valueEdit->value();}

    // Static:
        QString varWidget::checkName(const QString& name)
        {
            QString out="";
            for(QString::const_iterator pos=name.begin(); pos!=name.end(); pos++)
            {
                if((*pos=='_')||(*pos>='a' && *pos<='z')||(*pos>='A' && *pos<='Z')||(pos!=name.begin() && *pos>='0' && *pos<='9'))
                    out+=*pos;
            }
            if(out == "")
                out="noName";
            return out;
        }
    
// Public slots:
    void varWidget::rename(QString newName, const QString& forceOldName)
    {
        if(forceOldName != "")
            oldName = forceOldName;

        nameEdit->setText(newName = varWidget::checkName(newName));
        QString oldNameRemember = oldName;
        if((oldName=newName) != oldNameRemember)
            renamed(oldNameRemember, newName, this);
    }

    void varWidget::changeValue(const calc::real& newValue)
    {valueEdit->setValue(newValue);}

// Private slots:
    void varWidget::nameEdited(const QString& newName)
    { rename(newName); }

    void varWidget::valueHasChanged(const double& newValue)
    {valueChanged(nameEdit->text(), newValue);}
