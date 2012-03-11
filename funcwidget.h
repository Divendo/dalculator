#ifndef FUNCWIDGET_H
#define FUNCWIDGET_H

#include <QWidget>
#include <QLineEdit>

class funcWidget : public QWidget
{
    Q_OBJECT
    public:
        funcWidget(const QString& name, const QString& content, QWidget *parent = 0, const int& minWidth=350);

        QString funcName() const;
        QString funcContent() const;

        static QString checkName(const QString& name);

    public slots:
        void rename(QString newName, const QString& forceOldName = "");
        void changeContent(const QString& newContent);

    signals:
        void renamed(const QString& oldName, const QString& newName, funcWidget* self);
        void contentChanged(const QString& name, const QString& newContent);

    protected:
        void changeEvent(QEvent* e);

    private slots:
        void nameEdited(const QString& newName);
        void contentHasChanged(const QString& newContent);

    private:
        QLineEdit* nameEdit;
        QLineEdit* contentEdit;
        QString oldName;
};

#endif // FUNCWIDGET_H
