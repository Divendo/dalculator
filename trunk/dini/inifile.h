#ifndef INIFILE_H
#define INIFILE_H

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

#include "inisection.h"
#include <string>
#include <vector>

namespace dini
{
    // Error class, thrown when an error occurs while processing a file
    class fileError
    {
        public:
            enum errorType              // Where the error occurred
            {
                writeError,             // While writing
                readError,              // While reading
                openForWritingError,    // While opening file for writing
                openForReadingError     // While opening file for reading
            };

            fileError(const std::string& filename, const errorType& type);

            std::string filename;       // Name of the file the error occurred in
            errorType type;             // Where the error occurred
    };

    // Error class, thrown when the raw ini data is corrupted
    class errorCorrupted
    {
        public:
            enum corruptionType     // When the corruption was found
            {
                typeSection,        // While parsing a section name
                typeValue,          // While parsing a value
                typeNoSection       // When a value was found while there isn't a section found yet
            };

            errorCorrupted(const std::string& lineData, const unsigned int& line, const corruptionType& type);

            std::string lineData;   // The raw data on the line where the error occurred
            unsigned int line;      // On which line the error occurred
            corruptionType type;    // When the corruption was found
    };

    // Class which represents a whole ini file
    // An ini file exists out of sections, which each exist out of values
    class iniFile
    {
        public:
            // Iterators
            typedef std::vector<iniSection>::iterator iterator;
            typedef std::vector<iniSection>::reverse_iterator reverse_iterator;
            typedef std::vector<iniSection>::const_iterator const_iterator;
            typedef std::vector<iniSection>::const_reverse_iterator const_reverse_iterator;

            // Get a section by name
            iniSection& getSection(const std::string& name);
            iniSection getSection(const std::string& name) const throw(unknownName);
            // Change the contents of an entire section
            void setSection(const std::string& name, const iniSection& section);
            // Rename a section (the new name may not already exist), returns true if the renaming was succesfull
            bool rename(const std::string& oldName, const std::string& newName);
            // Erase a section, returns true if the section was found and erased succesfull
            bool erase(const std::string& name);
            // Erase the value where the iterator points to
            void erase(const iterator& pos);
            // Erase a range of values, from first (included) to last (excluded)
            void erase(const iterator& first, const iterator& last);
            // Check whether a section exists
            bool sectionExists(const std::string& name) const;
            // Clear the whole file (remove all sections)
            void clear();

            // Get section by name
            iniSection& operator[](const std::string& name);
            iniSection operator[](const std::string& name) const;

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

            // Save all data to a ini file
            void saveToFile(const std::string& filename) const throw(fileError);
            // Load all data from a ini file
            void loadFromFile(const std::string& filename) throw(fileError, errorCorrupted);

        private:
            std::string removeWhitespacesAndComments(const std::string& line) const;
            iniSection sectionFromLine(const std::string& line) const throw(errorCorrupted);
            iniValue valueFromLine(const std::string& line) const throw(errorCorrupted);

            std::vector<iniSection> sections;
    };
}

#endif // INIFILE_H
