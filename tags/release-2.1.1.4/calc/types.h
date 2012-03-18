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

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <map>
#include <vector>

namespace calc
{
    // Using namespaces
    using std::string;

    // Forward declaration of some classes
    class calc;
    class mathFunction;

    // Some typedefs
    typedef double real;
    typedef unsigned char idType;

    typedef std::map<string, mathFunction*> functionList;
    typedef std::map<string, real> varList;
    typedef std::vector<real> argList;

    // Enums:
    enum realOutputType
    {
        outputType_auto,
        outputType_scientific,
        outputType_bin,
        outputType_oct,
        outputType_dec,
        outputType_hex,
        outputType_time
    };
}

#endif // TYPES_H
