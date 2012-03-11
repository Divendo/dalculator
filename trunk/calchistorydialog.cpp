#include "calchistorydialog.h"
#include "ui_calchistorydialog.h"

#include <QShowEvent>
#include <QMessageBox>
#include <algorithm>

// Public:
    calcHistoryDialog::calcHistoryDialog(const int& maxSize, QWidget* parent)
    : QDialog(parent), ui(new Ui::calcHistoryDialog),
    currPos(-1), maxSize(maxSize),
    goUp(QKeySequence("Up"), this), goDown(QKeySequence("Down"), this)
    {
        // Setup the ui
        ui->setupUi(this);
        // Reserve enough space for the history
        history.reserve(maxSize);

        // Connect the shortcut keys
        connect(&goUp, SIGNAL(activated()), this, SLOT(goUpActivated()));
        connect(&goDown, SIGNAL(activated()), this, SLOT(goDownActivated()));
    }

    calcHistoryDialog::calcHistoryDialog(dini::iniSection& section, const int& maxSize, QWidget* parent)
    : QDialog(parent), ui(new Ui::calcHistoryDialog),
    currPos(-1), maxSize(maxSize),
    goUp(QKeySequence("Up"), this), goDown(QKeySequence("Down"), this)
    {
        // Setup the ui
        ui->setupUi(this);
        // Reserve enough space for the history
        history.reserve(maxSize);
        // Initialise the history with the data from the dini::iniSection
        if(section["history_size"].validateType(dini::typeInt))
        {
            const int historySize = section["history_size"].toInt();
            for(int i = 0; i < historySize && i < maxSize; ++i)
            {
                history.push_back(section["history"+dini::intToString(i)].toString().c_str());
                ui->listHistory->addItem(history.back());
            }
        }

        // Connect the shortcut keys
        connect(&goUp, SIGNAL(activated()), this, SLOT(goUpActivated()));
        connect(&goDown, SIGNAL(activated()), this, SLOT(goDownActivated()));
    }

    calcHistoryDialog::~calcHistoryDialog()
    {
        delete ui;
    }

    void calcHistoryDialog::add(const QString& str)
    {
        // Add the expression and reset the position
        addPrivate(str);
        resetPos();
    }

    QString calcHistoryDialog::get() const
    {
        // In case no expression is selected, we return an empty string
        if(currPos == -1)
            return "";
        // In case the current expression is selected, we return it
        else if(currPos == static_cast<int>(history.size()))
            return ui->buttonCurrExpr->text();
        // In case an expression in the history is selected, we return it
        else
            return history[currPos];
    }
    int calcHistoryDialog::size() const
    { return history.size(); }

    void calcHistoryDialog::fromDiniSection(const dini::iniSection& section)
    {
        // Clear the current history
        clear();
        if(section["history_size"].validateType(dini::typeInt))
        {
            // Read the size of the history
            const int historySize = section["history_size"].toInt();
            // Put every expression from the iniSection in the history, as long as we're not exceeding the maximum
            for(int i = 0; i < historySize && i < maxSize; ++i)
            {
                history.push_back(section["history"+dini::intToString(i)].toString().c_str());
                ui->listHistory->addItem(history.back());
            }
        }
    }
    dini::iniSection calcHistoryDialog::toDiniSection(const std::string& name) const
    {
        // Create an iniSection
        dini::iniSection out(name);
        // Set the size
        out["history_size"] = static_cast<int>(history.size());
        // Loop through the history and add every expression
        for(unsigned int i = 0; i < history.size(); ++i)
            out["history"+dini::doubleToString(i)] = history[i].toStdString();
        // Return the iniSection
        return out;
    }

// Public slots:
    void calcHistoryDialog::clear()
    {
        // Clear the actual history
        history.clear();
        // Clear the history list
        ui->listHistory->clear();
        // Reset the position (also clears the current expression) and notify that the history is cleared
        resetPos();
        cleared();
    }

    void calcHistoryDialog::resetPos()
    {
        // Reset the position
        ui->listHistory->setCurrentRow(currPos = -1);

        // Clear the current expression
        ui->buttonCurrExpr->setText("");
        ui->buttonCurrExpr->setEnabled(false);
        ui->buttonCurrExpr->setChecked(false);
    }

    bool calcHistoryDialog::newer(const QString& currExpr)
    {
        // If the history isn't empty we can go newer
        if(history.empty())
            return false;

        // Only set the current expression if we aren't browsing the history already
        // Then go to the next expression
        if(currPos == -1)
        {
            ui->buttonCurrExpr->setText(currExpr);
            ui->buttonCurrExpr->setEnabled(true);
            currPos = 0;
        }
        // Only go back to the end of the history if the next possition is greater then the history size
        else if(++currPos > static_cast<int>(history.size()))
            currPos = 0;

        // Select the right list item
        if(currPos == static_cast<int>(history.size()))
            ui->buttonCurrExpr->setChecked(true);
        ui->listHistory->setCurrentRow(currPos);
        return true;
    }

    bool calcHistoryDialog::older(const QString& currExpr)
    {
        // If the history isn't empty we can go older
        if(history.empty())
            return false;

        // Only set the current expression if we aren't browsing the history already
        // Then go to the previous expression
        if(currPos == -1)
        {
            ui->buttonCurrExpr->setText(currExpr);
            ui->buttonCurrExpr->setEnabled(true);
            currPos = static_cast<int>(history.size())-1;
        }
        // Only go back to the start of the history (i.e. the current expression) if the next possition is smaller then 0
        else if(--currPos < 0)
            currPos = static_cast<int>(history.size());

        // Select the right list item
        if(currPos == static_cast<int>(history.size()))
            ui->buttonCurrExpr->setChecked(true);
        ui->listHistory->setCurrentRow(currPos);
        return true;
    }

