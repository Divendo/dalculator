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

#include "mathfunction.h"
#include "calc.h"
#include <limits>
#include <algorithm>

namespace calc
{
    // mathFunction:
        // Public:
            mathFunction::mathFunction(const bool& cleanUpNeeded)
            : cleanMeUp(cleanUpNeeded) {}

            void mathFunction::setCleanUpNeeded(const bool& newCleanUpNeeded)
            { cleanMeUp = newCleanUpNeeded; }
            bool mathFunction::cleanUpNeeded() const
            { return cleanMeUp; }

    // preDefinedMathFunction:
        // Public:
            preDefinedMathFunction::preDefinedMathFunction(const function& initFunction, const bool& cleanUpNeeded)
            : mathFunction(cleanUpNeeded), currFunc(initFunction){}

            void preDefinedMathFunction::setFunction(const function& newFunction)
            { currFunc = newFunction; }

            real preDefinedMathFunction::execute(const argList& vars, const string& name) throw(calcError)
            {
                try
                {
                    // Execute the function using the given variables
                    return currFunc(vars);
                }
                catch(calcError& err)
                {
                    // If an error occurred, and it was because of the arguments of the function, add the name of the function as extra info
                    if(err.type == calcError::invalidArguments)
                        err.extraStringInfo.push_back(name);
                    throw err;
                }
                catch(...)
                { throw calcError("Unknown error", calcError::unknown, name); }
            }

    // cppMathFunction:
        // Public:
            cppMathFunction::cppMathFunction(const function& initFunction, const bool& cleanUpNeeded)
            : mathFunction(cleanUpNeeded), currFunc(initFunction){}

            void cppMathFunction::setFunction(const function& newFunction)
            { currFunc = newFunction; }

            real cppMathFunction::execute(const argList& vars, const string& name) throw(calcError)
            {
                try
                {
                    // Check the number of arguments
                    if(vars.size()<1)
                        throw calcError("Too less arguments", calcError::invalidArguments, name, vars.size());
                    if(vars.size()>1)
                        throw calcError("Too many arguments", calcError::invalidArguments, name, vars.size());

                    // Execute the function using the given arguments
                    return currFunc(vars[0]);
                }
                catch(calcError& err)
                {
                    if(err.type == calcError::invalidArguments)
                        err.extraStringInfo.push_back(name);
                    throw err;
                }
                catch(...)
                { throw calcError("Unknown error", calcError::unknown); }
            }

    // userDefinedMathFunction:
        // Public:
            userDefinedMathFunction::userDefinedMathFunction(const string& expression, const bool& cleanUpNeeded)
            : mathFunction(cleanUpNeeded), calculator(new calc(expression, false)) {}

            userDefinedMathFunction::~userDefinedMathFunction()
            { delete calculator; }

            void userDefinedMathFunction::setExpression(const string& newExpression)
            {
                // Change the expression of the calculator
                calculator->setExpression(newExpression);
            }

            string userDefinedMathFunction::getExpression() const
            { return calculator->getExpression(); }

            bool userDefinedMathFunction::isValidExpression()
            {
                // If the expression isn't parsed yet, parse it
                calculator->parse();

                // Return whether the expression is a valid one
                return calculator->isValidExpression();
            }

            real userDefinedMathFunction::execute(const argList& vars, const string& name) throw(calcError)
            {
                // Check if this function isn't (indirectly) calling itself
                if(std::find(userDefinedMathFunction::callStack.begin(), userDefinedMathFunction::callStack.end(), name) != userDefinedMathFunction::callStack.end())
                {
                    userDefinedMathFunction::callStack.clear();
                    throw calcError("A function may not (indirectly) call itself", calcError::recursiveCall, name);
                }
                userDefinedMathFunction::callStack.push_back(name);

                // Find the number of arguments in the expression
                unsigned short argumentCount = 0;
                string expr = calculator->getExpression();
                while(argumentCount < std::numeric_limits<unsigned short>::max() && expr.find("ARG"+real2str(argumentCount)) != string::npos)
                    ++argumentCount;

                // Check if the numbers of arguments is right, if not throw an error
                if(vars.size()<argumentCount)
                {
                    // Clear the call stack
                    userDefinedMathFunction::callStack.clear();

                    // Throw the error
                    std::vector<real> extraRealInfo(2, vars.size());
                    extraRealInfo[1] = argumentCount;
                    throw calcError("Too less arguments", calcError::invalidArguments, std::vector<string>(1, name), extraRealInfo);
                }
                if(vars.size()>argumentCount)
                {
                    // Clear the call stack
                    userDefinedMathFunction::callStack.clear();

                    // Throw the error
                    std::vector<real> extraRealInfo(2, vars.size());
                    extraRealInfo[1] = argumentCount;
                    throw calcError("Too many arguments", calcError::invalidArguments, std::vector<string>(1, name), extraRealInfo);
                }

                // Parse the expression (if it isn't parsed yet)
                calculator->parse();

                // Throw an error if the expression is invalid
                if(!calculator->isValidExpression())
                {
                    // Clear the call stack
                    userDefinedMathFunction::callStack.clear();

                    // Throw the error
                    std::vector<string> extraStringInfo(2, calculator->getExpression());
                    extraStringInfo[1] = name;
                    throw calcError("Invalid expression in the function", calcError::invalidExpression, extraStringInfo);
                }

                // Remember the original values of the variables and then set the new values of them
                varList originalVars;
                for(size_t i = 0; i<vars.size(); ++i)
                {
                    string name = "ARG"+real2str(i);
                    if(calculator->varExists(name))
                        originalVars[name] = calculator->getVar(name);
                    calculator->setVar(name, vars[i]);
                }

                try
                {
                    // Calculate the expression
                    real out = calculator->calculate();

                    // Put the original values back
                    for(varList::const_iterator pos = originalVars.begin(); pos != originalVars.end(); ++pos)
                        calculator->setVar(pos->first, pos->second);

                    // This function is done executing, clear pop it from the call stack
                    userDefinedMathFunction::callStack.pop_back();

                    // Return the result
                    return out;
                }
                catch(calcError&)
                {
                    // Clear the call stack
                    userDefinedMathFunction::callStack.clear();

                    // Put the original values back
                    for(varList::const_iterator pos = originalVars.begin(); pos != originalVars.end(); ++pos)
                        calculator->setVar(pos->first, pos->second);

                    // Rethrow the error
                    throw;
                }
            }

        // Private:
            // Static:
                std::list<string> userDefinedMathFunction::callStack = std::list<string>();
}
