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

#ifndef CALC_PRIVATE_H
#define CALC_PRIVATE_H

#include <cctype>

#include "types.h"
#include "error.h"

namespace calc
{
    namespace calcPrivate
    {
        // Checks if the given character is a character that's allowed in a name
        // A name consists of an alphabetical character or an underscore (_)
        // and can be followed by alphanumerical characters and underscores
        inline bool isNameChar(const char& chr, const bool& firstChar)
        { return (chr=='_') || (firstChar ? std::isalpha(chr) : std::isalnum(chr)); }

        // Converts a string containing a binary number to a real
        real binStr2real(const string& str, const bool& throwError = false);
        // Converts a string containing an octal number to a real
        real octStr2real(const string& str, const bool& throwError = false);
        // Converts a string containing a decimal number to a real
        real decStr2real(const string& str, const bool& throwError = false);
        // Converts a string containing a hexadecimal number to a real
        real hexStr2real(const string& str, const bool& throwError = false);

        // Converts a real to a string containing the binary number
        string real2binStr(const real& val);
        // Converts a real to a string containing the octal number
        string real2octStr(const real& val);
        // Converts a real to a string containing the decimal number
        string real2decStr(const real& val, const realOutputType& outputType, const int& precision);
        // Converts a real to a string containing the hexadecimal number
        string real2hexStr(const real& val);
        // Converts a real to a string containing it's value in time (1 is 1 sec)
        string real2timeStr(real val);
    }
}

#endif // CALC_PRIVATE_H
