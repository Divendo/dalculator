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

#include "calc_private.h"
#include <cctype>
#include <cmath>
#include <limits>
#include <sstream>
#include <iomanip>

namespace calc
{
    namespace calcPrivate
    {
        real binStr2real(const string& str, const bool& throwError)
        {
            real out = 0;                           // The result is stored in this variable
            unsigned short counter = 0;             // Counts the number of 1's and 0's that are parsed

            // Iterate through the string, from back to forth
            for(string::const_reverse_iterator pos = str.rbegin(); pos != str.rend(); ++pos)
            {
                // In case of a 1 or 0, we add it to out
                // Otherwise, throw an error
                if(*pos=='0' || *pos=='1')
                    out += (*pos-'0') * std::pow(2, counter++);
                else if(throwError && *pos!='-')
                    throw calcError("Unknown token", calcError::unknownToken, *pos);
            }

            // Make the value negative if the first character was a - sign
            if(str[0]=='-')
                out*=-1;

            // Return the value
            return out;
        }

        real octStr2real(const string& str, const bool& throwError)
        {
            real out = 0;                           // The result is stored in this variable
            unsigned short counter = 0;             // Counts the number of read ciphers

            // Iterate through the string, from back to forth
            for(string::const_reverse_iterator pos = str.rbegin(); pos != str.rend(); ++pos)
            {
                // If the current character is a octal cipher, we add it to out
                // Otherwise, throw an error
                if(*pos>='0' && *pos<='7')
                    out += (*pos-'0') * pow(8, counter++);
                else if(throwError && *pos!='-')
                    throw calcError("Unknown token", calcError::unknownToken, *pos);
            }

            // Make the value negative if the first character was a - sign
            if(str[0]=='-')
                out*=-1;

            // Return the value
            return out;
        }

        real decStr2real(const string& str, const bool& throwError)
        {
            real out = 0;                           // The result is stored in this variable
            bool dotEncounterd = false;             // Whether or not we've already encountered a dot (.)
            unsigned short counter = 0;             // Counts the number of read ciphers
            real eValue = 0;                        // The value behind the E or e is stored in this variable (in case scientific notation is used: 1.23e45)
            bool negative = false;                  // Whether the value should be negative or not

            // Iterate through the string, from back to forth
            for(string::const_reverse_iterator pos = str.rbegin(); pos != str.rend(); ++pos)
            {
                // If the current character is a decimal cipher, we add it to out
                if(*pos>='0' && *pos<='9')
                    out += (*pos-'0') * pow(10, counter++);
                // If the current character is a dot and we didn't encounter one yet
                // We divide out by 10^counter, reset the counter and remember that we've encountered a dot
                // If we had already encountered a dot, we'll just throw an error
                else if(*pos=='.')
                {
                    if(!dotEncounterd)
                    {
                        out /= pow(10, counter);
                        counter = 0;
                        dotEncounterd = true;
                        continue;
                    }
                    else if(throwError)
                        throw calcError("Unexptected '.'", calcError::unexpectedToken);
                }
                // If we encounter an e or E, we set the eValue to the current value and reset out, counter, negative and dotEncounterd
                else if(*pos=='e' || *pos=='E')
                {
                    eValue = out*(negative ? -1 : 1);
                    out = (counter = 0);
                    negative = (dotEncounterd = false);
                }
                // If we encounter a - sign, remember that the value is negative
                else if(*pos == '-')
                    negative = true;
                // Any other character has to be invalid
                else if(throwError)
                    throw calcError("Unknown token", calcError::unknownToken, *pos);
            }

            // Mulitply out by 10^eValue to apply the scientific notation
            out *= pow(10, eValue);

            // If the value should be negative, make it negative
            if(negative)
                out*=-1;

            // Return the value
            return out;
        }

        real hexStr2real(const string& str, const bool& throwError)
        {
            real out = 0;                           // The result is stored in this variable
            unsigned short counter = 0;             // Counts the number of read ciphers

            // Iterate through the string, from back to forth
            for(string::const_reverse_iterator pos = str.rbegin(); pos != str.rend(); ++pos)
            {
                // If the current character is a hexadecimal cipher, we add it to out
                if(*pos>='0' && *pos<='9')
                    out+=(*pos-'0')*pow(16, counter++);
                else if(*pos>='a' && *pos<='f')
                    out+=(*pos-'a'+10)*pow(16, counter++);
                else if(*pos>='A' && *pos<='F')
                    out+=(*pos-'A'+10)*pow(16, counter++);
                // Any other character (-, x and X excluded) doesn't belong there
                else if(throwError && *pos!='-' && *pos!='x' && *pos!='X')
                    throw calcError("Unknown token", calcError::unknownToken, *pos);
            }

            // Make the value negative if the first character was a - sign
            if(str[0]=='-')
                out*=-1;

            // Return the value
            return out;
        }

