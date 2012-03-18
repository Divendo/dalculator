#ifndef VARSWIDGET_H
#define VARSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include <map>
#include <vector>

#include "varwidget.h"
#include "calc/calc.h"

class varsWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit varsWidget(const std::map<QString, calc::real> initVars=std::map<QString, calc::real>(), QWidget *parent = 0);

    public slots:
        void setVars(const std::map<QString, calc::real>& newVars);

    signals:
        void varAdded(const QString& name, const calc::real& value);
        void varRenamed(const QString& oldName, const QString& newName);
        void varValueChanged(const QString& name, const calc::real& newValue);

    protected:
        void changeEvent(QEvent *e);

    private slots:
        void addVariable();
        void clearLayout();

        void changeName(const QString& oldName, const QString& newName, varWidget* owner);

    private:
        QString findName(const QString& baseName, bool mayExistOnce=false);

        QVBoxLayout* layout;
        QPushButton* buttonAddVar;

        typedef std::vector<varWidget*> varWidgetList;
        varWidgetList varWidgets;
};

#endif // VARSWIDGET_H