// Protected:
    void calcHistoryDialog::changeEvent(QEvent* e)
    {
        // In case the language changes, the ui needs to be retranslated
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

// Private:
    void calcHistoryDialog::addPrivate(const QString& str)
    {
        // If the maximum size is reached already we erase the oldest record
        if(static_cast<int>(history.size()+1) > maxSize)
        {
            history.erase(history.begin());
            delete ui->listHistory->takeItem(0);
        }
        // Add the expression to the history and to the GUI-list
        history.push_back(str);
        ui->listHistory->addItem(str);
    }

// Private slots:
    void calcHistoryDialog::on_buttonClose_clicked()
    { this->accept(); }

    void calcHistoryDialog::on_buttonUse_clicked()
    {
        // If the current expression is selected, we give the command to use that expression
        if(currPos == static_cast<int>(history.size()))
            useExpression(ui->buttonCurrExpr->text());
        else
        {
            // Retrieve the currently selected item
            QListWidgetItem* currentItem = ui->listHistory->currentItem();
            // If an item was selected, give the command to use that expression
            if(currentItem)
                useExpression(currentItem->text());
        }
        // Close
        this->accept();
    }

    void calcHistoryDialog::on_listHistory_itemSelectionChanged()
    {
        // Only enable the use and remove buttons if an item is selected, or in case of the use button if the current expression is selected
        ui->buttonUse->setEnabled(ui->listHistory->selectedItems().size() || ui->buttonCurrExpr->isChecked());
        ui->buttonRemove->setEnabled(ui->listHistory->selectedItems().size());
        // When the selection changes, we also need to change the current position
        if(ui->listHistory->currentRow() != -1)
            currPos = ui->listHistory->currentRow();
        // Uncheck the current expression button if a valid item is selected
        ui->buttonCurrExpr->setChecked(!ui->listHistory->currentItem());
    }

    void calcHistoryDialog::on_buttonRemove_clicked()
    {
        // If there is currently an item selected, erase it and reset the position
        if(ui->listHistory->currentItem())
        {
            history.erase(history.begin() + ui->listHistory->currentRow());
            delete ui->listHistory->takeItem(ui->listHistory->currentRow());

            if(currPos == static_cast<int>(history.size()))
            {
                if(ui->buttonCurrExpr->isEnabled())
                    ui->buttonCurrExpr->setChecked(true);
                else if(history.size() == 0)
                    resetPos();
                else
                    currPos = history.size() - 1;
            }
            ui->listHistory->setCurrentRow(currPos);
        }
    }

    void calcHistoryDialog::on_buttonClearHistory_clicked()
    {
        // Ask for confirmation, if confirmed we clear the history
        if(QMessageBox::question(this, tr("Clear history"), tr("Are you sure you want to clear the history?\nThis can't be undone!"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            clear();
    }

    void calcHistoryDialog::on_buttonCurrExpr_clicked()
    {
        // Set the right position
        if(ui->buttonCurrExpr->isChecked())
            ui->listHistory->setCurrentRow( currPos = static_cast<int>(history.size()) );
        else
            ui->listHistory->setCurrentRow( currPos = static_cast<int>(history.size()-1) );
    }

    void calcHistoryDialog::goUpActivated()
    {
        // If the history isn't empty we can go older
        if(history.empty())
            return;

        // Go to an older expression
        // Only go back to the start of the history (i.e. the current expression) if the next possition is smaller then 0
        if(--currPos < 0)
            currPos = static_cast<int>(history.size());

        // Select the right list item
        if(currPos == static_cast<int>(history.size()))
        {
            if(ui->buttonCurrExpr->isEnabled())
                ui->buttonCurrExpr->setChecked(true);
            else
                currPos = static_cast<int>(history.size() - 1);
        }
        ui->listHistory->setCurrentRow(currPos);
    }

    void calcHistoryDialog::goDownActivated()
    {
        // If the history isn't empty we can go older
        if(history.empty())
            return;

        // Go to a newer expression
        // Only go back to the end of the history if the next possition is greater then the history size
        else if(++currPos > static_cast<int>(history.size()))
            currPos = 0;

        // Select the right list item
        if(currPos == static_cast<int>(history.size()))
        {
            if(ui->buttonCurrExpr->isEnabled())
                ui->buttonCurrExpr->setChecked(true);
            else
                currPos = 0;
        }
        ui->listHistory->setCurrentRow(currPos);
    }
