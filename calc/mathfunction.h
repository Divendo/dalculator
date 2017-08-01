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

#ifndef MATHFUNCTION_H
#define MATHFUNCTION_H

#include <list>
#include "types.h"
#include "error.h"

namespace calc
{
    class mathFunction
    {
        public:
            // Constructor
            mathFunction(const bool& cleanUpNeeded);

            // Sets whether this object should be cleaned up by its parent, i.e. that it should be deleted when its parent is done with it
            void setCleanUpNeeded(const bool& newCleanUpNeeded);
            // Get if this object should be cleaned up by its parent
            bool cleanUpNeeded() const;

            // Execute the function
            virtual real execute(const argList& vars, const string& name) = 0;

        protected:
            // Whether this function should be cleaned up by its parent
            bool cleanMeUp;
    };

    class preDefinedMathFunction : public mathFunction
    {
        public:
            // Typedef what a function is
            typedef real (*function)(const argList&);

            // Constructor
            preDefinedMathFunction(const function& initFunction, const bool& cleanUpNeeded = false);

            // Change the function
            void setFunction(const function& newFunction);

            // Execute this function
            virtual real execute(const argList& vars, const string& name);

        private:
            // The current function to be executed by execute()
            function currFunc;
    };

    class cppMathFunction : public mathFunction
    {
        public:
            // Typedef what a function is
            typedef double (*function)(double);

            // Constructor
            cppMathFunction(const function& initFunction, const bool& cleanUpNeeded = false);

            // Change the function
            void setFunction(const function& newFunction);

            // Execute this function
            virtual real execute(const argList& vars, const string& name);

        private:
            // The current function to be executed by execute()
            function currFunc;
    };

    class userDefinedMathFunction : public mathFunction
    {
        public:
            // Constructor
            userDefinedMathFunction(const string& expression, const bool& cleanUpNeeded = false);
            // Destructor
            ~userDefinedMathFunction();

            // Set the expression
            void setExpression(const string& newExpression);
            // Get the current expression
            string getExpression() const;

            // Check if the current expression is valid
            bool isValidExpression();

            // Execute the expression
            virtual real execute(const argList& vars, const string& name);

        private:
            // Static member to filter out functions that (indirectly) call themselfs
            static std::list<string> callStack;

            // The calculator holding the expression, needed to calculate the expression
            calc* calculator;
    };
}

#endif // MATHFUNCTION_H
