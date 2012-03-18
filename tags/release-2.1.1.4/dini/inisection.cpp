#include "inisection.h"
#include "dini_private.h"

namespace dini
{
// unknownName
    // Public:
        unknownName::unknownName(const std::string& name)
            :name(name){}

// iniSection
    // Public:
        iniSection::iniSection(const std::string& name)
            :sectionName(diniPrivate::validName(name)?name:"section"){}
        iniSection::iniSection(const std::string& name, const iniSection& other)
            :sectionName(diniPrivate::validName(name)?name:"section"), values(other.values){}

        std::string iniSection::name() const
        { return sectionName; }

        bool iniSection::setName(const std::string& name)
        {
            // Only rename if the new name is valid, return if the section was renamed or not
            if(diniPrivate::validName(name))
            {
                sectionName=name;
                return true;
            }
            return false;
        }

        void iniSection::clear()
        { values.clear(); }

        iniValue& iniSection::getValue(const std::string& name)
        {
            // Search for the value by name, if it's found, return it.
            // If it isn't found, create it and return the new value
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                    return *pos;
            }
            values.push_back(iniValue(name));
            return values.back();
        }

        iniValue iniSection::getValue(const std::string& name) const throw(unknownName)
        {
            // Search for the value by name, if it's found, return it.
            // If not, throw an error
            for(std::vector<iniValue>::const_iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                    return *pos;
            }
            throw unknownName(name);
        }

        void iniSection::setValue(const std::string& name, const iniValue& value)
        {
            // Search for the value by name, if it's found, assign the new value to it, if not, create it and assign the new value to it
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                {
                    *pos=value;
                    return;
                }
            }
            values.push_back(iniValue(name, value));
        }
        void iniSection::setValue(const std::string& name, const int& value)
        {
            // Search for the value by name, if it's found, assign the new value to it, if not, create it and assign the new value to it
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                {
                    pos->setValue(value);
                    return;
                }
            }
            values.push_back(iniValue(name, value));
        }
        void iniSection::setValue(const std::string& name, const double& value)
        {
            // Search for the value by name, if it's found, assign the new value to it, if not, create it and assign the new value to it
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                {
                    pos->setValue(value);
                    return;
                }
            }
            values.push_back(iniValue(name, value));
        }
        void iniSection::setValue(const std::string& name, const char& value)
        {
            // Search for the value by name, if it's found, assign the new value to it, if not, create it and assign the new value to it
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                {
                    pos->setValue(value);
                    return;
                }
            }
            values.push_back(iniValue(name, value));
        }
        void iniSection::setValue(const std::string& name, const bool& value)
        {
            // Search for the value by name, if it's found, assign the new value to it, if not, create it and assign the new value to it
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                {
                    pos->setValue(value);
                    return;
                }
            }
            values.push_back(iniValue(name, value));
        }
        void iniSection::setValue(const std::string& name, const std::string& value)
        {
            // Search for the value by name, if it's found, assign the new value to it, if not, create it and assign the new value to it
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                {
                    pos->setValue(value);
                    return;
                }
            }
            values.push_back(iniValue(name, value));
        }
        void iniSection::setValue(const std::string& name, const char* value)
        {
            // Search for the value by name, if it's found, assign the new value to it, if not, create it and assign the new value to it
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                {
                    pos->setValue(value);
                    return;
                }
            }
            values.push_back(iniValue(name, value));
        }

        bool iniSection::addValue(const iniValue& value)
        {
            // Check if the value doesn't already exist, if it doesn't, add it to the list of values and return true, if it doesn't return false
            if(valueExists(value.name()))
                return false;
            values.push_back(value);
            return true;
        }

        bool iniSection::rename(const std::string& oldName, const std::string& newName)
        {
            // If the new name already exists, or the new name isn't valid return false
            if(valueExists(newName) || !diniPrivate::validName(newName))
                return false;
            // Search for the value and change it's name, return if succesfull (which will always be because we've allready checked if the name is valid)
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(oldName==pos->name())
                    return pos->setName(newName);
            }
            return false;
        }
        bool iniSection::erase(const std::string& name)
        {
            // Search for the section, if we find it we erase it and return true, if not we return false
            for(std::vector<iniValue>::iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                {
                    values.erase(pos);
                    return true;
                }
            }
            return false;
        }
        void iniSection::erase(const iterator& pos)
        { values.erase(pos); }
        void iniSection::erase(const iterator& first, const iterator& last)
        { values.erase(first, last); }
        bool iniSection::valueExists(const std::string& name) const
        {
            // Search for the section, return true if we find it, if not: return false
            for(std::vector<iniValue>::const_iterator pos=values.begin(); pos!=values.end(); pos++)
            {
                if(name==pos->name())
                    return true;
            }
            return false;
        }

        iniValue& iniSection::operator[](const std::string& name)
        {return getValue(name);}
        iniValue iniSection::operator[](const std::string& name) const throw(unknownName)
        {return getValue(name);}

        iniSection& iniSection::operator=(const iniSection& other)
        {
            // Only copy the values of the other section, ignore it's name
            values=other.values;
            return *this;
        }

        iniSection::iterator iniSection::begin()
        { return values.begin(); }
        iniSection::const_iterator iniSection::begin() const
        { return values.begin(); }
        iniSection::reverse_iterator iniSection::rbegin()
        { return values.rbegin(); }
        iniSection::const_reverse_iterator iniSection::rbegin() const
        { return values.rbegin(); }

        iniSection::iterator iniSection::end()
        { return values.end(); }
        iniSection::const_iterator iniSection::end() const
        { return values.end(); }
        iniSection::reverse_iterator iniSection::rend()
        { return values.rend(); }
        iniSection::const_reverse_iterator iniSection::rend() const
        { return values.rend(); }
}
