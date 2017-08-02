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


#include "qtcalc.h"
#include <QFile>
#include <cmath>
#include <cstdlib>
#include <QDir>

// Class QTCalc:
    // Public:
        QTCalc::QTCalc(const outputType& calcOutputType, const angleType& angle)
        : settingFilename("calcsettings"),
        calcOutputType(calcOutputType), currAngleType(angle)
        {
            // Set the built-in functions, and store them in a calc::functionList
            // Set the names in both capital and non-capital form
            builtInFunctions["ABS"]     = new calc::cppMathFunction(std::abs, false);
            builtInFunctions["CEIL"]    = new calc::cppMathFunction(std::ceil, false);
            builtInFunctions["EXP"]     = new calc::cppMathFunction(std::exp, false);
            builtInFunctions["LOG"]     = new calc::cppMathFunction(std::log, false);
            builtInFunctions["LOG10"]   = new calc::cppMathFunction(std::log10, false);
            builtInFunctions["FLOOR"]   = new calc::cppMathFunction(std::floor, false);
            builtInFunctions["DEG"]     = new calc::cppMathFunction(mathFunctions::deg, false);
            builtInFunctions["RAD"]     = new calc::cppMathFunction(mathFunctions::rad, false);
            builtInFunctions["ROUND"]   = new calc::cppMathFunction(mathFunctions::round, false);
            builtInFunctions["FACULTY"] = new calc::cppMathFunction(mathFunctions::faculty, false);
            builtInFunctions["abs"]     = new calc::cppMathFunction(std::abs, false);
            builtInFunctions["ceil"]    = new calc::cppMathFunction(std::ceil, false);
            builtInFunctions["exp"]     = new calc::cppMathFunction(std::exp, false);
            builtInFunctions["log"]     = new calc::cppMathFunction(std::log, false);
            builtInFunctions["log10"]   = new calc::cppMathFunction(std::log10, false);
            builtInFunctions["floor"]   = new calc::cppMathFunction(std::floor, false);
            builtInFunctions["deg"]     = new calc::cppMathFunction(mathFunctions::deg, false);
            builtInFunctions["rad"]     = new calc::cppMathFunction(mathFunctions::rad, false);
            builtInFunctions["round"]   = new calc::cppMathFunction(mathFunctions::round, false);
            builtInFunctions["faculty"] = new calc::cppMathFunction(mathFunctions::faculty, false);


            builtInFunctions["COS"]     = new QTCalcMemberMathFunction(&QTCalc::cos, this, false);
            builtInFunctions["ACOS"]    = new QTCalcMemberMathFunction(&QTCalc::acos, this, false);
            builtInFunctions["COSH"]    = new QTCalcMemberMathFunction(&QTCalc::cosh, this, false);
            builtInFunctions["SIN"]     = new QTCalcMemberMathFunction(&QTCalc::sin, this, false);
            builtInFunctions["ASIN"]    = new QTCalcMemberMathFunction(&QTCalc::asin, this, false);
            builtInFunctions["SINH"]    = new QTCalcMemberMathFunction(&QTCalc::sinh, this, false);
            builtInFunctions["TAN"]     = new QTCalcMemberMathFunction(&QTCalc::tan, this, false);
            builtInFunctions["ATAN"]    = new QTCalcMemberMathFunction(&QTCalc::atan, this, false);
            builtInFunctions["TANH"]    = new QTCalcMemberMathFunction(&QTCalc::tanh, this, false);
            builtInFunctions["cos"]     = new QTCalcMemberMathFunction(&QTCalc::cos, this, false);
            builtInFunctions["acos"]    = new QTCalcMemberMathFunction(&QTCalc::acos, this, false);
            builtInFunctions["cosh"]    = new QTCalcMemberMathFunction(&QTCalc::cosh, this, false);
            builtInFunctions["sin"]     = new QTCalcMemberMathFunction(&QTCalc::sin, this, false);
            builtInFunctions["asin"]    = new QTCalcMemberMathFunction(&QTCalc::asin, this, false);
            builtInFunctions["sinh"]    = new QTCalcMemberMathFunction(&QTCalc::sinh, this, false);
            builtInFunctions["tan"]     = new QTCalcMemberMathFunction(&QTCalc::tan, this, false);
            builtInFunctions["atan"]    = new QTCalcMemberMathFunction(&QTCalc::atan, this, false);
            builtInFunctions["tanh"]    = new QTCalcMemberMathFunction(&QTCalc::tanh, this, false);

            builtInFunctions["RAND"]    = new calc::preDefinedMathFunction(mathFunctions::random, false);
            builtInFunctions["IF"]      = new calc::preDefinedMathFunction(mathFunctions::ifFunction, false);
            builtInFunctions["rand"]    = new calc::preDefinedMathFunction(mathFunctions::random, false);
            builtInFunctions["if"]      = new calc::preDefinedMathFunction(mathFunctions::ifFunction, false);

            // Add the built-in functions to the calculator
            for(calc::functionList::iterator pos = builtInFunctions.begin(); pos != builtInFunctions.end(); ++pos)
                calculator.setFunction(pos->first, pos->second);

            // Set the built in variables
            calculator.setVar("pi", mathConstant::PI);
            calculator.setVar("e", mathConstant::E);
            calculator.setVar("phi", mathConstant::PHI);

            // Remember the built-in variables
            builtInVars = *calculator.getVars();

            // Use a timer to save the settings of the calculator every now and then, this way the settings are always up-to-date but not saving constantly
            connect(&saveSettingsTimer, SIGNAL(timeout()), this, SLOT(saveSettings()));
            saveSettingsTimer.setInterval(5000);
            saveSettingsTimer.setSingleShot(true);
        }

        QTCalc::~QTCalc()
        {
            for(calc::functionList::iterator pos = builtInFunctions.begin(); pos != builtInFunctions.end(); ++pos)
                delete pos->second;
        }

        std::map<QString, QString> QTCalc::getFuncs() const throw()
        {
            // Get the function from the calculator
            const calc::functionList* funcs = calculator.getFunctions();
            std::map<QString, QString> out;

            // Loop through all functions and only add the user defined functions to the map
            for(calc::functionList::const_iterator pos = funcs->begin(); pos != funcs->end(); ++pos)
            {
                calc::userDefinedMathFunction* currFunc = 0;
                if( (currFunc = dynamic_cast<calc::userDefinedMathFunction*>(pos->second)) )
                    out[pos->first.c_str()] = currFunc->getExpression().c_str();
            }

            // Return the result
            return out;
        }

        std::map<QString, calc::real> QTCalc::getVars() const throw()
        {
            // Get the variables from the calculator
            const calc::varList* vars = calculator.getVars();
            std::map<QString, calc::real> out;

            // Loop through all variables and copy their values to the map
            for(calc::varList::const_iterator pos = vars->begin(); pos != vars->end(); ++pos)
                out[pos->first.c_str()] = pos->second;

            // Return the result
            return out;
        }

        void QTCalc::setSettingFilename(const QString& filename)
        { settingFilename = filename; }

        QTCalc::outputType QTCalc::getOutputType() const
        { return calcOutputType; }

        QTCalc::angleType QTCalc::getAngleType() const
        { return currAngleType; }

    // Public slots:
        void QTCalc::calculate(const QString& expr)
        try
        {
            // Calculate the expression, any errors will be caught below
            const calc::real out = calculator.calculate(expr.toStdString());

            // Determine the output type, and store the output in a string
            QString msg;
            switch(calcOutputType)
            {
                case outputScientific:
                    msg = calc::real2str(out, calc::outputType_scientific).c_str();
                break;

                case outputBin:
                    msg = calc::real2str(out, calc::outputType_bin).c_str();
                break;

                case outputOct:
                    msg = calc::real2str(out, calc::outputType_oct).c_str();
                break;

                case outputDec:
                    msg = calc::real2str(out, calc::outputType_dec).c_str();
                break;

                case outputHex:
                    msg = calc::real2str(out, calc::outputType_hex).c_str();
                break;

                case outputTime:
                    msg = calc::real2str(out, calc::outputType_time).c_str();
                break;

                case outputAutoDetect:
                    if(expr.indexOf(':')!=-1)
                        msg = calc::real2str(out, calc::outputType_time).c_str();
                    else
                        msg = calc::real2str(out, calc::outputType_auto).c_str();
                break;
            }

            // Output the result
            result(msg, false);

            // Schedule the settings to be saved, since variables may have changed
            saveSettingsLater();
        }
        catch(calc::calcError& err)
        { calcErrorOccurred(err); }
        catch(calc::overflowError& err)
        { calcErrorOccurred(err); }

        void QTCalc::recalculate()
        try
        {
            // Recalculate the expression, any errors will be caught below
            const calc::real out = calculator.calculate();

            // Determine the output type, and store the output in a string
            QString msg;
            switch(calcOutputType)
            {
                case outputScientific:
                    msg=calc::real2str(out, calc::outputType_scientific).c_str();
                break;

                case outputBin:
                    msg=calc::real2str(out, calc::outputType_bin).c_str();
                break;

                case outputOct:
                    msg=calc::real2str(out, calc::outputType_oct).c_str();
                break;

                case outputDec:
                    msg=calc::real2str(out, calc::outputType_dec).c_str();
                break;

                case outputHex:
                    msg=calc::real2str(out, calc::outputType_hex).c_str();
                break;

                case outputTime:
                    msg=calc::real2str(out, calc::outputType_time).c_str();
                break;

                case outputAutoDetect:
                    if(calculator.getExpression().find(':')!=calc::string::npos)
                        msg=calc::real2str(out, calc::outputType_time).c_str();
                    else
                        msg=calc::real2str(out, calc::outputType_auto).c_str();
                break;
            }

            // Output the result
            result(msg, false);

            // Schedule the settings to be saved, since variables may have changed
            saveSettingsLater();
        }
        catch(calc::calcError& err)
        { calcErrorOccurred(err); }
        catch(calc::overflowError& err)
        { calcErrorOccurred(err); }

        void QTCalc::setVar(const QString& name, const calc::real& value)
        {
            // Add the variable, or change the value of an old one
            calculator.setVar(name.toStdString(), value);
            // Schedule the settings to be saved
            saveSettingsLater();
        }

        void QTCalc::renameVar(const QString& oldName, const QString& newName)
        {
            // Rename the variable
            calculator.renameVar(oldName.toStdString(), newName.toStdString());
            // If there is a built-in variable with the same name as the old name, restore the value of the built-in variable
            calc::varList::iterator foundPos;
            if((foundPos = builtInVars.find(oldName.toStdString())) != builtInVars.end())
                calculator.setVar(foundPos->first, foundPos->second);
            // Schedule the settings to be saved
            saveSettingsLater();
        }

        void QTCalc::deleteVar(const QString& name)
        {
            // Delete the variable
            calculator.deleteVar(name.toStdString());
            // If there is a built-in variable with the same name as the deleted variable, restore the value of the built-in variable
            calc::varList::iterator foundPos;
            if((foundPos = builtInVars.find(name.toStdString())) != builtInVars.end())
                calculator.setVar(foundPos->first, foundPos->second);
            // Schedule the settings to be saved
            saveSettingsLater();
        }


        void QTCalc::setFunc(const QString& name, const QString& content)
        {
            // Used to remember the retrieved function from the calculator engine
            calc::userDefinedMathFunction* function = 0;

            // If the function does alreadt exist and is an user defined function we just change the content
            // Otherwise we add the function to the calculator, or replace the function that was there before
            if( (function = dynamic_cast<calc::userDefinedMathFunction*>(calculator.getFunction(name.toStdString()))) )
                function->setExpression(content.toStdString());
            else
                calculator.setFunction(name.toStdString(), new calc::userDefinedMathFunction(content.toStdString(), true));

            // Schedule the settings to be saved
            saveSettingsLater();
        }

        void QTCalc::renameFunc(const QString& oldName, const QString& newName)
        {
            // If there is already a built in function with the same name as the new name, we delete it from the calculator
            if(builtInFunctions.find(newName.toStdString()) != builtInFunctions.end() && !dynamic_cast<calc::userDefinedMathFunction*>(calculator.getFunction(newName.toStdString())))
                calculator.deleteFunction(newName.toStdString());

            // Rename the function
            calculator.renameFunction(oldName.toStdString(), newName.toStdString());
            calc::functionList::iterator foundPos;
            // If the is a built-in function with the same name as the old name, restore it
            if((foundPos = builtInFunctions.find(oldName.toStdString())) != builtInFunctions.end())
                calculator.setFunction(foundPos->first, foundPos->second);
            // Schedule the settings to be saved
            saveSettingsLater();
        }

        void QTCalc::deleteFunc(const QString& name)
        {
            // Delete the function
            calculator.deleteFunction(name.toStdString());
            // If there is a built-in function with the same name as the deleted function, restore the built-in function
            calc::functionList::iterator foundPos;
            if((foundPos=builtInFunctions.find(name.toStdString())) != builtInFunctions.end())
                calculator.setFunction(foundPos->first, foundPos->second);
            // Schedule the settings to be saved
            saveSettingsLater();
        }

        void QTCalc::loadSettings()
        {
            try
            {
                // If the file doesn't exist, there is nothing to be done
                if(!QFile::exists(QDir::currentPath()+'/'+settingFilename))
                    return;
                // Read the file and store the settings in the calculator and throw an error if it fails
                settingHandler.loadFromFile((QDir::currentPath()+'/'+settingFilename).toStdString());
                if(!settingHandler.copyToCalculator(calculator))
                    throw calc::parseError("Corrupted file", (QDir::currentPath()+'/'+settingFilename).toStdString());
            }
            // Catch any errors and display the right message
            catch(calc::parseError& err)
            {
                if(err.msg == "Corrupted file")
                    error(tr("Couldn't load the settings, the file seems to be corrupted!"));
                else
                    error(tr("An unexpected occurred error while loading the settings!"));
            }
            catch(calc::fileError)
            { error(tr("An unexpected error occurred while trying to load the settings!")); }
        }

        void QTCalc::saveSettings()
        {
            try
            {
                // Clear the settings handler, copy the settings from the calculator and save the settings
                settingHandler.clear();
                settingHandler.settingsFromSource(calculator);
                settingHandler.saveToFile((QDir::currentPath()+'/'+settingFilename).toStdString());
                // Stop the schedule-timer
                saveSettingsTimer.stop();
            }
            // Catch any errors and display the right message
            catch(calc::fileError)
            { error(tr("An unexpected error occurred while trying to save the settings!")); }
        }

        void QTCalc::setOutputType(const outputType& newType)
        { calcOutputType = newType; }

        void QTCalc::setAngleType(const angleType& newType)
        { currAngleType = newType; }

    // Private slots:
        void QTCalc::calcErrorOccurred(const calc::calcError& err)
        {
            // Find out what message should be displayed and display it
            QString msg = "";
            switch(err.type)
            {
                case calc::calcError::unknownToken:
                    if(err.extraRealInfo.size()>0)
                        msg = tr("Unknown token: '%1', at position %2").arg(err.extraStringInfo[0].c_str()).arg(err.extraRealInfo[0] + 1);
                    else
                        msg = tr("Unknown token: '%1'").arg(err.extraStringInfo[0].c_str());
                break;

                case calc::calcError::unexpectedToken:
                    if(err.msg == "Unexptected '.'")
                        msg = tr("Unexpected '.' in a number");
                    else if(err.msg == "Unexpected token")
                        msg = tr("Unexpected token: '%1', at position %2").arg(err.extraStringInfo[0].c_str()).arg(err.extraRealInfo[0] + 1);
                    else
                        msg = tr("Unexpected token: '%1'").arg(err.extraStringInfo[0].c_str());
                break;

                case calc::calcError::unclosedBracket:
                    msg = tr("You didn't close all brackets, %1 brackets still need to be closed!").arg(err.extraRealInfo[0]);
                break;

                case calc::calcError::invalidExpression:
                    if(err.extraStringInfo.size() == 1)
                        msg = tr("Invalid expression: '%1'").arg(err.extraStringInfo[0].c_str());
                    else
                        msg = tr("Invalid expression: '%1', in function %2").arg(err.extraStringInfo[0].c_str(), err.extraStringInfo[1].c_str());
                break;

                case calc::calcError::invalidOperands:
                    if(err.msg == "No negative roots allowed")
                        msg = tr("Can't take the root of a negative value");
                    else if(err.msg == "Only integer powers of negative numbers")
                        msg = tr("Only integer powers of negative numbers are allowed");
                    else if(err.msg == "Division by 0")
                        msg = tr("Can't divide by 0!");
                    else if(err.msg == "Modulo by 0")
                        msg = tr("Can't modulo by 0!");
                break;

                case calc::calcError::invalidArguments:
                    if(err.msg == "Too less arguments")
                        msg = tr("To less arguments: %1 given, %2 expected in function %3").arg(err.extraRealInfo[0]).arg(err.extraRealInfo[1]).arg(err.extraStringInfo[0].c_str());
                    else if(err.msg == "Too many arguments")
                        msg = tr("To many arguments: %1 given, %2 expected in function %3").arg(err.extraRealInfo[0]).arg(err.extraRealInfo[1]).arg(err.extraStringInfo[0].c_str());
                    else
                        msg = tr("Invalid argument: %1, given to function %2").arg(err.extraRealInfo[0]).arg(err.extraStringInfo[0].c_str());
                break;

                case calc::calcError::unknownName:
                    msg = tr((err.msg+": %1").c_str()).arg(err.extraStringInfo[0].c_str());        // err.getMsg is of "Unknown function" of "Unknown variable".
                break;

                case calc::calcError::emptyExpression:
                    msg = tr("Can't calculate an empty expression!");
                break;

                case calc::calcError::recursiveCall:
                    msg = tr("A function may function may not (indirectly) call itself, %1 does").arg(err.extraStringInfo[0].c_str());
                break;

                default:
                    msg = tr("An unknown error has occurred!");
                break;
            }
            result(msg, true);
        }

        void QTCalc::calcErrorOccurred(const calc::overflowError& err)
        {
            // Display the right error, depending on the type of overflow
            switch(err.type)
            {
                case calc::overflowError::bin:
                    result(tr("Value too big to convert to binary"), true);
                break;
                case calc::overflowError::oct:
                    result(tr("Value too big to convert to octal"), true);
                break;
                case calc::overflowError::hex:
                    result(tr("Value too big to convert to hexadecimal"), true);
                break;
            }
        }

    // Private:
        void QTCalc::saveSettingsLater()
        {
            if(!saveSettingsTimer.isActive())
                saveSettingsTimer.start();
        }

        calc::real QTCalc::cos(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            return std::cos(currAngleType == angleDegrees ? mathFunctions::rad(args[0]) : args[0]); }

        calc::real QTCalc::cosh(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            return std::cosh(currAngleType == angleDegrees ? mathFunctions::rad(args[0]) : args[0]);
        }

        calc::real QTCalc::acos(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            if(currAngleType == angleDegrees)
                return mathFunctions::deg(std::acos(args[0]));
            return std::acos(args[0]);
        }

        calc::real QTCalc::sin(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            return std::sin(currAngleType == angleDegrees ? mathFunctions::rad(args[0]) : args[0]);
        }

        calc::real QTCalc::sinh(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            return std::sinh(currAngleType == angleDegrees ? mathFunctions::rad(args[0]) : args[0]);
        }

        calc::real QTCalc::asin(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            if(currAngleType == angleDegrees)
                return mathFunctions::deg(std::asin(args[0]));
            return std::asin(args[0]);
        }

        calc::real QTCalc::tan(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            return std::tan(currAngleType == angleDegrees ? mathFunctions::rad(args[0]) : args[0]);
        }

        calc::real QTCalc::tanh(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            return std::tanh(currAngleType == angleDegrees ? mathFunctions::rad(args[0]) : args[0]);
        }

        calc::real QTCalc::atan(const calc::argList& args)
        {
            if(args.size() != 1)
            {
                calc::calcError err(args.size() ? "Too many arguments" : "Too less arguments", calc::calcError::invalidArguments, args.size());
                err.extraRealInfo.push_back(1);
                throw err;
            }
            if(currAngleType == angleDegrees)
                return mathFunctions::deg(std::atan(args[0]));
            return std::atan(args[0]);
        }

