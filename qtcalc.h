/******************************* License - GPLv3 ********************************
* Dalculator, a simple calculator                                               *
* Copyright (C) 2011 Divendo                                                    *
*                                                                               *
* This program is free software: you can redistribute it and/or modify          *
* it under the terms of the GNU General Public License as published by          *
* the Free Software Foundation, either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This program is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
* GNU General Public License for more details.                                  *
*                                                                               *
* You should have received a copy of the GNU General Public License             *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.         *
********************************************************************************/


#ifndef QTCALC_H
#define QTCALC_H

#include <QObject>
#include <QTimer>
#include <map>
#include "calc/calc.h"

// Class that makes the calculator engine interact with the GUI
class QTCalc : public QObject
{
    Q_OBJECT

    public:
        // Enum that's used to identify the output type
        enum outputType {outputAutoDetect, outputBin, outputOct, outputDec, outputHex, outputScientific, outputTime};
        // Enum that's used to identify the type of the angles
        enum angleType {angleDegrees, angleRadians};

        // Constructor
        QTCalc(const outputType& calcOutputType = outputAutoDetect, const angleType& angle = angleRadians);
        // Destructor
        ~QTCalc();

        // Get a map containing all the functions and their expressions
        std::map<QString, QString> getFuncs() const throw();
        // Get a map containing all variables and their values
        std::map<QString, calc::real> getVars() const throw();

        // Set the location of the settings file
        void setSettingFilename(const QString& filename);

        // Get the output type
        outputType getOutputType() const;
        // Get the angle type
        angleType getAngleType() const;

    public slots:
        // Calculate the given expresion
        void calculate(const QString& expr);
        // Recalculate the last expression
        void recalculate();

        // Add a variable with the given name and value
        void setVar(const QString& name, const calc::real& value);
        // Rename a variable
        void renameVar(const QString& oldName, const QString& newName);
        // Delete a variable
        void deleteVar(const QString& name);

        // Add a function with the given name and expression
        void setFunc(const QString& name, const QString& content);
        // Rename a function
        void renameFunc(const QString& oldName, const QString& newName);
        // Delete a function
        void deleteFunc(const QString& name);

        // Load the calculator settings (i.e. the variables and functions)
        void loadSettings();
        // Save the calculator settings (i.e. the variables and functions)
        void saveSettings();

        // Change the output type
        void setOutputType(const outputType& newType);
        // Change the angle type
        void setAngleType(const angleType& newType);

    signals:
        // Reports the result of a calculation
        void result(const QString& msg, const bool& errorOccurred);
        // Reports an error
        void error(const QString& msg);

    private slots:
        // Functions to handle a calculator error and display the right error
        void calcErrorOccurred(const calc::calcError& err);
        void calcErrorOccurred(const calc::overflowError& err);

    private:
        // Schedules the settings to be saved in a while
        void saveSettingsLater();

        // Calculator, the engine
        calc::calc calculator;

        // The filename of the settings file
        QString settingFilename;
        // The setting handler
        calc::settingHandler settingHandler;

        // The current output type
        outputType calcOutputType;
        // The current angle type
        angleType currAngleType;

        // Timer used to schedule saving the settings
        QTimer saveSettingsTimer;

        // A list of built-in variables
        calc::varList builtInVars;
        // A list of built-in functions
        calc::functionList builtInFunctions;

        // Some built-in functions, the names are self explaining
        // The current angle type is used for the type of angles
        inline calc::real cos(const calc::argList& args);
        inline calc::real cosh(const calc::argList& args);
        inline calc::real acos(const calc::argList& args);
        inline calc::real sin(const calc::argList& args);
        inline calc::real sinh(const calc::argList& args);
        inline calc::real asin(const calc::argList& args);
        inline calc::real tan(const calc::argList& args);
        inline calc::real tanh(const calc::argList& args);
        inline calc::real atan(const calc::argList& args);
        inline calc::real avg(const calc::argList& args);
        inline calc::real ncr(const calc::argList& args);
        inline calc::real npr(const calc::argList& args);
};

// Class to make member functions from the QTCalc class available as math functions for the calculator engine
class QTCalcMemberMathFunction : public calc::mathFunction
{
    public:
        // Typedef what a function is
        typedef calc::real (QTCalc::*function)(const calc::argList&);

        // Constructor
        QTCalcMemberMathFunction(function initFunction, QTCalc* obj, const bool& cleanUpNeeded = false);

        // Execute this function
        calc::real execute(const calc::argList& vars, const calc::string& name);

    private:
        // The current function to be executed by execute()
        function currFunc;

        // The object that should be used by execute() to execute the function on
        QTCalc* obj;
};

// Some constants
namespace mathConstant
{
    const double PI     = 3.1415926535897932385;
    const double E      = 2.718281828459;
    const double PHI    = 1.618033988749895;
}

// Some built-in functions
namespace mathFunctions
{
    // Convert the number of degrees to radians
    inline double rad(double deg);
    // Convert the number of radians to degrees
    inline double deg(double rad);
    // Rounds the number to the nearest integer
    inline double round(double src);
    // Returns the faculty of n, in other words: n!
    inline double faculty(double n);

    // Returns a random argument
    // What numbers are possible depends on the number of arguments:
    //  0 =>    A random number in the range [0, 1] is returned
    //  1 =>    A random integer in the range [0, firstArgument] is returned
    //  2 =>    A random integer in the range [firstArgument, secondArgument] is returned
    calc::real random(const calc::argList& vars);

    // If the first argument is not 0, the second argument is returned
    // Otherwise the third argument is returned, the third argument defaults to zero
    calc::real ifFunction(const calc::argList& vars);
}

#endif // QTCALC_H
