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

#ifndef CALC_H
#define CALC_H

#include <string>
#include <map>
#include <vector>
#include <list>

#include "types.h"
#include "error.h"
#include "settinghandler.h"
#include "mathfunction.h"

namespace calc
{
    class calc
    {
        public:
            // Constructor, initialise the expression with expr and set whether the functions should be cleaned up by the destructor
            calc(const string& expr = "", const bool& cleanFunctionsUp = true);
            ~calc();

            // Get or set the expression
            void setExpression(const string& expr);
            string getExpression() const;

            // Delete a variable, returns true if the variable existed and is deleted
            bool deleteVar(const string& name);
            // Set the value of a variable and returns a reference to that value
            real& setVar(const string& name, const real& value = 0);
            // Change the whole list of variables to the new list
            void setVarlist(const varList& newVars);
            // Rename a variable, returns true if succesful (i.e. the variable did exist and there didn't already exist a variabele with the new name)
            bool renameVar(const string& oldName, const string& newName);
            // Get a reference to the value of a variable, this function will create the variable if it didn't exist
            real& getVar(const string& name);
            // Get the value of a variable, returns 0 if the variable didn't exist
            real getVar(const string& name) const;
            // Returns true if the variable exists
            bool varExists(const string& name) const;
            // Returns a const pointer to the varList that contains all variables
            const varList* getVars() const;

            // Delete a function, returns true if the function existed and is deleted
            bool deleteFunction(const string& name);
            // Set a function to the given mathFunction
            void setFunction(const string& name, mathFunction* function = 0);
            // Rename a function, returns true if succesful (i.e. the function did exist and there didn't already exist a function with the new name)
            bool renameFunction(const string& oldName, const string& newName);
            // Get a pointer to the function with the given name (returns 0 if the function doesn't exist)
            mathFunction* getFunction(const string& name);
            // Get a const pointer to the function with the given name (returns 0 if the function doesn't exist)
            const mathFunction* getFunction(const string& name) const;
            // Returns true if the function with the given name exists
            bool functionExists(const string& name) const;
            // Returns a const pointer to the functionList that contains all functions
            const functionList* getFunctions() const;

            // Parse the current expression and remember any errors, this function won't do anything if the current expression is already parsed
            void parse();
            // Whether the current expression has already been parsed or not
            bool parsed() const;
            // Force to parse the current expression again
            void forceParse();

            // Get all errors encountered while parsing the last time
            const std::vector<calcError>* getParseErrors();
            // Returns true if the current expression can be calculated (i.e. doesn't contain any errors)
            // This doesn't garantuee that calculating the expression won't throw any errors,
            // the expression is a valid expression but may for example contain an unknown variable
            // Note that this is always false if the expression isn't parsed yet
            bool isValidExpression() const;

            // Calculate the current expression, this function will parse the expression if it wasn't parsed already
            // Returns the result of the expression, if an error occurs while parsing or calculating an error is thrown
            // If more than one error is encountered during the parsing, the first error is thrown all other errors can still be retrieved using getParseErrors()
            real calculate();
            // Set the current expression to newExpr and calculate the expression, this function will also parse the expression
            // Returns the result of the expression, if an error occurs while parsing or calculating an error is thrown
            real calculate(const string& newExpr);

        private:
            // Static:
                // The list of all variables
                static varList currVars;
                // The list of all functions
                static functionList currFunctions;

            // Token, this represents a part of the expression for example a number or an operator
            struct Token
            {
                // Possible types of a token
                enum Type
                {
                    tokenUnknown,                   // A token of an unknown type
                    tokenOperator,                  // The token is an operator
                    tokenReal,                      // The token is a real value, the value is stored in a yet to be parsed string
                    tokenRealReal,                  // The token is a real value, the value is already parsed and stored in a real
                    tokenName,                      // The token is a name of a function or variable, in case it's a function name it will become a tokenFunctionStart
                    tokenWhitespace,                // The token is a whitespace character
                    tokenOpenBracket,               // The token is an opening bracket: (
                    tokenCloseBracket,              // The token is an closing bracket: )
                    tokenComma,                     // The token is a comma: ,
                    tokenFunctionStart,             // The token is the start of a function, this was a tokenName before
                    tokenAssignmentOperator         // The token is an assignment operator, i.e: =
                };

                // The type of the token
                Type type;
                // A string containing some extra information about the token (for example: which operator it is)
                string str;
                // The value of a tokenRealReal
                real val;

                // Constructors to initialise all variables of the token
                Token(const Type& type = tokenUnknown, const string& str = "", const real& val = 0)
                : type(type), str(str), val(val) {}
                Token(const Type& type, const char& chr, const real& val = 0)
                : type(type), str(1, chr), val(val) {}
            };

            // Calculator functions, these do the actual calculating
            // Every function returns true if it needs to be called another time and false when it's done
                bool calcDoBrackets();
                bool calcDoFunctions();
                bool calcDoPowers();
                bool calcDoMultiply();
                bool calcDoPlusmin();
                bool calcDoCompare();
                bool calcDoBitwise();
                bool calcDoAssignments();

            // Prevent copying:
            calc& operator=(calc& other);
            calc(const calc& other);

            // Function to search for any errors, this is called after parsing
            void searchForErrors();

            // Check whether the character is an operator
            bool isOperator(const char& chr) const;
            // Check whether the character can be part of a real value, firstStrPart should be the real as a string for as far as it's known
            bool isReal(const char& chr, const string& firstStrPart = "") const;
            // Check whether the character is a whitespace
            bool isWhitespace(const char& chr) const;
            // Returns the real value of the token, this resolves any variables as well
            real tokenToValue(const Token& token) const;

            // Construct a calculator using the given tokens
            // Also set whether the to be created instance should clean up all math functions
            calc(const std::vector<Token>& tokens, const bool& cleanFunctionsUp = false);

            // The current expression
            string currExpr;
            // Whether the expression is parsed or not
            bool expressionParsed;
            // An array containing all error that occurred during the parsing
            std::vector<calcError> errors;
            // Vector to store the tokens, this is the result of parsing the expression
            std::vector<Token> tokens;
            // Whether the functions should be cleaned up or not in the destructor
            bool cleanFunctionsUp;
    };

    // Functions:
        // Converts the given string (str) to a real value, if throwError is true this function will throw an error if something goes wrong
        real str2real(const string& str, const bool& throwError = false);
        // Converts the given string (str) containing a time (e.g. 2:31:12) to a real holding the number of seconds
        // If throwError is true this function will throw an error if something goes wrong
        real timestr2real(const string& str, const bool& throwError = false);
        // Converts the given real (val) to a string, using the given format (outputType) and precision
        // If the precision is a negative number std::numeric_limits<real>::digits10 will be used
        string real2str(const real& val, const realOutputType& outputType = outputType_auto, const int& precision = -1);
}

#endif // CALC_H
