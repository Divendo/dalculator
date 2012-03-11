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

#ifndef ERROR_H
#define ERROR_H

#include "types.h"

namespace calc
{
    // Standard error
    class error {};

    // An error with a message
    class errorWithMsg : public error
    {
        public:
            errorWithMsg(const string& msg = "")
            : msg(msg) {}

            string msg;                             // The message of the error
    };

    // A file error, this is an error which occurred while operating a file
    class fileError : public error
    {
        public:
            enum actionType
            {
                action_unknown,                     // The action that caused the error is unknown
                action_opening,                     // The error occurred while opening the file
                action_reading,                     // The error occurred while reading the file
                action_writing                      // The error occurred while writing to the file
            };

            fileError(const string& fileName, const actionType& action=action_unknown)
            : fileName(fileName), action(action) {}

            string fileName;                        // The name of the file where the error occurred
            actionType action;                      // The action that caused the error
    };

    // A parse error, an error that occurred while parsing a file
    class parseError : public errorWithMsg
    {
        public:
            parseError(const string& msg = "", const string& extraData = "")
            : errorWithMsg(msg), extraData(extraData) {}

            string extraData;                       // Some extra data attached to the error
    };

    // A calculator error, an error that occurred while trying to calculate an expression
    class calcError : public errorWithMsg
    {
        public:
            enum errorType
            {
                unknown,                            // The type of the error is unknown
                unknownToken,                       // An unknown token was found
                unexpectedToken,                    // An unexpected token was found
                unclosedBracket,                    // An unclosed bracket was found
                invalidExpression,                  // The expression is invalid
                invalidOperands,                    // One of the operands, or both operands are invalid
                invalidArguments,                   // An invalid argument or an invalid number of arguments was detected
                unknownName,                        // The function or variable doesn't exist
                emptyExpression,                    // The expression is empty
                recursiveCall                       // A function is calling itself
            };

            calcError(const string& msg = "", const errorType& type = unknown, const std::vector<string>& extraStringInfo = std::vector<string>(), const std::vector<real>& extraRealInfo = std::vector<real>())
                : errorWithMsg(msg), type(type), extraStringInfo(extraStringInfo), extraRealInfo(extraRealInfo) {}
            calcError(const string& msg, const errorType& type, const string& extraStringInfo)
                : errorWithMsg(msg), type(type), extraStringInfo(1, extraStringInfo) {}
            calcError(const string& msg, const errorType& type, const real& extraRealInfo)
                : errorWithMsg(msg), type(type), extraRealInfo(1, extraRealInfo) {}
            calcError(const string& msg, const errorType& type, const string& extraStringInfo, const real& extraRealInfo)
                : errorWithMsg(msg), type(type), extraStringInfo(1, extraStringInfo), extraRealInfo(1, extraRealInfo) {}


            errorType type;                         // The type of the error, i.e. what caused the error
            std::vector<string> extraStringInfo;    // Extra info stored in strings
            std::vector<real> extraRealInfo;        // Extra info stored in reals
    };

    // An overflow error, occurs when a too big value is being converted to a string
    class overflowError : public error
    {
        public:
            enum dataType
            {
                bin,                                // The error occurred while converting to a string containing a binary number
                oct,                                // The error occurred while converting to a string containing an octal number
                hex                                 // The error occurred while converting to a string containing a hexadecimal number
            };

            overflowError(const dataType& type)
                : type(type) {}

            dataType type;                          // The format of the string where the number was being converted to
    };
}

#endif // ERROR_H
