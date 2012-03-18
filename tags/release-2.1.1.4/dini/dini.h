#ifndef DINI_H
#define DINI_H

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

/************************************************** Help: ***************************************************
* Each header file contains some explanation on the functions and classes.                                  *
* Everything in this library is put in the namespace dini:: (which is the name of this library).            *
* You don't need any other libraries for this library,                                                      *
* except for the standard library of course, which is distributed with every C++ implementation.            *
* Just add all the *.h and *.cpp files to your project,                                                     *
* include dini.h where you need to use this library and you're ready to go!                                 *
*                                                                                                           *
* Some definitions which are usefull when reading the explanation in the headers:                           *
*    ini file                                                                                               *
*        A file using the ini structure (usually an *.ini file),                                            *
*        this file contains sections which each contain some values.                                        *
*        This is represented by the dini::iniFile class (in inifile.h)                                      *
*    ini section                                                                                            *
*        A section in an ini file, containing some values. Each section has a name.                         *
*        This is represented by the dini::iniSection class (in inisection.h)                                *
*    ini value                                                                                              *
*        A value in an ini section, it exists out of a name and a value                                     *
*        This is represented by the dini::iniValue class (ini inivalue.h)                                   *
************************************************************************************************************/

/********************************************* File structure: **********************************************
* The file structure used by this library is as follows.                                                    *
* Every section starts with a [section_name], and then a newline.                                           *
* Where section_name is of course the name of the section.                                                  *
* Then the values come, they are in the form name=value, and then a newline.                                *
* Some special characters (\, ;, =, newline ('\n'), cariage return ('\r') and null character ('\0'))        *
* are escaped, becoming (\\, \;, \=, \n, \r and \0).                                                        *
* These escape sequences will be parsed in to the original characters when reading the file.                *
* It's possible to use comment in the ini file.                                                             *
* When you write a ; (unescaped, so not: \;) the rest of the line will become commented.                    *
* Please note that any comments you make will be gone when saving the file again using this library.        *
*                                                                                                           *
* EXAMPLE:                                                                                                  *
* [section1]                                                                                                *
* name_of_value_in_section1=value in section1                                                               *
* another_value_name_in_section1=another value in section1;Some comment                                     *
* [section2]                                                                                                *
* name_of_value_in_section2=42                                                                              *
* ;etc                                                                                                      *
*                                                                                                           *
* For the names of the values and sections are some rules,                                                  *
* these are the same naming rules as in C++:                                                                *
*   1. The name may start with letters (capital or lower) or an underscore (_)                              *
*   2. After the first character of the name, letters (capital or lower),  underscores (_) and              *
*      digits (0-9) are allowed                                                                             *
*   3. Any other characters are not allowed, and the name may NOT start with a digit                        *
************************************************************************************************************/

#include "inifile.h"

#endif // DINI_H