// class QTCalcMemberMathFunction:
    // Public:
        QTCalcMemberMathFunction::QTCalcMemberMathFunction(function initFunction, QTCalc* obj, const bool& cleanUpNeeded)
        : calc::mathFunction(cleanUpNeeded), currFunc(initFunction), obj(obj) {}

        calc::real QTCalcMemberMathFunction::execute(const calc::argList& vars, const calc::string& name)
        {
            try
            {
                // If the object is null-pointer, throw an error
                if(!obj)
                    throw calc::calcError("The function couldn't be executed", calc::calcError::unknown, name);

                // Execute the function
                return (obj->*currFunc)(vars);
            }
            catch(calc::calcError& err)
            {
                // If the member function throws an error, we only need to attach the name of the function
                err.extraStringInfo.push_back(name);
                throw err;
            }
            catch(...)
            { throw calc::calcError("Unknown error", calc::calcError::unknown); }
        }

// Functions:
    namespace mathFunctions
    {
        inline double rad(double deg)
        { return deg/(180/mathConstant::PI); }

        inline double deg(double rad)
        { return rad*(180/mathConstant::PI); }

        inline double round(double src)
        { return (src-std::floor(src) < std::ceil(src)-src ? std::floor(src) : std::ceil(src)); }

        inline double faculty(double n)
        {
            if(n<0)
                throw calc::calcError("Invalid argument!", calc::calcError::invalidArguments, n);
            return (n > 1 ? n*faculty(n-1) : 1);
        }

        calc::real random(const calc::argList& vars)
        {
            // Look at the number of arguments, using that we decide how the function  should be executed
            switch(vars.size())
            {
                // No arguments, just a random number between 0 and 1
                case 0:
                return static_cast<calc::real>(qrand())/RAND_MAX;

                // One argument, being the maximum value to be returned
                // So return a random integer between 0 and the argument (included)
                case 1:
                    if(vars[0] <= 0)
                        throw calc::calcError("Invalid argument!", calc::calcError::invalidArguments);
                return qrand() % static_cast<unsigned int>(vars[0]+1);

                // Two arguments, a minimum and a maximum
                // So return a random integer between the first and the second argument (both included)
                case 2:
                    if(vars[0] >= vars[1])
                        throw calc::calcError("Invalid argument!", calc::calcError::invalidArguments);
                return qrand() % static_cast<unsigned int>(vars[1]-vars[0]+1) + vars[0];

                // More arguments means an invalid argument count
                default:
                {
                    calc::calcError err("Too many arguments", calc::calcError::invalidArguments, vars.size());
                    err.extraRealInfo.push_back(2);
                    throw err;
                }
                break;
            }
        }

        calc::real ifFunction(const calc::argList& vars)
        {
            // Check whether the number of arguments is right
            if(vars.size()<2)
            {
                calc::calcError err("Too less arguments", calc::calcError::invalidArguments, vars.size());
                err.extraRealInfo.push_back(2);
                throw err;
            }

            // If the first argument is not 0, the second argument is returned
            // Otherwise the third argument is returned, the third argument defaults to zero
            if(vars[0] != 0)
                return vars[1];
            else
                return (vars.size() >= 3 ? vars[2] : 0);
        }
    }