        string real2timeStr(real val)
        {
            // This string is going to contain the output
            string out;

            // If the value is negative, put a - in front of the string and make the value possitive for further processing
            if(val < 0)
            {
                out+='-';
                val*=-1;
            }

            // Repeat 3 times, for i = [2, 0]
            for(short i = 2; i >= 0; --i)
            {
                // If i is 0, x will be just the same as val
                // If not it will be val / 60^i, rounded down
                const real x = ( i==0 ? val : std::floor(val/std::pow(60, i)) );

                // If x is smaller than 10, add a 0 to the string
                // to make make sure every part in the time-string is 2 digits (e.g. 02:05:15 and not 2:5:15)
                if(x<10)
                    out+='0';
                // If i is 0, we also want to show some numbers behind the dot (milliseconds)
                if(i==0)
                    out+=real2decStr(x, outputType_dec, 3);
                // Otherwise just add x to the string and add a :
                else
                    out+=real2decStr(x, outputType_dec, 0)+':';

                // Subtract x*60^i from val, this is the part of val we just added to the string
                val -= x*std::pow(60, i);
            }

            // Return the result
            return out;
        }

        string real2binStr(const real& val)
        {
            // If the value is too big to convert, throw an error
            if(val > std::numeric_limits<unsigned long int>::max())
                throw overflowError(overflowError::bin);

            // If the value is 0, return "0"
            if(val == 0)
                return "0";

            // Round the value to the nearest integer, and if it's negative make it positive
            // Store the result in intVal
            unsigned long int intVal = (val<0 ? -1 : 1) * (std::ceil(val) - val > val - std::floor(val) ? std::floor(val) : std::ceil(val));
            string out;
            // As long as intVal isn't 0 we add a 1 or a 0 to the string and divide intVal by 2
            while(intVal!=0)
            {
                out.insert(out.begin(), intVal % 2 + '0');
                intVal /= 2;
            }

            // Return the string, put a - in front of it if val was negative
            return (val<0 ? "-" : "")+out;
        }

        string real2octStr(const real& val)
        {
            // If the value is too big to convert, throw an error
            if(val>std::numeric_limits<unsigned long int>::max())
                throw overflowError(overflowError::oct);

            // Round the value to the nearest integer, and if it's negative make it positive
            // Store the result in intVal
            unsigned long int intVal = (val<0 ? -1 : 1)*(std::ceil(val) - val > val - std::floor(val) ? std::floor(val) : std::ceil(val));

            // Create a string using a std::ostringstream
            // Return the result
            std::ostringstream outstream;
            if(val<0)
                outstream<<'-';
            outstream<<'0'<<std::oct<<intVal;
            return outstream.str();
        }

        string real2decStr(const real& val, const realOutputType& outputType, const int& precision)
        {
            // Create a std::ostringstream and set some flags to make sure the string is outputted in the way we want it to be
            // Then create a string from the stream, containing the number
            std::ostringstream outstream;
            if(outputType == outputType_scientific)
                outstream<<std::scientific;
            else if(outputType!=outputType_auto)
                outstream<<std::fixed;
            outstream<<std::setprecision(precision)<<val;
            string out = outstream.str();

            // If the precision is 0 or no dot is found, we can just return the string right away
            size_t pos;
            if(precision == 0 || (pos = out.find('.'))==string::npos)
                return out;

            // If not, we need to clean up any 0's at the end first
            size_t zeroFromPos = pos;               // From what position the first 0 was found
            bool eFound = false;                    // If and e or E (from scientific notation) was found
            for(++pos; pos < out.size(); ++pos)
            {
                // If we encounter a 0, and zeroFromPos has not been set, we remember the position of the 0
                if(out[pos] == '0' && zeroFromPos == string::npos)
                    zeroFromPos=pos;
                // If the current character is something else than a 0, we unset zeroFromPos
                // Unless an e or E (from scientific notation) is found, in that case we set zeroFromPos to the current position,
                // remember that an e or E was found and break out of the loop
                else if(out[pos]!='0')
                {
                    if(out[pos]=='e' || out[pos]=='E')
                    {
                        if(zeroFromPos == string::npos)
                            zeroFromPos = pos;
                        eFound = true;
                        break;
                    }
                    else
                        zeroFromPos = string::npos;
                }
            }

            // If we found an e or E (from scientific notation)
            if(eFound)
            {
                // Remember everything of the string after the e or E
                std::string tmp = out.substr(++pos);
                // Chop everything after the e or E off from the string
                out = out.substr(0, zeroFromPos)+'e';

                // Remove any 0's and +'s before, untill a digit is found
                for(size_t tmpPos = 0; tmpPos < tmp.length(); ++tmpPos)
                {
                    if(tmp[tmpPos] == '-')
                        out+='-';
                    else if(tmp[tmpPos]=='+' || tmp[tmpPos]=='0')
                        continue;
                    else
                    {
                        out+=tmp.substr(tmpPos);
                        break;
                    }
                }
                return out[out.length()-1]=='e' ? out.substr(0, out.length()-1) : out;
            }
            // If no e or E was found, we return the string chopping off any 0's at the end
            else
                return out.substr(0, zeroFromPos);
        }

        string real2hexStr(const real& val)
        {
            // If the value is too big to convert, throw an error
            if(val>std::numeric_limits<unsigned long int>::max())
                throw overflowError(overflowError::hex);

            // Round the value to the nearest integer, and if it's negative make it positive
            // Store the result in intVal
            unsigned long int intVal = (val<0 ? -1 : 1)*(std::ceil(val) - val > val - std::floor(val) ? std::floor(val) : std::ceil(val));

            // Create a string using a std::ostringstream
            // Return the result
            std::ostringstream outstream;
            if(val<0)
                outstream<<'-';
            outstream<<"0x"<<std::hex<<intVal;
            return outstream.str();
        }
    }
}
