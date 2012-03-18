#ifndef DINI_PRIVATE_H
#define DINI_PRIVATE_H

#include <string>

namespace diniPrivate
{
    bool validName(const std::string& str);
    bool strCaseCompare(const std::string& str1, const std::string& str2);
}

#endif // DINI_PRIVATE_H
