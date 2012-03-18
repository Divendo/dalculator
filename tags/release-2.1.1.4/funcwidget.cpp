#include "funcwidget.h"
#include <QHBoxLayout>
#include <limits>
#include <QEvent>

// Public:
    funcWidget::funcWidget(const QString& name, const QString& content, QWidget *parent, const int& minWidth) :
    QWidget(parent), nameEdit(new QLineEdit(name, this)), contentEdit(new QLineEdit(content, this)), oldName(name)
    {
        QHBoxLayout* layout=new QHBoxLayout(this);

        nameEdit->setToolTip(tr("The name of the function"));
        connect(nameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(nameEdited(const QString&)));
        layout->addWidget(nameEdit, 1);

        contentEdit->setToolTip(tr("What the function does"));
        connect(contentEdit, SIGNAL(textChanged(const QString&)), this, SLOT(contentHasChanged(const QString&)));
        layout->addWidget(contentEdit, 2);

        layout->setSizeConstraint(QLayout::SetFixedSize);
        this->setLayout(layout);
        this->setMinimumWidth(minWidth);
    }

    QString funcWidget::funcName() const
    {return nameEdit->text();}
    QString funcWidget::funcContent() const
    {return contentEdit->text();}

    // Static:
        QString funcWidget::checkName(const QString& name)
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
    void funcWidget::rename(QString newName, const QString& forceOldName)
    {
        if(forceOldName != "")
            oldName = forceOldName;

        nameEdit->setText(newName = funcWidget::checkName(newName));
        QString oldNameRemember = oldName;
        if((oldName=newName) != oldNameRemember)
            renamed(oldNameRemember, newName, this);
    }

    void funcWidget::changeContent(const QString& newContent)
    { contentEdit->setText(newContent); }

// Protected:
    void funcWidget::changeEvent(QEvent* e)
    {
        QWidget::changeEvent(e);
        switch (e->type())
        {
            case QEvent::LanguageChange:
                nameEdit->setToolTip(tr("The name of the function"));
                contentEdit->setToolTip(tr("What the function does"));
            break;

            default:
            break;
        }
    }

// Private slots:
    void funcWidget::nameEdited(const QString& newName)
    { rename(newName); }

    void funcWidget::contentHasChanged(const QString& newContent)
    { contentChanged(nameEdit->text(), newContent); }
