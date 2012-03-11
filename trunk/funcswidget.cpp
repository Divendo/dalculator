#include "funcswidget.h"
#include <QVBoxLayout>
#include <QEvent>

// Public:
    funcsWidget::funcsWidget(const std::map<QString, QString>& initFuncs, QWidget *parent) :
    QWidget(parent), layout(new QVBoxLayout(this)), buttonAddFunction(new QPushButton(QIcon(":/icons/plus.png"), tr("Add function"), this))
    {
        connect(buttonAddFunction, SIGNAL(clicked()), this, SLOT(addFunction()));
        layout->addWidget(buttonAddFunction);
        layout->addStretch();
        this->setLayout(layout);

        this->setFuncs(initFuncs);
    }

    funcsWidget::~funcsWidget()
    {
        for(funcWidgetList::const_iterator pos=funcWidgets.begin(); pos!=funcWidgets.end(); pos++)
            delete *pos;
    }

// Public slots:
    void funcsWidget::setFuncs(const std::map<QString, QString>& newFuncs)
    {
        clearLayout();
        for(std::map<QString, QString>::const_iterator pos=newFuncs.begin(); pos!=newFuncs.end(); pos++)
        {
            funcWidgets.push_back(new funcWidget(pos->first, pos->second, this, this->width()));
            layout->insertWidget(layout->count()-2, funcWidgets.back());

            connect(funcWidgets.back(), SIGNAL(renamed(const QString&, const QString&, funcWidget*)), this, SLOT(changeName(const QString&, const QString&, funcWidget*)));
            connect(funcWidgets.back(), SIGNAL(contentChanged(const QString&, const QString&)), this, SIGNAL(funcContentChanged(const QString&, const QString&)));
        }
    }

// Protected:
    void funcsWidget::changeEvent(QEvent *e)
    {
        QWidget::changeEvent(e);
        switch (e->type())
        {
            case QEvent::LanguageChange:
                buttonAddFunction->setText(tr("Add function"));
            break;

            default:
            break;
        }
    }

// Private slots:
    void funcsWidget::addFunction()
    {
        const QString newName = findName("newFunc");
        funcWidgets.push_back(new funcWidget(newName, "ARG0+ARG1", this, this->width()));
        layout->insertWidget(layout->count()-2, funcWidgets.back());

        connect(funcWidgets.back(), SIGNAL(renamed(const QString&, const QString&, funcWidget*)), this, SLOT(changeName(const QString&, const QString&, funcWidget*)));
        connect(funcWidgets.back(), SIGNAL(contentChanged(const QString&, const QString&)), this, SIGNAL(funcContentChanged(const QString&, const QString&)));

        funcAdded(newName, "ARG0+ARG1");
    }

    void funcsWidget::clearLayout()
    {
        for(funcWidgetList::const_iterator pos=funcWidgets.begin(); pos!=funcWidgets.end(); pos++)
            delete *pos;
        funcWidgets.clear();
    }


    void funcsWidget::changeName(const QString& oldName, const QString& newName, funcWidget* owner)
    {
        QString newNewName = findName(newName, true);
        if(newNewName != newName)
            owner->rename(newNewName, oldName);
        else
            funcRenamed(oldName, newName);
    }

// Private:
    QString funcsWidget::findName(const QString& baseName, bool mayExistOnce)
    {
        unsigned short count = 0;
        std::vector<unsigned short> numbersInUse;
        for(funcWidgetList::const_iterator pos=funcWidgets.begin(); pos!=funcWidgets.end(); pos++)
        {
            QString name = (*pos)->funcName();
            if(name==baseName)
            {
                if(mayExistOnce)
                    mayExistOnce=false;
                else
                    count=1;
                continue;
            }
            else if(name.startsWith(baseName))
            {
                unsigned short x = name.remove(0, baseName.size()).toUShort();
                if(x != 0)
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
                ++count;
            else
                break;
        }
        if(count != 0)
            return (baseName+QString::number(count));
        else
            return baseName;
    }

