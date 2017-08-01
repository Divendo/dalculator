#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QCloseEvent>
#include <QFile>
#include <QKeySequence>
#include <QDir>

// Public:
    MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      errorMessageTimer(0), errorMessageOpacity(0),
      myVarsFuncsDialog(0),
      myUpdateChecker(updates::version(2, 1, 1, 4, false)), updateWindow(0),
      aboutDialog(0),
      englishTranslator(0), dutchTranslator(0),
      historyDialog(100, this)
    {
        // Set the right value for the data directory
#ifdef Q_WS_X11
        dataDir = "/usr/share/dalculator/data";
#else
        dataDir = QDir::currentPath();
#endif

        // Set up UI:
            ui->setupUi(this);

            // Menu:
                ui->actionExit->setShortcut(QKeySequence::Quit);
                ui->actionHelp->setShortcut(QKeySequence::HelpContents);
                ui->actionRecalculate->setShortcut(QKeySequence::Refresh);
                ui->actionPrevious_calculation->setShortcut(QKeySequence("Up"));
                ui->actionNext_calculation->setShortcut(QKeySequence("Down"));
                ui->actionShow_history->setShortcut(QKeySequence("Ctrl+H"));
                connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

            // Buttons:
                connect(ui->buttonOk, SIGNAL(clicked()), this, SLOT(calculate()));

            // Calculations history
                connect(ui->actionPrevious_calculation, SIGNAL(triggered()), this, SLOT(previousExpression()));
                connect(ui->actionNext_calculation, SIGNAL(triggered()), this, SLOT(nextExpression()));
                connect(ui->actionRecalculate, SIGNAL(triggered()), this, SIGNAL(recalculate()));
                connect(ui->calcInput, SIGNAL(textEdited(const QString&)), &historyDialog, SLOT(resetPos()));
                connect(&historyDialog, SIGNAL(useExpression(const QString&)), this, SLOT(setExpression(const QString&)));
                connect(&historyDialog, SIGNAL(cleared()), this, SLOT(calcHistoryCleared()));

            // Message displays:
                connect(ui->errorMessage, SIGNAL(clicked()), this, SLOT(closeErrorMessage()));

        // Connect update checker:
                connect(&myUpdateChecker, SIGNAL(updatesChecked(const bool&)), this, SLOT(updatesChecked(const bool&)));
                connect(&myUpdateChecker, SIGNAL(errorOccurred(const bool&)), this, SLOT(updateErrorOccurred(const bool&)));

        // Connect calculator:
            connect(this, SIGNAL(calcString(const QString&)), &calculator, SLOT(calculate(const QString&)));
            connect(&calculator, SIGNAL(result(const QString&, const bool&)), this, SLOT(calculated(const QString&, const bool&)));
            connect(this, SIGNAL(saveCalculator()), &calculator, SLOT(saveSettings()));
            connect(this, SIGNAL(loadCalculator()), &calculator, SLOT(loadSettings()));
            connect(&calculator, SIGNAL(error(const QString&)), this, SLOT(settingLoadError(const QString&)));
            connect(this, SIGNAL(recalculate()), &calculator, SLOT(recalculate()));

        // Initialise the calculator and all settings
            initialise();
    }

    MainWindow::~MainWindow()
    {
        // Only delete the objects that are created
        if(errorMessageTimer != 0)
            delete errorMessageTimer;
        if(myVarsFuncsDialog != 0)
            delete myVarsFuncsDialog;
        if(updateWindow != 0)
            delete updateWindow;
        if(aboutDialog != 0)
            delete aboutDialog;
        if(englishTranslator != 0)
            delete englishTranslator;
        if(dutchTranslator != 0)
            delete dutchTranslator;

        delete ui;
    }

