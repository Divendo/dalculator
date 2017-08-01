#include "inifile.h"
#include "dini_private.h"

#include <fstream>
#include <cctype>

namespace dini
{
// fileError
    // Public:
        fileError::fileError(const std::string& filename, const errorType& type)
            :filename(filename), type(type){}

// errorCorrupted
    // Public:
        errorCorrupted::errorCorrupted(const std::string& lineData, const unsigned int& line, const corruptionType& type)
            :lineData(lineData), line(line), type(type){}

// iniFile
    // Public:
        iniSection& iniFile::getSection(const std::string& name)
        {
            // Search for the section, and if we find it, return it
            for(std::vector<iniSection>::iterator pos=sections.begin(); pos!=sections.end(); ++pos)
            {
                if(name==pos->name())
                    return *pos;
            }
            // If we don't find it, create an empty section with the name and return that empty section
            sections.push_back(iniSection(name));
            return sections.back();
        }

        iniSection iniFile::getSection(const std::string& name) const
        {
            // Search for the section, if we find it, return it, if not, throw an error
            for(std::vector<iniSection>::const_iterator pos=sections.begin(); pos!=sections.end(); ++pos)
            {
                if(name==pos->name())
                    return *pos;
            }
            throw unknownName(name);
        }

        void iniFile::setSection(const std::string& name, const iniSection& section)
        {
            // Search for the section, if we find it, assign the new section to it
            for(std::vector<iniSection>::iterator pos=sections.begin(); pos!=sections.end(); ++pos)
            {
                if(pos->name()==name)
                {
                    *pos=section;
                    return;
                }
            }
            // If we don't find it, just add the section to the list
            sections.push_back(iniSection(name, section));
        }

        bool iniFile::rename(const std::string& oldName, const std::string& newName)
        {
            // If the new name already exists, or the new name isn't valid return false
            if(sectionExists(newName) || !diniPrivate::validName(newName))
                return false;
            // Search for the section and change it's name, return if succesfull (which will always be because we've allready checked if the name is valid)
            for(std::vector<iniSection>::iterator pos=sections.begin(); pos!=sections.end(); ++pos)
            {
                if(oldName==pos->name())
                    return pos->setName(newName);
            }
            return false;
        }

        bool iniFile::erase(const std::string& name)
        {
            // Search for the section, if we find it we erase it and return true, if not we return false
            for(std::vector<iniSection>::iterator pos=sections.begin(); pos!=sections.end(); ++pos)
            {
                if(name==pos->name())
                {
                    sections.erase(pos);
                    return true;
                }
            }
            return false;
        }
        void iniFile::erase(const iterator& pos)
        { sections.erase(pos); }
        void iniFile::erase(const iterator& first, const iterator& last)
        { sections.erase(first, last); }

        bool iniFile::sectionExists(const std::string& name) const
        {
            // Search for the section, return true if we find it, if not: return false
            for(std::vector<iniSection>::const_iterator pos=sections.begin(); pos!=sections.end(); ++pos)
            {
                if(name==pos->name())
                    return true;
            }
            return false;
        }

        void iniFile::clear()
        { sections.clear(); }

        iniSection& iniFile::operator[](const std::string& name)
        { return getSection(name); }
        iniSection iniFile::operator[](const std::string& name) const
        { return getSection(name); }

        iniFile::iterator iniFile::begin()
        { return sections.begin(); }
        iniFile::const_iterator iniFile::begin() const
        { return sections.begin(); }
        iniFile::reverse_iterator iniFile::rbegin()
        { return sections.rbegin(); }
        iniFile::const_reverse_iterator iniFile::rbegin() const
        { return sections.rbegin(); }

        iniFile::iterator iniFile::end()
        { return sections.end(); }
        iniFile::const_iterator iniFile::end() const
        { return sections.end(); }
        iniFile::reverse_iterator iniFile::rend()
        { return sections.rend(); }
        iniFile::const_reverse_iterator iniFile::rend() const
        { return sections.rend(); }

