#ifndef INIVALUE_H
#define INIVALUE_H

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

#include <string>

namespace dini
{
    enum valueType
    {
        typeInt,
        typeDouble,
        typeChar,
        typeBool,
        typeString
    };

    // Represents an ini value, which is a pair of a name and a value
    class iniValue
    {
        public:
            // Constructs the iniValue using name, and an empty value
            iniValue(const std::string& name="name");
            // Constructs the iniValue using name, and the value of other (the name of other is ignored)
            iniValue(const std::string& name, const iniValue& other);
            // Constructors, the value needs a name and can be constructed from a boo, char, int, double, const char* or std::string
            iniValue(const std::string& name, const int& value);
            iniValue(const std::string& name, const double& value);
            iniValue(const std::string& name, const char& value);
            iniValue(const std::string& name, const bool& value);
            iniValue(const std::string& name, const std::string& value);
            iniValue(const std::string& name, const char* value);

            // Get the name of this iniValue
            std::string name() const;
            // Set the name of this iniValue, returns true if renamed succesfull (that is, if the name is a valid name)
            bool setName(const std::string& newName);

            // Checks if it's possible to convert to the given type
            bool validateType(const valueType& type) const;

            // Tries to convert to an int, if succesfull the int is returned, if not typeInt is thrown
            int toInt() const;
            // Tries to convert to a double, if succesfull the double is returned, if not typeDouble is thrown
            double toDouble() const;
            // Tries to convert to a char, if succesfull the char is returned, if not typeChar is thrown
            char toChar() const;
            // Tries to convert to a bool, if succesfull the bool is returned, if not typeBool is thrown
            bool toBool() const;
            // Returns the value of this iniValue as a string, note that this function is always succesfull (in contrary to the other conversion functions)
            std::string toString() const;

            // Copies the value of the other iniValue to this iniValue, ignoring the other's name
            void setValue(const iniValue& other);
            // Sets the value to the given value
            void setValue(const int& value);
            void setValue(const double& value);
            void setValue(const char& value);
            void setValue(const bool& value);
            void setValue(const std::string& value);
            void setValue(const char* value);

            // Copies the value of the other iniValue to this iniValue, ignoring the other's name
            iniValue& operator=(const iniValue& other);
            // Sets the value to the given value
            iniValue& operator=(const int& value);
            iniValue& operator=(const double& value);
            iniValue& operator=(const char& value);
            iniValue& operator=(const bool& value);
            iniValue& operator=(const std::string& value);
            iniValue& operator=(const char* value);

            // Adds the value of other to the value of this iniValue, by appending the other's value as a string to the value of this iniValue
            iniValue& operator+=(const iniValue& other);
            // Try to add an int to the current value, the current value will be converted to a double to do so, so this function can throw a typeDouble if it can't be converted
            iniValue& operator+=(const int& value);
            // Try to add a double to the current value, the current value will be converted to a double to do so, so this function can throw a typeDouble if it can't be converted
            iniValue& operator+=(const double& value);
            // Adds the char to the current value, by appending it as a string
            iniValue& operator+=(const char& value);
            // Try to add a bool to the current value, the current value will be converted to a double to do so, so this function can throw a typeDouble if it can't be converted
            iniValue& operator+=(const bool& value);
            // Adds a std::string to the current value, by appending it as a string
            iniValue& operator+=(const std::string& value);
            // Adds a const char* to the current value, by appending it as a string
            iniValue& operator+=(const char* value);

            // Substracts the value of other of the value of this iniValue, by converting both values to a double, so this function can throw a typeDouble if on of them can't be converted
            iniValue& operator-=(const iniValue& other);
            // Substracts a int, double, char or bool of the current value, the current value will be converted to a double to do so, so this function can throw a typeDouble if it can't be converted
            iniValue& operator-=(const int& value);
            iniValue& operator-=(const double& value);
            iniValue& operator-=(const char& value);
            iniValue& operator-=(const bool& value);

            // This is the same as calling !toBool()
            bool operator!() const;

            // Compares the value of this iniValue with the value of the other iniValue (ignoring the names)
            bool operator==(const iniValue& other) const;
            bool operator!=(const iniValue& other) const;

        private:
            std::string strName;
            std::string currValue;
    };

    // Function to convert an int to a string
    std::string intToString(const int& myInt);
    // Function to convert a double to a string, using the given precision
    std::string doubleToString(const double& myDouble, const int& precision=10);
    // Converting a bool to a string, returning "true" or "false"
    std::string boolToString(const bool& myBool);
}

#endif // INIVALUE_H
