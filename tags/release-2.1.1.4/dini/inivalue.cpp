#include "inivalue.h"
#include "dini_private.h"

#include <sstream>
#include <iomanip>

namespace dini
{
// iniValue
    // Public:
        iniValue::iniValue(const std::string& name)
            :strName(diniPrivate::validName(name)?name:"name"), currValue(""){}
        iniValue::iniValue(const std::string& name, const iniValue& other)
            :strName(diniPrivate::validName(name)?name:"name"), currValue(other.currValue){}

        iniValue::iniValue(const std::string& name, const int& value)
            :strName(diniPrivate::validName(name)?name:"name"), currValue(intToString(value)){}
        iniValue::iniValue(const std::string& name, const double& value)
            :strName(diniPrivate::validName(name)?name:"name"), currValue(doubleToString(value)){}
        iniValue::iniValue(const std::string& name, const char& value)
            :strName(diniPrivate::validName(name)?name:"name"), currValue(1, value){}
        iniValue::iniValue(const std::string& name, const bool& value)
            :strName(diniPrivate::validName(name)?name:"name"), currValue(boolToString(value)){}
        iniValue::iniValue(const std::string& name, const std::string& value)
            :strName(diniPrivate::validName(name)?name:"name"), currValue(value){}
        iniValue::iniValue(const std::string& name, const char* value)
            :strName(diniPrivate::validName(name)?name:"name"), currValue(value){}

        std::string iniValue::name() const
        { return strName; }
        bool iniValue::setName(const std::string& newName)
        {
            // Check if the name is valid, if it is, rename and return true, if not, return false
            if(diniPrivate::validName(newName))
            {
                strName=newName;
                return true;
            }
            return false;
        }

        bool iniValue::validateType(const valueType& type) const
        {
            // Just try converting to the given value type, if that doesn't throw any errors, we return true, if it does throw errors we return false
            try
            {
                switch(type)
                {
                    case typeInt:       toInt();    break;
                    case typeDouble:    toDouble(); break;
                    case typeChar:      toChar();   break;
                    case typeBool:      toBool();   break;
                    case typeString:    return true;        // Converting to a string will always be succesfull
                }
            }
            catch(...)
            { return false; }
            return true;
        }

        int iniValue::toInt() const throw(valueType)
        {
            // Try converting the string to an int, if it fails, throw an error
            int out=0;
            std::stringstream stream(currValue);
            stream>>out;
            if(stream.fail())
                throw typeInt;
            return out;
        }
        double iniValue::toDouble() const throw(valueType)
        {
            // Try converting the string to a double, if it fails, throw an error
            double out=0;
            std::stringstream stream(currValue);
            stream>>out;
            if(stream.fail())
                throw typeDouble;
            return out;
        }
        char iniValue::toChar() const throw(valueType)
        {
            // Just return the first character of the string, this can't be done if the string is empty
            if(currValue.length()!=1)
                throw typeChar;
            return currValue[0];
        }
        bool iniValue::toBool() const throw(valueType)
        {
            // "0" or "false" gives false, "true" or any other number gives true
            if(currValue=="0" || diniPrivate::strCaseCompare(currValue, "false"))
                return false;
            if(diniPrivate::strCaseCompare(currValue, "true"))
                return true;
            try
            { return toDouble()!=0; }
            catch(...)
            { throw typeBool; }
        }
        std::string iniValue::toString() const
        { return currValue; }

        void iniValue::setValue(const iniValue& other)
        { currValue = other.currValue; }
        void iniValue::setValue(const int& value)
        { currValue=intToString(value); }
        void iniValue::setValue(const double& value)
        { currValue=doubleToString(value); }
        void iniValue::setValue(const char& value)
        { currValue=value; }
        void iniValue::setValue(const bool& value)
        { currValue=boolToString(value); }
        void iniValue::setValue(const std::string& value)
        { currValue=value; }
        void iniValue::setValue(const char* value)
        { currValue=value; }

        iniValue& iniValue::operator=(const iniValue& other)
        { currValue=other.currValue; return *this; }
        iniValue& iniValue::operator=(const int& value)
        { setValue(value); return *this; }
        iniValue& iniValue::operator=(const double& value)
        { setValue(value); return *this; }
        iniValue& iniValue::operator=(const char& value)
        { setValue(value); return *this; }
        iniValue& iniValue::operator=(const bool& value)
        { setValue(value); return *this; }
        iniValue& iniValue::operator=(const std::string& value)
        { setValue(value); return *this; }
        iniValue& iniValue::operator=(const char* value)
        { setValue(value); return *this; }

        iniValue& iniValue::operator+=(const iniValue& other)
        { currValue+=other.currValue; return *this; }
        iniValue& iniValue::operator+=(const int& value) throw(valueType)
        { setValue(toDouble()+value); return *this; }
        iniValue& iniValue::operator+=(const double& value) throw(valueType)
        { setValue(toDouble()+value); return *this; }
        iniValue& iniValue::operator+=(const char& value)
        { currValue+=value; return *this; }
        iniValue& iniValue::operator+=(const bool& value) throw(valueType)
        { setValue(toDouble()+value); return *this; }
        iniValue& iniValue::operator+=(const std::string& value)
        { currValue+=value; return *this; }
        iniValue& iniValue::operator+=(const char* value)
        { currValue+=value; return *this; }

        iniValue& iniValue::operator-=(const iniValue& other) throw(valueType)
        { setValue(toDouble()-other.toDouble()); return *this; }
        iniValue& iniValue::operator-=(const int& value) throw(valueType)
        { setValue(toDouble()-value); return *this; }
        iniValue& iniValue::operator-=(const double& value) throw(valueType)
        { setValue(toDouble()-value); return *this; }
        iniValue& iniValue::operator-=(const char& value) throw(valueType)
        { setValue(toDouble()-value); return *this; }
        iniValue& iniValue::operator-=(const bool& value) throw(valueType)
        { setValue(toDouble()-value); return *this; }

        bool iniValue::operator!() const throw(valueType)
        { return !toBool(); }

        bool iniValue::operator==(const iniValue& other) const
        { return currValue==other.currValue; }
        bool iniValue::operator!=(const iniValue& other) const
        { return currValue!=other.currValue; }

// Functions:
    std::string intToString(const int& myInt)
    {
        // Create a stream, put the int in it, and return the stream as a string
        std::ostringstream out;
        out<<myInt;
        return out.str();
    }

    std::string doubleToString(const double& myDouble, const int& precision)
    {
        // Create a stream, set the precision and put the double in it, and return the stream as a string
        std::ostringstream out;
        out<<std::setprecision(precision)<<myDouble;
        return out.str();
    }

    std::string boolToString(const bool& myBool)
    { return myBool?"true":"false"; }
}