        void iniFile::saveToFile(const std::string& filename) const
        {
            // Open file for writing, and check if it's openend succesfully
            std::ofstream file(filename.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
            if(!file.good())
            {
                file.close();
                throw fileError(filename, fileError::openForWritingError);
            }

            // Loop through all sections and write every section to the file
            for(std::vector<iniSection>::const_iterator pos=sections.begin(); pos!=sections.end(); ++pos)
            {
                // If something is wrong, close the file and throw an error
                if(!file.good())
                {
                    file.close();
                    throw fileError(filename, fileError::writeError);
                }
                // Write the section name to the file
                file<<'['<<pos->name()<<"]\n";
                // Loop through all values in the section and write each of them to the file
                for(iniSection::const_iterator pos2=pos->begin(); pos2!=pos->end(); ++pos2)
                {
                    // Write the name of the value to the file
                    file<<pos2->name()<<'=';
                    // Get the value, and escape any special characters
                    std::string value=pos2->toString();
                    for(std::string::const_iterator strPos=value.begin(); strPos!=value.end(); ++strPos)
                    {
                        switch(*strPos)
                        {
                            case '\\':
                            case ';':
                            case '=':    file<<'\\'<<*strPos;   break;

                            case '\n':  file<<"\\n";            break;
                            case '\r':  file<<"\\r";            break;
                            case '\0':  file<<"\\0";            break;
                            default:    file<<*strPos;          break;
                        }
                    }
                    file<<'\n';
                }
                file<<'\n';
            }
            file.close();
        }

        void iniFile::loadFromFile(const std::string& filename)
        {
            // Open file for reading, and check if it's opened succesfully
            std::ifstream file(filename.c_str(), std::ios_base::in | std::ios_base::binary);
            if(!file.good())
            {
                file.close();
                throw fileError(filename, fileError::openForReadingError);
            }

            // Clear all the data in this object
            clear();

            // String to temporaly store the current line of data
            std::string lineData;
            // Keep track of the line number in case an error has to be thrown
            // and read all of the lines in the file
            for(unsigned int line=1; std::getline(file, lineData); line++)
            {
                // If something is wrong, close the file and throw an error
                if(!file.good())
                {
                    file.close();
                    throw fileError(filename, fileError::readError);
                }
                // Remove any whitespaces and comments from the current line of data
                // If there isn't any data left after removing the whitespaces and comments,
                // we won't need to try to extract data from it
                if((lineData=removeWhitespacesAndComments(lineData))!="")
                {
                    try
                    {
                        // If it's start of a new section, create a new section
                        // If not, it has to be a value, so read the value and store it in the current section
                        // But if there isn't a section opened yet, something's wrong and we throw an error
                        if(lineData[0]=='[')
                            sections.push_back(sectionFromLine(lineData));
                        else if(sections.size() != 0)
                            sections.back().addValue(valueFromLine(lineData));
                        else
                            throw errorCorrupted(lineData, line, errorCorrupted::typeNoSection);
                    }
                    catch(errorCorrupted& err)
                    {
                        // Close the file, give some more info about the error (which line), and then rethrow the error
                        file.close();
                        err.line=line;
                        throw err;
                    }
                }
            }
            file.close();
        }

    // Private:
        std::string iniFile::removeWhitespacesAndComments(const std::string& line) const
        {
            // String to return
            std::string out="";
            // Whether we're still stripping whitespaces or not
            bool stripSpaces=true;

            // Loop through the whole string
            for(std::string::const_iterator pos=line.begin(); pos!=line.end(); pos++)
            {
                // Strip all the whitespaces until something else than a whitespace is found
                if(stripSpaces)
                {
                    if(!std::isspace(*pos))
                        stripSpaces=false;
                    else
                        continue;
                }
                // If we're done stripping whitespaces we'll be looking for comments
                // When a ';' is found without a \ before it, the rest of the line is commented (so the rest of the line is ignored)
                if(*pos==';' && (out.length()==0 || out[out.length()-1]!='\\'))
                    break;
                // If the current character wasn't a whitespace or comment, we append it to the string we'll return
                else
                    out+=*pos;
            }

            // Return the result
            return out;
        }

        iniSection iniFile::sectionFromLine(const std::string& line) const
        {
            // Search for the first '[', if it can't be found something's wrong
            const size_t start=line.find('[');
            if(start==std::string::npos)
                throw errorCorrupted(line, -1, errorCorrupted::typeSection);
            // Search for the first ']', after the '[' we just found
            // Again: if it can't be found, something's wrong
            const size_t end=line.find(']', start);
            if(end==std::string::npos)
                throw errorCorrupted(line, -1, errorCorrupted::typeSection);
            // If both are found, create a section with the name between the '[' and ']'
            return iniSection(line.substr(start+1, end-start-1));
        }

        iniValue iniFile::valueFromLine(const std::string& line) const
        {
            // We're going to return 'out', buffer is to temporary store a part of the line, and nameDone is true if the '=' is found
            iniValue out;
            std::string buffer="";
            bool nameDone=false;

            // Loop through the whole line
            for(std::string::const_iterator pos=line.begin(); pos!=line.end(); pos++)
            {
                // If the '=' isn't found yet
                if(!nameDone)
                {
                    // If the current character is the '=', the name is finished
                    // So we set the name of the value, clear the buffer and set 'nameDone' to true
                    if(*pos=='=')
                    {
                        out.setName(buffer);
                        buffer="";
                        nameDone=true;
                    }
                    // If the current character wasn't the '=', we check if the current character is a valid name character
                    // If it is, we add it to the buffer
                    else if( *pos=='_' || (*pos>='a' && *pos<='z') || (*pos>='A' && *pos<='Z') || (pos!=line.begin() && *pos>='0' && *pos<='9') )
                        buffer+=*pos;
                    // If it isn't we throw an error
                    else
                        throw errorCorrupted(line, -1, errorCorrupted::typeValue);
                }
                // If the '=' is found (so the whole name is found), the rest has to be the value
                else
                {
                    // If we found an escape character, we parse it and add it to the buffer
                    if(*pos=='\\')
                    {
                        if((++pos)==line.end())
                            throw errorCorrupted(line, -1, errorCorrupted::typeValue);
                        switch(*pos)
                        {
                            case '0':   buffer+='\0';   break;
                            case 'n':   buffer+='\n';   break;
                            case 'r':   buffer+='\r';   break;
                            default:    buffer+=*pos;   break;
                        }
                    }
                    // If it's a normal character, just add it to the buffer
                    else
                        buffer+=*pos;
                }
            }
            // The value is in the buffer, so store it in our iniValue object and return it
            out.setValue(buffer);
            return out;
        }
}