// Public slots:
    void MainWindow::calculate()
    {
        // Close the error message in case one was shown
        closeErrorMessage();
        // Add the current expression to the history
        historyDialog.add(ui->calcInput->text());
        // Give the command to calculate the expression
        calcString(ui->calcInput->text());
        // Enable recalculation
        ui->actionRecalculate->setEnabled(true);
        // Enable navigating through history if there is more than 1 record in the history, otherwise disable navigating through history
        ui->actionPrevious_calculation->setEnabled(historyDialog.size() - 1);
        ui->actionNext_calculation->setEnabled(historyDialog.size() - 1);
    }

    void MainWindow::calculated(const QString& msg, const bool& errorOccurred)
    {
        // If an error occurred, we display the error
        if(errorOccurred)
            displayErrorMessage(msg);
        // If no error occurred we display the answer and select it
        else
        {
            ui->calcInput->setText(msg);
            ui->calcInput->setSelection(msg.length(), -msg.length());
        }

        // If the dialog containing the variables and function is created, reload the variables and functions since a variable may have changed by the expression
        if(myVarsFuncsDialog)
            myVarsFuncsDialog->reload();
    }

    void MainWindow::displayErrorMessage(const QString& msg)
    {
        // Set the error text
        ui->errorMessage->setText(msg);
        ui->errorMessage->setStyleSheet("color:rgba(255,0,0,0%);font-weight:bold;width:100%;");
        // Set the opacity to 0, to create a nice fade-in effect
        errorMessageOpacity = 0;
        // Enable the button containing the error message
        ui->errorMessage->setEnabled(true);

        // If there was already an error message timer we delete it
        if(errorMessageTimer)
            delete errorMessageTimer;

        // Create a new error message timer, to time the fading, connect the right slot and start timing
        errorMessageTimer = new QTimer(this);
        connect(errorMessageTimer, SIGNAL(timeout()), this, SLOT(errorMessageTimerEnded()));
        errorMessageTimer->start(75);
    }

// Protected:
    void MainWindow::changeEvent(QEvent* e)
    {
        // In case the language changes, the ui needs to be retranslated
        QMainWindow::changeEvent(e);
        switch(e->type())
        {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
            break;

            default:
            break;
        }
    }

// Private:

    void MainWindow::initialise()
    {
        // Load the variables and functions
            loadCalculator();

        // Load the settings
            try
            {
                // Load Settings
                if(QFile::exists(QDir::currentPath()+"/settings.ini"))
                    settings.loadFromFile((QDir::currentPath()+"/settings.ini").toStdString());

                // Recover any corrupted settings:
                if(!settings["main"].valueExists("language"))
                    settings["main"]["language"]="en";
                if(!settings["main"]["autoCheckForUpdates"].validateType(dini::typeBool))
                    settings["main"]["autoCheckForUpdates"]=true;
                if(!settings["history"]["history_size"].validateType(dini::typeInt))
                    historyDialog.clear();

                ui->actionAutocheck_for_updates->setChecked(settings["main"]["autoCheckForUpdates"].toBool());

                if(settings["main"]["language"].toString()=="nl")
                    on_actionDutch_triggered();
                if(settings["main"]["autoCheckForUpdates"].toBool()==true)
                    ui->actionCheck_for_updates->trigger();
                historyDialog.fromDiniSection(settings["history"]);
                if(historyDialog.size()>0)
                {
                    ui->actionPrevious_calculation->setEnabled(true);
                    ui->actionNext_calculation->setEnabled(true);
                }

                if(!settings["calculator"].valueExists("outputType"))
                    settings["calculator"]["outputType"]="auto";
                if(settings["calculator"]["outputType"].toString()=="auto")
                    ui->action_Auto_detect->trigger();
                else if(settings["calculator"]["outputType"].toString()=="time")
                    ui->actionTime->trigger();
                else if(settings["calculator"]["outputType"].toString()=="scientific")
                    ui->actionScientific->trigger();
                else if(settings["calculator"]["outputType"].toString()=="hex")
                    ui->actionHexadecimal->trigger();
                else if(settings["calculator"]["outputType"].toString()=="bin")
                    ui->actionBinary->trigger();
                else if(settings["calculator"]["outputType"].toString()=="oct")
                    ui->actionOctal->trigger();
                else if(settings["calculator"]["outputType"].toString()=="dec")
                    ui->actionDecimal->trigger();

                if(!settings["calculator"].valueExists("angleType"))
                    settings["calculator"]["angleType"]="rad";
                if(settings["calculator"]["angleType"].toString()=="deg")
                    ui->action_Degrees->trigger();
                else
                    ui->action_Radians->trigger();
            }
            catch(...)
            {}
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        try
        {
            // Before closing make sure the settings of the calculator and the settings of the application gets saved
            saveCalculator();
            settings["history"] = historyDialog.toDiniSection("history");
            settings.saveToFile((QDir::currentPath()+"/settings.ini").toStdString());
        }
        catch(...)
        {}
        event->accept();
    }

    void MainWindow::checkOutputType()
    {
        // Determine which output type should be checked in the menu, based on the name of the output type
        ui->action_Auto_detect->setChecked(settings["calculator"]["outputType"].toString() == "auto");
        ui->actionTime->setChecked(settings["calculator"]["outputType"].toString() == "time");
        ui->actionScientific->setChecked(settings["calculator"]["outputType"].toString() == "scientific");
        ui->actionHexadecimal->setChecked(settings["calculator"]["outputType"].toString() == "hex");
        ui->actionBinary->setChecked(settings["calculator"]["outputType"].toString() == "bin");
        ui->actionOctal->setChecked(settings["calculator"]["outputType"].toString() == "oct");
        ui->actionDecimal->setChecked(settings["calculator"]["outputType"].toString() == "dec");
    }

