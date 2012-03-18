#include "varswidget.h"

#include <iostream>
#include <QEvent>

// Public:
    varsWidget::varsWidget(const std::map<QString, calc::real> initVars, QWidget *parent) :
    QWidget(parent), layout(new QVBoxLayout(this)), buttonAddVar(new QPushButton(QIcon(":/icons/plus.png"), tr("Add variable"), this))
    {
        connect(buttonAddVar, SIGNAL(clicked()), this, SLOT(addVariable()));
        layout->addWidget(buttonAddVar);
        layout->addStretch();
        this->setLayout(layout);

        this->setVars(initVars);
    }

// Public slots:
    void varsWidget::setVars(const std::map<QString, calc::real>& newVars)
    {
        clearLayout();
        for(std::map<QString, calc::real>::const_iterator pos=newVars.begin(); pos!=newVars.end(); pos++)
        {
            varWidgets.push_back(new varWidget(pos->first, pos->second, this, this->width()));
            layout->insertWidget(layout->count()-2, varWidgets.back());

            connect(varWidgets.back(), SIGNAL(renamed(const QString&, const QString&, varWidget*)), this, SLOT(changeName(const QString&, const QString&, varWidget*)));
            connect(varWidgets.back(), SIGNAL(valueChanged(const QString&, const calc::real&)), this, SIGNAL(varValueChanged(const QString&, const calc::real&)));
        }
    }

// Private slots:
    void varsWidget::addVariable()
    {
        const QString newName=findName("newVar");
        varWidgets.push_back(new varWidget(newName, 0, this, this->width()));
        layout->insertWidget(layout->count()-2, varWidgets.back());

        connect(varWidgets.back(), SIGNAL(renamed(const QString&, const QString&, varWidget*)), this, SLOT(changeName(const QString&, const QString&, varWidget*)));
        connect(varWidgets.back(), SIGNAL(valueChanged(const QString&, const calc::real&)), this, SIGNAL(varValueChanged(const QString&, const calc::real&)));

        varAdded(newName, 0);
    }

    void varsWidget::clearLayout()
    {
        for(varWidgetList::const_iterator pos=varWidgets.begin(); pos!=varWidgets.end(); pos++)
            delete *pos;
        varWidgets.clear();
    }

    void varsWidget::changeName(const QString& oldName, const QString& newName, varWidget* owner)
    {
        QString newNewName=findName(newName, true);
        if(newNewName!=newName)
            owner->rename(newNewName, oldName);
        else
            varRenamed(oldName, newName);
    }

// Protected:
    void varsWidget::changeEvent(QEvent *e)
    {
        QWidget::changeEvent(e);
        switch (e->type())
        {
            case QEvent::LanguageChange:
                buttonAddVar->setText(tr("Add variable"));
            break;
            default:
            break;
        }
    }

// Private:
    QString varsWidget::findName(const QString& baseName, bool mayExistOnce)
    {
        unsigned short count=0;
        std::vector<unsigned short> numbersInUse;
        for(varWidgetList::const_iterator pos=varWidgets.begin(); pos!=varWidgets.end(); pos++)
        {
            QString name=(*pos)->varName();
            if(name==baseName)
            {
                if(mayExistOnce)
                    mayExistOnce=false;
                else
                    count=1;
                continue;
            }
            if(name.startsWith(baseName))
            {
                unsigned short x=0;
                if((x=name.remove(0, baseName.size()).toUShort())!=0)
                {
                    if(numbersInUse.size()==0)
                        numbersInUse.push_back(x);
                    else if(x>numbersInUse.back())
                        numbersInUse.push_back(x);
                    else
                    {
                        for(std::vector<unsigned short>::iterator pos=numbersInUse.begin(); pos!=numbersInUse.end(); pos++)
                        {
                            if(x<*pos)
                            {
                                numbersInUse.insert(pos, x);
                                break;
                            }
                        }
                    }
                }
            }
        }
        for(std::vector<unsigned short>::const_iterator pos=numbersInUse.begin(); pos!=numbersInUse.end(); pos++)
        {
            if(*pos==count)
                count++;
            else
                break;
        }
        if(count!=0)
            return (baseName+QString::number(count));
        else
            return baseName;
    }
