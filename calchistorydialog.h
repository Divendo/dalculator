#ifndef CALCHISTORYDIALOG_H
#define CALCHISTORYDIALOG_H

#include <QDialog>
#include <vector>
#include <QString>
#include <QShortcut>
#include "dini/dini.h"

namespace Ui { class calcHistoryDialog; }

class calcHistoryDialog : public QDialog
{
    Q_OBJECT

    public:
        // Constructors and destructor
        calcHistoryDialog(const int& maxSize = 100, QWidget* parent = 0);
        calcHistoryDialog(dini::iniSection& section, const int& maxSize = 100, QWidget* parent = 0);
        ~calcHistoryDialog();

        // Add an expression
        void add(const QString& str);
        // Get the current expression
        QString get() const;
        // Get the history size
        int size() const;

        // Initialise the history from a dini::iniSection
        void fromDiniSection(const dini::iniSection& section);
        // Returns the history as a dini::iniSection
        dini::iniSection toDiniSection(const std::string& name) const;

    public slots:
        // Clear the history
        void clear();
        // Reset the current history position
        void resetPos();
        // Go one place newer in the history, return if it was possible
        bool newer(const QString& currExpr);
        // Go one place older in the history, return if it was possible
        bool older(const QString& currExpr);

    signals:
        // Command to use the given expression (i.e. it was selected by the user)
        void useExpression(const QString& expr);
        // Indicates that the history was cleared
        void cleared();

    protected:
        // To handle translate events
        void changeEvent(QEvent* e);

    private:
        // Adds the given expression to the history, this function will also erase the oldest expression if the history exceeds it's maximum size
        void addPrivate(const QString& str);

        Ui::calcHistoryDialog* ui;                                              // The actual ui of the dialog

        int currPos;                                                            // The current position in the history
        int maxSize;                                                            // The maximum size of the history
        std::vector<QString> history;                                           // The actual history
        QShortcut goUp;                                                         // Keyboard shortcut to go one place up (i.e. older) in history
        QShortcut goDown;                                                       // Keyboard shortcut to go one place down (i.e. newer) in history

    private slots:
        void on_buttonCurrExpr_clicked();
        void on_buttonClearHistory_clicked();
        void on_buttonRemove_clicked();
        void on_listHistory_itemSelectionChanged();
        void on_buttonUse_clicked();
        void on_buttonClose_clicked();

        void goUpActivated();
        void goDownActivated();
};

#endif // CALCHISTORYDIALOG_H