// Private slots:
    void MainWindow::errorMessageTimerEnded()
    {
        // Make the error message fade in a bit more
        ui->errorMessage->setStyleSheet(QString("color:rgba(255,0,0,%1%);font-weight:bold;").arg(std::min(int(errorMessageOpacity+=10), 100)));
        // When the opacity is 100 or greater we stop the timer and make sure the opacity is set to 100
        if(errorMessageOpacity >= 100)
        {
            delete errorMessageTimer;
            errorMessageTimer = 0;
            errorMessageOpacity = 100;
        }
    }
    void MainWindow::errorMessageCloseTimerEnded()
    {
        // Make the error message fade out a bit more
        ui->errorMessage->setStyleSheet(QString("color:rgba(255,0,0,%1%);font-weight:bold;").arg(std::max(int(errorMessageOpacity-=10), 0)));
        // When the opacity is 0 or smaller we clear the error message, stop the timer and make sure the opacity is set to 0
        if(errorMessageOpacity <= 0)
        {
            ui->errorMessage->setText("");
            delete errorMessageTimer;
            errorMessageTimer = 0;
            errorMessageOpacity = 0;
        }
    }

    void MainWindow::closeErrorMessage()
    {
        // Delete the timer, if one was in use
        if(errorMessageTimer)
            delete errorMessageTimer;
        // Create a new timer and connect the right slot to it, then start it
        errorMessageTimer = new QTimer(this);
        connect(errorMessageTimer, SIGNAL(timeout()), this, SLOT(errorMessageCloseTimerEnded()));
        errorMessageTimer->start(75);
        // Disable the button containing the error message
        ui->errorMessage->setEnabled(false);
    }


    void MainWindow::varsFuncsDialogClosed()
    { ui->showVarsFuncs->setEnabled(true); }

    void MainWindow::updatesChecked(const bool& newVersionAvailable)
    {
        // If an update is available and there isn't already an update window open, create an update window and show it
        if(newVersionAvailable && !updateWindow)
        {
            updateWindow = new QMessageBox( QMessageBox::Information,
                                            tr("New version available"),
                                            tr("A new version of Dalculator is available.\nWould you like to go to the download page to download and install the new version?"),
                                            QMessageBox::Yes | QMessageBox::No,
                                            this);
            updateWindow->show();
            connect(updateWindow, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateWindowButtonClicked(QAbstractButton*)));
        }
        ui->actionCheck_for_updates->setEnabled(true);
    }
    void MainWindow::updateWindowButtonClicked(QAbstractButton* button)
    {
        // If the yes-button was clicked, open the webpage where the update can be downloaded
        if(updateWindow->buttonRole(button) == QMessageBox::YesRole)
            QDesktopServices::openUrl(myUpdateChecker.getSoftwarePage());

        // Schedule the update window for deletion
        updateWindow->deleteLater();
        updateWindow = 0;
    }
    void MainWindow::updateErrorOccurred(const bool& httpError)
    {
        // Stop if an update window is already shown, because we don't want multiple update windows
        if(updateWindow)
            return;
        // Generate the right error message
        if(httpError)
        {
            updateWindow = new QMessageBox( QMessageBox::Warning,
                                            tr("Update HTTP Error"),
                                            tr("Dalculator couldn't retrieve the latest version number from the internet.\n"
                                            "Make sure you have an internet connection and your firewall isn't blocking Dalculator.\n"
                                            "Do you want to check for a new version manually?\n"
                                            "You can find your current version by clicking Help > About..."),
                                            QMessageBox::Yes | QMessageBox::No,
                                            this);
        }
        else
        {
            updateWindow = new QMessageBox( QMessageBox::Warning,
                                            tr("Update unknown error"),
                                            tr("An unknown error occurred while checking for updates.\n"
                                            "Would you like to check for a new version manually?\n"
                                            "You can find your current version by clicking Help > About..."),
                                            QMessageBox::Yes | QMessageBox::No,
                                            this);
        }
        // Show the update window and connect the right slot
        updateWindow->show();
        connect(updateWindow, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateWindowButtonClicked(QAbstractButton*)));
        ui->actionCheck_for_updates->setEnabled(true);
    }

    void MainWindow::settingLoadError(const QString& msg)
    { QMessageBox::critical(this, tr("Error"), msg); }

    void MainWindow::previousExpression()
    {
        // Go one place back in history (if possible) and change the current expression
        if(historyDialog.older(ui->calcInput->text()))
            ui->calcInput->setText(historyDialog.get());
    }

    void MainWindow::nextExpression()
    {
        // Go one place forward in history (if possible) and change the current expression
        if(historyDialog.newer(ui->calcInput->text()))
            ui->calcInput->setText(historyDialog.get());
    }

    void MainWindow::calcHistoryCleared()
    {
        // Disable history navigation, for there is no history
        ui->actionPrevious_calculation->setEnabled(false);
        ui->actionNext_calculation->setEnabled(false);
    }

    void MainWindow::setExpression(const QString& newExpr)
    { ui->calcInput->setText(newExpr); }

    // Mainwindow.ui:
        void MainWindow::on_showVarsFuncs_clicked()
        {
            // If the dialog isn't created yet, we create it
            if(!myVarsFuncsDialog)
            {
                myVarsFuncsDialog=new varsFuncsDialog(&calculator, this);
                connect(myVarsFuncsDialog, SIGNAL(accepted()), this, SLOT(varsFuncsDialogClosed()));
                connect(myVarsFuncsDialog, SIGNAL(rejected()), this, SLOT(varsFuncsDialogClosed()));
            }
            // Show the dialog and disable the button for as long as it's shown
            ui->showVarsFuncs->setEnabled(false);
            myVarsFuncsDialog->show();
        }

        void MainWindow::on_actionCheck_for_updates_triggered()
        {
            // Disable the check for updates menu button and check for updates
            ui->actionCheck_for_updates->setEnabled(false);
            myUpdateChecker.checkForUpdates();
        }

        void MainWindow::on_actionAbout_triggered()
        {
            // If the dialog isn't created yet, we create it
            if(!aboutDialog)
                aboutDialog = new dialogAbout(this);
            // Show the dialog
            aboutDialog->show();
        }

        void MainWindow::on_actionAutocheck_for_updates_triggered(const bool& checked)
        { settings["main"]["autoCheckForUpdates"] = checked; }

        void MainWindow::on_actionEnglish_triggered()
        {
            // Get the right font and make the right menu button bold
            QFont font = ui->actionEnglish->font();
            font.setBold(true);
            ui->actionEnglish->setFont(font);
            font.setBold(false);
            ui->actionDutch->setFont(font);

            // If the English translator isn't loaded yet, then load it
            if(!englishTranslator)
            {
                englishTranslator = new QTranslator();
                englishTranslator->load("lang_en", dataDir);
            }

            // Use the English translator
            qApp->installTranslator(englishTranslator);

            // If the Dutch translator is being used, remove it
            if(dutchTranslator)
                qApp->removeTranslator(dutchTranslator);

            // Remember the current language
            settings["main"]["language"]="en";
        }

        void MainWindow::on_actionDutch_triggered()
        {
            // Get the right font and make the right menu button bold
            QFont font=ui->actionDutch->font();
            font.setBold(true);
            ui->actionDutch->setFont(font);
            font.setBold(false);
            ui->actionEnglish->setFont(font);

            // If the Dutch translator isn't loaded yet, then load it
            if(!dutchTranslator)
            {
                dutchTranslator = new QTranslator();
                dutchTranslator->load("lang_nl", dataDir);
            }

            // Use the Dutch translator
            qApp->installTranslator(dutchTranslator);

            // If the English translator is being used, remove it
            if(englishTranslator)
                qApp->removeTranslator(englishTranslator);

            // Remember the current language
            settings["main"]["language"]="nl";
        }

        void MainWindow::on_actionHelp_triggered()
        { QDesktopServices::openUrl(QUrl("file:///"+dataDir+"/help/index.html", QUrl::TolerantMode)); }

        void MainWindow::on_action_Auto_detect_triggered()
        {
            calculator.setOutputType(QTCalc::outputAutoDetect);
            settings["calculator"]["outputType"]="auto";
            checkOutputType();
        }

        void MainWindow::on_actionTime_triggered()
        {
            calculator.setOutputType(QTCalc::outputTime);
            settings["calculator"]["outputType"]="time";
            checkOutputType();
        }

        void MainWindow::on_actionDecimal_triggered()
        {
            calculator.setOutputType(QTCalc::outputDec);
            settings["calculator"]["outputType"]="dec";
            checkOutputType();
        }

        void MainWindow::on_actionOctal_triggered()
        {
            calculator.setOutputType(QTCalc::outputOct);
            settings["calculator"]["outputType"]="oct";
            checkOutputType();
        }

        void MainWindow::on_actionBinary_triggered()
        {
            calculator.setOutputType(QTCalc::outputBin);
            settings["calculator"]["outputType"]="bin";
            checkOutputType();
        }

        void MainWindow::on_actionHexadecimal_triggered()
        {
            calculator.setOutputType(QTCalc::outputHex);
            settings["calculator"]["outputType"]="hex";
            checkOutputType();
        }

        void MainWindow::on_actionScientific_triggered()
        {
            calculator.setOutputType(QTCalc::outputScientific);
            settings["calculator"]["outputType"]="scientific";
            checkOutputType();
        }

        void MainWindow::on_action_Degrees_triggered()
        {
            ui->action_Degrees->setChecked(true);
            ui->action_Radians->setChecked(false);

            calculator.setAngleType(QTCalc::angleDegrees);
            settings["calculator"]["angleType"]="deg";
        }

        void MainWindow::on_action_Radians_triggered()
        {
            ui->action_Degrees->setChecked(false);
            ui->action_Radians->setChecked(true);

            calculator.setAngleType(QTCalc::angleRadians);
            settings["calculator"]["angleType"]="rad";
        }

        void MainWindow::on_actionShow_history_triggered()
        { historyDialog.show(); }
