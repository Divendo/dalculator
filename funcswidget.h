#ifndef FUNCSWIDGET_H
#define FUNCSWIDGET_H

#include <QVBoxLayout>
#include <QPushButton>
#include <map>
#include <vector>

#include "funcwidget.h"

class funcsWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit funcsWidget(const std::map<QString, QString>& initFuncs=std::map<QString, QString>(), QWidget *parent = 0);
        ~funcsWidget();

    public slots:
        void setFuncs(const std::map<QString, QString>& newFuncs);

    signals:
        void funcAdded(const QString& name, const QString& content);
        void funcRenamed(const QString& oldName, const QString& newName);
        void funcContentChanged(const QString& name, const QString& newContent);

    protected:
        void changeEvent(QEvent *e);

    private slots:
        void addFunction();
        void clearLayout();

        void changeName(const QString& oldName, const QString& newName, funcWidget* owner);

    private:
        QString findName(const QString& baseName, bool mayExistOnce=false);

        QVBoxLayout* layout;
        QPushButton* buttonAddFunction;

        typedef std::vector<funcWidget*> funcWidgetList;
        funcWidgetList funcWidgets;
};

#endif // FUNCSWIDGET_H
