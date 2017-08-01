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

#include "settinghandler.h"
#include "mathfunction.h"
#include "calc_private.h"
#include "calc.h"
#include <fstream>

namespace calc
{
    // settingHandler:
        // Public:
            settingHandler::settingHandler()
            {}

            settingHandler::settingHandler(const calc& settingSource)
            { settingsFromSource(settingSource); }

            void settingHandler::settingsFromSource(const calc& settingSource)
            {
                // Clear all current settings
                clear();

                // Get all variables and functions from the calculator
                varList vars = *settingSource.getVars();
                functionList functions = *settingSource.getFunctions();

                // Iterate through all variables and store their values
                for(varList::const_iterator pos = vars.begin(); pos != vars.end(); ++pos)
                    insert(std::make_pair("v"+pos->first, real2str(pos->second)));

                // Iterate through all functions and store their values, only store the user defined functions
                for(functionList::const_iterator pos = functions.begin(); pos != functions.end(); ++pos)
                {
                    if(userDefinedMathFunction* currFunc = dynamic_cast<userDefinedMathFunction*>(pos->second))
                        insert(std::make_pair("f"+pos->first, currFunc->getExpression()));
                }
            }

            bool settingHandler::copyToCalculator(calc& calculator) const
            {
                // This variable is going to remember whether the function was executed succesfully
                bool succes = true;

                // Iterate through all settings in this container
                for(const_iterator pos = begin(); pos != end(); ++pos)
                {
                    // If the first letter is a 'v', it's a variable
                    // If the first letter is a 'f', it's a function
                    // Otherwise something went wrong
                    switch(pos->first[0])
                    {
                        case 'v':
                            calculator.setVar(pos->first.substr(1), str2real(pos->second));
                        break;

                        case 'f':
                        {
                            userDefinedMathFunction* func = new userDefinedMathFunction(pos->second, true);
                            calculator.setFunction(pos->first.substr(1), func);
                        }
                        break;

                        default:
                            succes = false;
                        break;
                    }
                }

                // Return whether everything went well
                return succes;
            }

            void settingHandler::loadFromFile(const string& fileName)
            {
                clear();
                fileParserV1(fileName);
            }

            void settingHandler::saveToFile(const string& fileName)
            { saveToFileV1(fileName); }

        // Protected:
            bool settingHandler::isValidName(const string& name)
            {
                bool firstChar = true;
                for(string::const_iterator pos=name.begin(); pos!=name.end(); pos++)
                {
                    if(!calcPrivate::isNameChar(*pos, firstChar))
                        return false;
                    firstChar = false;
                }
                return name.size()>0;
            }

        // Private:
            void settingHandler::fileParserV1(const string& filename)
            {
                // Open file, and tell it to throw an exception if something goes wrong
                std::ifstream in(filename.c_str(), std::ifstream::in | std::ifstream::binary);
                in.exceptions(std::ifstream::badbit);

                // Check if the file is opened and throw an error if it isn't
                if(!in.is_open() || !in.good())
                    throw fileError(filename, fileError::action_opening);

                try
                {
                    // If the file version isn't 1, something's wrong
                    if(in.get() != 1)
                        throw parseError("Unknown version", filename);

                    // Read all name-value pairs
                    std::string name = "";
                    std::string value = "";
                    while(getline(in, name, '\0'))
                    {
                        // If we can't read a value, the file must be corrupted (every name needs a value)
                        if(!getline(in, value, '\0'))
                            throw parseError("Corrupted file", filename);

                        // The file is corrupt if the name is empty, doesn't start with a 'v' or 'f' or is not a valid name
                        if(name.size() <= 1 || (name[0]!='v' && name[0]!='f') || !isValidName(name.substr(1)))
                            throw parseError("Corrupted file", filename);

                        // This pair is read succesfully, add to the settings
                        insert(std::make_pair(name, value));
                    }

                    // Close the file
                    in.close();
                }
                catch(parseError&)
                {
                    // Something went wrong while parsing, close the file and rethrow the error
                    in.close();
                    throw;
                }
                catch(...)
                {
                    // Something went wrong while reading, close the file and throw an error
                    in.close();
                    throw fileError(filename, fileError::action_reading);
                }
            }

            void settingHandler::saveToFileV1(const string& filename)
            {
                // Open file, and tell it to throw an exception if something goes wrong
                std::ofstream out(filename.c_str(), std::ofstream::out | std::ofstream::binary);
                out.exceptions(std::ofstream::badbit);

                // Check if the file is opened and throw an error if it isn't
                if(!out.is_open() || !out.good())
                    throw fileError(filename, fileError::action_opening);

                try
                {
                    // Write the file version to the file
                    out<<'\x01';

                    // Loop through all settings, and write them to the file
                    for(const_iterator pos = begin(); pos != end(); ++pos)
                       out<<pos->first<<'\0'<<pos->second<<'\0';

                    // Close the file
                    out.close();
                }
                catch(...)
                {
                    // Something went wrong while writing, close the file and throw an error
                    out.close();
                    throw fileError(filename, fileError::action_writing);
                }
            }
}
