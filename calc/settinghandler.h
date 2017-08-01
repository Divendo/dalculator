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

#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H

#include <list>
#include "types.h"
#include "error.h"

namespace calc
{
    class settingHandler : public std::map<string, string>
    {
        public:
            // Constructors
            settingHandler();
            settingHandler(const calc& settingSource);

            // Copy all settings from the given calculator
            void settingsFromSource(const calc& settingSource);
            // Copy the settings to the variables and functions of the given calculator
            bool copyToCalculator(calc& calculator) const;

            // Load the settings from a file
            void loadFromFile(const string& fileName);
            // Save the settings to a file
            void saveToFile(const string& fileName);

        protected:
            // Checks if the given name is a valid name
            bool isValidName(const string& name);

        private:
            // Read or write a file that is using file-format version 1
            void fileParserV1(const string& filename);
            void saveToFileV1(const string& filename);
    };
}

#endif // SETTINGHANDLER_H
