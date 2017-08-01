#ifndef INISECTION_H
#define INISECTION_H

/************************************************** Info: ***************************************************
* Author:     Divendo                                                                                       *
* Version:    1.1                                                                                           *
* Website:    http://divendo-webs.com                                                                       *
*                                                                                                           *
* This code is under the GPLv3 license.                                                                     *
* That means that you're free to use and edit this code,                                                    *
* as long as you publish any changes you make using this license.                                           *
*                                                                                                           *
* For the full license, see gpl3.txt or gpl3.html.                                                          *
************************************************************************************************************/

#include "inivalue.h"
#include <vector>
#include <string>

namespace dini
{
    // Error class, thrown when trying to get a value or section by name and it doesn't exist
    class unknownName
    {
        public:
            unknownName(const std::string& name);

            std::string name;   // Name of the value or section which doesn't exist
    };

    // Represents an ini section, containing values
    class iniSection
    {
        public:
            // Iterators
            typedef std::vector<iniValue>::iterator iterator;
            typedef std::vector<iniValue>::reverse_iterator reverse_iterator;
            typedef std::vector<iniValue>::const_iterator const_iterator;
            typedef std::vector<iniValue>::const_reverse_iterator const_reverse_iterator;

            // Construct by only giving a name
            iniSection(const std::string& name="name");
            // Construct by giving a name and another section to copy the values from (the name of the other section will be ignored)
            iniSection(const std::string& name, const iniSection& other);

            // Get the name of this section
            std::string name() const;
            // Set the name of this section, returns true if the name was changed succesfull, which is when the name is a valid name
            bool setName(const std::string& name);

            // Clear all values in this section
            void clear();

            // Get a value by name
            iniValue& getValue(const std::string& name);
            iniValue getValue(const std::string& name) const;

            // Assigns a value to a name
            void setValue(const std::string& name, const iniValue& value);
            void setValue(const std::string& name, const int& value);
            void setValue(const std::string& name, const double& value);
            void setValue(const std::string& name, const char& value);
            void setValue(const std::string& name, const bool& value);
            void setValue(const std::string& name, const std::string& value);
            void setValue(const std::string& name, const char* value);

            // Adds a value to this section, returns true if succesfull (that is, if there doesn't already exist a value with the same name)
            bool addValue(const iniValue& value);

            // Renames a value (the new name may not already exist), returns true if succesfull
            bool rename(const std::string& oldName, const std::string& newName);
            // Erase a value by name, returns true if succesfull
            bool erase(const std::string& name);
            // Erase the value where the iterator points to
            void erase(const iterator& pos);
            // Erase a range of values, from first (included) to last (excluded)
            void erase(const iterator& first, const iterator& last);
            // Checks whether a value exists
            bool valueExists(const std::string& name) const;

            // Gets a value by name
            iniValue& operator[](const std::string& name);
            iniValue operator[](const std::string& name) const;

            // Copies all the values from another section in this one, ignoring the name of the other section
            iniSection& operator=(const iniSection& other);

            // Get iterator to the beginning of the list of sections
            iterator begin();
            const_iterator begin() const;
            // Get reverse iterator to the beginning of the list of sections
            reverse_iterator rbegin();
            const_reverse_iterator rbegin() const;

            // Get iterator to the end of the list of sections
            iterator end();
            const_iterator end() const;
            // Get reverse iterator to the end of the list of sections
            reverse_iterator rend();
            const_reverse_iterator rend() const;

        private:
            std::string sectionName;
            std::vector<iniValue> values;
    };
}

#endif // INISECTION_H
