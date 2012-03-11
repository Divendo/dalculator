#include "dini_private.h"

#include <cctype>

namespace diniPrivate
{
    bool validName(const std::string& str)
    {
        if(str.length()==0)
            return false;
        for(std::string::const_iterator pos=str.begin(); pos!=str.end(); pos++)
        {
            if( !(*pos=='_' || (*pos>='a' && *pos<='z') || (*pos>='A' && *pos<='Z') || (pos!=str.begin() && *pos>='0' && *pos<='9')) )
                return false;
        }
        return true;
    }

    bool strCaseCompare(const std::string& str1, const std::string& str2)
    {
        if(str1.length()!=str2.length())
            return false;
        for(unsigned int i=0; i<str1.length(); i++)
        {
            if(std::tolower(str1[i])!=std::tolower(str2[i]))
                return false;
        }
        return true;
    }
}
