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

#include "calc.h"
#include "calc_private.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace calc
{
    // Public:
        // Constructor
        calc::calc(const string& expr, const bool& cleanFunctionsUp)
        : currExpr(expr), expressionParsed(false), cleanFunctionsUp(cleanFunctionsUp) {}

        // Destructor, cleans up the functions if told to do so
        calc::~calc()
        {
            if(cleanFunctionsUp)
            {
                for(functionList::iterator pos = calc::currFunctions.begin(); pos != calc::currFunctions.end(); ++pos)
                {
                    if(pos->second->cleanUpNeeded())                            // Only delete the function if it needs a clean up (i.e. it's allocated using 'new')
                    {
                        delete pos->second;
                        pos->second = 0;
                    }
                }
            }
        }

        void calc::setExpression(const string& expr)
        {
            currExpr = expr;
            expressionParsed = false;
            errors.clear();
            tokens.clear();
        }

        string calc::getExpression() const
        { return currExpr; }

        // Functions for the variables
        bool calc::deleteVar(const string& name)
        { return calc::currVars.erase(name); }

        real& calc::setVar(const string& name, const real& value)
        { return calc::currVars[name] = value; }

        void calc::setVarlist(const varList& newVars)
        { calc::currVars = newVars; }

        bool calc::renameVar(const string& oldName, const string& newName)
        {
            if(calc::currVars.count(oldName) && !calc::currVars.count(newName))
            {
                calc::currVars[newName] = calc::currVars[oldName];
                calc::currVars.erase(oldName);
                return true;
            }
            return false;
        }

        real& calc::getVar(const string& name)
        { return calc::currVars[name]; }

        real calc::getVar(const string& name) const
        {
            if(calc::currVars.count(name))
                return calc::currVars[name];
            return 0;
        }

        bool calc::varExists(const string& name) const
        { return calc::currVars.count(name); }

        const varList* calc::getVars() const
        { return &calc::currVars; }

        // Functions for the functions
        bool calc::deleteFunction(const string& name)
        {
            if(calc::currFunctions[name]->cleanUpNeeded())
                delete calc::currFunctions[name];
            return calc::currFunctions.erase(name);
        }

        void calc::setFunction(const string& name, mathFunction* function)
        {
            if(calc::currFunctions.count(name) && calc::currFunctions[name]->cleanUpNeeded())
                delete calc::currFunctions[name];
            calc::currFunctions[name] = function;
        }
        mathFunction* calc::getFunction(const string& name)
        {
            if(calc::currFunctions.count(name))
                return calc::currFunctions[name];
            return 0;
        }
        const mathFunction* calc::getFunction(const string& name) const
        {
            if(calc::currFunctions.count(name))
                return calc::currFunctions[name];
            return 0;
        }
        bool calc::renameFunction(const string& oldName, const string& newName)
        {
            if(calc::currFunctions.count(oldName) && !calc::currFunctions.count(newName))
            {
                calc::currFunctions[newName] = calc::currFunctions[oldName];
                calc::currFunctions.erase(oldName);
                return true;
            }
            return false;
        }

        bool calc::functionExists(const string& name) const
        { return calc::currFunctions.count(name); }

        const functionList* calc::getFunctions() const
        { return &calc::currFunctions; }

        // Parsing functions
        void calc::parse()
        {
            if(!expressionParsed)
                forceParse();
        }

        bool calc::parsed() const
        { return expressionParsed; }

        void calc::forceParse()
        {
            // Clear all tokens and errors
            tokens.clear();
            errors.clear();

            // Initialise some variables we'll need for parsing
            Token::Type currType = Token::tokenUnknown;                         // The type of the current token
            string buffer = "";                                                 // A buffer to temporarily store a part of the expression

            // Loop through the whole expression
            for(string::const_iterator pos = currExpr.begin(); pos != currExpr.end(); ++pos)
            {
                if(currType == Token::tokenName)
                {
                    // If the current character can't be part of the name, the end of the name is found
                    // If the current character is part of the name, we add it to the buffer
                    if(!calcPrivate::isNameChar(*pos, false))
                    {
                        tokens.push_back(Token(currType, buffer));
                        currType = Token::tokenUnknown;
                        buffer = "";
                    }
                    else
                    {
                        buffer += *pos;
                        continue;
                    }
                }
                else if(isWhitespace(*pos))
                {
                    tokens.push_back(Token(Token::tokenWhitespace, *pos));
                    continue;
                }
                if(currType == Token::tokenReal)
                {
                    // If the following conditions are all true, the end of the real value is found
                    //  1. The current character isn't a part of the real value,
                    //     of course a real value ends when a character other than a character from a real value is found
                    //  2. Not when the current character is a '-' or '+' and the last character in the buffer is an 'e' or 'E'
                    //     This is an exception on condition 1, after an 'e' or 'E' a '+' or '-' is allowed (for scientific notation: 1.23e+45)
                    // If one of the conditions is false, the current character is just added to the buffer
                    if(!isReal(*pos, buffer[0] == '-' ? buffer.substr(1) : buffer) && !( (*pos=='-' || *pos=='+') && (buffer[buffer.length()-1]=='e' || buffer[buffer.length()-1]=='E') ))
                    {
                        tokens.push_back(Token(currType, buffer));
                        currType = Token::tokenUnknown;
                        buffer = "";
                    }
                    else
                    {
                        buffer += *pos;
                        continue;
                    }
                }

                // If the current character is an operator, add the operator to the token list
                if(isOperator(*pos))
                {
                    if(*pos == '-')                                             // In case of a minus we need to find out whether it's unary or not
                    {
                        // Find out whether the minus is an unary minus or not
                        bool unaryMin = !tokens.size();                         // If the size of the tokens list is 0, it's an unary minus for sure
                        if(!unaryMin)                                           // If it isn't an unary minus for sure, we'll have to find out if it is
                        {
                            switch(tokens.back().type)                          // For finding out, we're looking at the type of the last token
                            {
                                case Token::tokenOperator:                      // After an operator,
                                case Token::tokenComma:                         // comma,
                                case Token::tokenOpenBracket:                   // opening bracket
                                case Token::tokenAssignmentOperator:            // or assignment operator
                                    unaryMin = true;                            // It's an unary minus for sure
                                break;

                                default:                                        // Otherwise it's a regular binary minus
                                    tokens.push_back(Token(Token::tokenOperator, "-"));
                                break;
                            }
                        }

                        // If the minus is an unary minus, we'll need to find out what kind of token is following
                        if(unaryMin)
                        {
                            string::const_iterator nextChr = pos+1;
                            while(nextChr != currExpr.end() && isWhitespace(*nextChr))
                                ++nextChr;
                            if(nextChr == currExpr.end())
                                tokens.push_back(Token(Token::tokenOperator, "-"));
                            else
                            {
                                if(isReal(*nextChr))
                                {
                                    currType = Token::tokenReal;
                                    buffer += *pos;
                                }
                                else if(calcPrivate::isNameChar(*nextChr, true))
                                {
                                    currType = Token::tokenName;
                                    buffer += *pos;
                                }
                                else if(*nextChr=='(')
                                {
                                    tokens.push_back(Token(Token::tokenOpenBracket, "-("));
                                    pos = nextChr;
                                }
                                else
                                    tokens.push_back(Token(Token::tokenOperator, "-"));
                            }
                        }
                    }
                    else if(*pos == '=')
                        tokens.push_back(Token(Token::tokenAssignmentOperator, "="));   // There is no '=' allowed as first token, but the error-checking function will filter that out
                    else
                        tokens.push_back(Token(Token::tokenOperator, *pos));
                    continue;
                }

                // Determine what type of token the current character is or is starting
                if(isReal(*pos))
                {
                    currType = Token::tokenReal;
                    buffer += *pos;
                }
                else if(calcPrivate::isNameChar(*pos, true))
                {
                    currType = Token::tokenName;
                    buffer += *pos;
                }
                else if(*pos=='(')
                    tokens.push_back(Token(Token::tokenOpenBracket, "("));
                else if(*pos==')')
                    tokens.push_back(Token(Token::tokenCloseBracket, ")"));
                else if(*pos==',')
                    tokens.push_back(Token(Token::tokenComma, ","));
                else
                    tokens.push_back(Token(Token::tokenUnknown, *pos));
            }

            // If the last token wasn't added to the token list yet because the expression string ended
            // we'll add it now
            if(currType != Token::tokenUnknown)
                tokens.push_back(Token(currType, buffer));

            // Search for errors, and add them to the error vector
            searchForErrors();

            // Remember that we parsed the expression
            expressionParsed = true;
        }

        const std::vector<calcError>* calc::getParseErrors()
        { return &errors; }

        bool calc::isValidExpression() const
        { return expressionParsed && errors.size() == 0; }

        real calc::calculate(const string& newExpr)
        {
            setExpression(newExpr);
            return calculate();
        }

        real calc::calculate()
        {
            // Check if parsing is needed
            if(!expressionParsed)
                forceParse();

            // Check if any errors where found while checking for errors
            if(errors.size())
                throw errors[0];

            // Backup all tokens, this backup needs to be restored when this function is done
            std::vector<Token> tokensBackup = tokens;

            try
            {
                // Remove all whitespaces from the tokens
                for(std::vector<Token>::iterator pos = tokens.begin(); pos != tokens.end(); )
                {
                    if(pos->type == Token::tokenWhitespace)
                        pos = tokens.erase(pos);
                    else
                        ++pos;
                }

                // Execute all tokens in the proper order
                while(calcDoBrackets());
                while(calcDoFunctions());
                while(calcDoPowers());
                while(calcDoMultiply());
                while(calcDoPlusmin());
                while(calcDoCompare());
                while(calcDoBitwise());
                while(calcDoAssignments());

                // In the end only one token may be left
                if(!tokens.size())
                    throw calcError("Unknown error occurred", calcError::unknown);

                // Remeber the remaining token en restore the backup of the tokens
                Token lastToken = tokens[0];
                tokens = tokensBackup;

                // Find out the type of the remaining token
                switch(lastToken.type)
                {
                    // In case of a variable, real that still needs to be converted or a real that is already converted
                    // We return the value of it
                    case Token::tokenName:
                    return tokenToValue(lastToken);

                    case Token::tokenReal:
                        if(lastToken.str.find(':') != string::npos)
                            return timestr2real(lastToken.str);
                        else
                            return str2real(lastToken.str);

                    case Token::tokenRealReal:
                    return lastToken.val;

                    // In any other case, something went wrong
                    default:
                    throw calcError("Unknown error occurred", calcError::unknown);
                }
            }
            catch(calcError&)
            {
                tokens = tokensBackup;
                throw;
            }
            catch(std::exception& exc)
            {
                tokens = tokensBackup;
                throw calcError("STL error occurred", calcError::unknown, exc.what());
            }
            catch(...)
            {
                tokens = tokensBackup;
                throw calcError("Unknown error occurred", calcError::unknown);
            }
        }

    // Private:
        // Static:
            varList calc::currVars = varList();
            functionList calc::currFunctions = functionList();

        void calc::searchForErrors()
        {
            // Check if the expression was empty, if it is report an error
            if(!tokens.size())
                errors.push_back(calcError("Empty expression", calcError::emptyExpression));

            // Initialise some variables needed while checking for errors
            int openBrackets = 0;                                               // The number of open brackets
            Token* lastToken = 0;                                               // Pointer to the last token
            std::vector<int> functionsOpen;                                     // Used to determine whether a closing bracket is closing a function or closing a normal bracket
            unsigned int currTextPosition = 0;                                  // The current position in the expression

            // These types of tokens are accepted as next token
            std::vector<idType> acceptedTokens;
            acceptedTokens.push_back(Token::tokenReal);
            acceptedTokens.push_back(Token::tokenRealReal);
            acceptedTokens.push_back(Token::tokenName);
            acceptedTokens.push_back(Token::tokenOpenBracket);

            // Loop through all tokens
            for(std::vector<Token>::iterator pos = tokens.begin(); pos != tokens.end(); ++pos)
            {
                // Remember the current position in the expression
                currTextPosition += pos->str.length();

                // We ignore whitespaces
                if(pos->type == Token::tokenWhitespace)
                    continue;
                // Unknown tokens are not allowed
                if(pos->type == Token::tokenUnknown)
                {
                    // Report an error, showing the token and it's position
                    errors.push_back(calcError("Unknown token", calcError::unknownToken, pos->str, currTextPosition-pos->str.length()));
                    continue;
                }
                // If the current token isn't an accepted one
                if(std::find(acceptedTokens.begin(), acceptedTokens.end(), pos->type) == acceptedTokens.end())
                {
                    // Report an error containing the token and it's position
                    errors.push_back(calcError("Unexpected token", calcError::unexpectedToken, pos->str, currTextPosition-pos->str.length()));
                    continue;
                }

                // If the token is valid, check what token it is and set the tokens that are accepted for the next token
                acceptedTokens.clear();
                switch(pos->type)
                {
                    default:                                                    // Just to keep the compiler from giving warnings that some enum values are not handled

                    case Token::tokenOperator:
                        acceptedTokens.push_back(Token::tokenReal);
                        acceptedTokens.push_back(Token::tokenRealReal);
                        acceptedTokens.push_back(Token::tokenName);
                        acceptedTokens.push_back(Token::tokenOpenBracket);
                    break;

                    case Token::tokenReal:
                    case Token::tokenRealReal:
                        acceptedTokens.push_back(Token::tokenOperator);
                        if(functionsOpen.size() > 0)
                            acceptedTokens.push_back(Token::tokenComma);
                        if(openBrackets >= 1)
                            acceptedTokens.push_back(Token::tokenCloseBracket);
                    break;

                    case Token::tokenName:
                        acceptedTokens.push_back(Token::tokenOperator);
                        acceptedTokens.push_back(Token::tokenOpenBracket);
                        acceptedTokens.push_back(Token::tokenAssignmentOperator);

                        if(functionsOpen.size() > 0)
                            acceptedTokens.push_back(Token::tokenComma);
                        if(openBrackets >= 1)
                            acceptedTokens.push_back(Token::tokenCloseBracket);
                    break;

                    case Token::tokenOpenBracket:
                        ++openBrackets;
                        if(lastToken!=0 && lastToken->type == Token::tokenName)
                        {
                            lastToken->type = Token::tokenFunctionStart;
                            functionsOpen.push_back(openBrackets);
                        }

                        acceptedTokens.push_back(Token::tokenReal);
                        acceptedTokens.push_back(Token::tokenRealReal);
                        acceptedTokens.push_back(Token::tokenName);
                        acceptedTokens.push_back(Token::tokenCloseBracket);
                        acceptedTokens.push_back(Token::tokenOpenBracket);
                    break;

                    case Token::tokenCloseBracket:
                        if(functionsOpen.size()>0 && functionsOpen.back() == openBrackets)
                            functionsOpen.pop_back();
                        --openBrackets;

                        acceptedTokens.push_back(Token::tokenOperator);
                        if(functionsOpen.size() > 0)
                            acceptedTokens.push_back(Token::tokenComma);
                        if(openBrackets >= 1)
                            acceptedTokens.push_back(Token::tokenCloseBracket);
                    break;

                    case Token::tokenComma:
                        acceptedTokens.push_back(Token::tokenReal);
                        acceptedTokens.push_back(Token::tokenRealReal);
                        acceptedTokens.push_back(Token::tokenName);
                        acceptedTokens.push_back(Token::tokenOpenBracket);
                    break;

                    case Token::tokenAssignmentOperator:
                        acceptedTokens.push_back(Token::tokenReal);
                        acceptedTokens.push_back(Token::tokenRealReal);
                        acceptedTokens.push_back(Token::tokenName);
                        acceptedTokens.push_back(Token::tokenOpenBracket);
                    break;
                }

                // Remember the last token
                lastToken = &(*pos);
            }


            // If the last token is a token that may not be the last token, we report an error containing the token and it's position
            if(lastToken != 0 && (lastToken->type == Token::tokenOperator || lastToken->type == Token::tokenAssignmentOperator || lastToken->type == Token::tokenComma))
                errors.push_back(calcError("Unexpected token", calcError::unexpectedToken, lastToken->str, currTextPosition-lastToken->str.length()));

            // If there are still any brackets open, report an error and tell how many brackets are open
            if(openBrackets >= 1)
                errors.push_back(calcError("Unclosed bracket(s)", calcError::unclosedBracket, openBrackets));
        }

        bool calc::isOperator(const char& chr) const
        {
            return
                chr == '^' ||  chr == '~' ||  chr == '*' ||
                chr == '/' ||  chr == '%' ||  chr == '+' ||
                chr == '-' ||  chr == '<' ||  chr == '>' ||
                chr == '&' ||  chr == '|' ||  chr == '=';
        }

        bool calc::isReal(const char& chr, const string& firstStrPart) const
        {
            if(firstStrPart.empty())
                return chr == '.' || chr == ':' || std::isdigit(chr);
            else if(firstStrPart[0] == '0')
            {
                if(firstStrPart.size() == 1)
                    return chr == 'x' || chr == 'X' || chr == '.' || chr == ':' || chr == 'e' || chr == 'E' || std::isdigit(chr);
                else if(firstStrPart[1] == 'x' || firstStrPart[1] == 'X')
                    return std::isxdigit(chr);
            }
            return chr == '.' || chr == ':' || chr == 'e' || chr == 'E' || std::isdigit(chr);
        }

        bool calc::isWhitespace(const char& chr) const
        { return std::isspace(chr); }

        real calc::tokenToValue(const Token& token) const
        try
        {
            switch(token.type)
            {
                // In case the token is a real that's already converted, just return it's value
                case Token::tokenRealReal:
                return token.val;

                // In case it's a real that still needs to be parsed, parse it and return the result
                case Token::tokenReal:
                    if(token.str.find(':')!=string::npos)
                        return timestr2real(token.str);
                    else
                        return str2real(token.str);

                // In case of a name, look it up in the list of variables and return it's value if it exists otherwise throw an error
                case Token::tokenName:
                {
                    const bool unaryMin = (token.str[0]=='-');
                    if(!calc::currVars.count(unaryMin ? token.str.substr(1) : token.str))
                        throw calcError("Unknown variable", calcError::unknownName, token.str);
                    return calc::currVars[unaryMin ? token.str.substr(1) : token.str] * (unaryMin ? -1 : 1);
                }

                // If it's any other type than named above, something's wrong so we throw an error
                default:
                throw calcError("Unexpected token id", calcError::unexpectedToken, token.str, token.type);
            }
        }
        catch(calcError)
        { throw; }
        catch(...)
        { throw calcError("Unknown error occurred", calcError::unknown); }

        bool calc::calcDoBrackets()
        {
            // Initialise some variables needed for the handling of the brackets
            unsigned int bracketsOpen = 0;                                      // How many brackets are opened
            bool functionOpen = false;                                          // Whether the current opened brackets are from a function or are just regular brackets
            std::vector<Token>::iterator pos = tokens.begin();                  // The current position in the vector of tokens, needs to be visible after the loop
            std::vector<Token>::iterator lastOpenBracket = tokens.end();        // Remember the position of the last opening bracket we found
            std::vector<Token>::const_iterator lastToken = tokens.end();        // Remember the last token

            // Loop through all tokens
            for(; pos != tokens.end(); ++pos)
            {
                // If we find an opening bracket
                if(pos->type == Token::tokenOpenBracket)
                {
                    if(bracketsOpen == 0 && lastToken != tokens.end() && lastToken->type == Token::tokenFunctionStart)
                        functionOpen = true;
                    else if(bracketsOpen++ == 0)
                    {
                        lastOpenBracket = pos;
                        functionOpen = false;
                    }
                    lastToken = pos;
                    continue;
                }

                // If there currently is no function opened and we've found a closing bracket
                // Look whether all opening brackets we found are closed, and if so we break out the loop in order to calculate everything between the brackets
                if(!functionOpen && pos->type == Token::tokenCloseBracket)
                {
                    lastToken = pos;
                    if(--bracketsOpen == 0)
                        break;
                }

                // Remember the last token
                lastToken = pos;
            }

            // If we didn't find any opening brackets there is no need to call this function any more
            if(lastOpenBracket == tokens.end())
                return false;

            // Create a calculator and calculate the tokens between the pair of brackets
            // We don't want the temporary calculator to clean the math functions up, so we pass false as the second argument
            // After that we replace the opening bracket by the result of it and remove all tokens after it
            calc tmp("", false);
            tmp.expressionParsed = true;
            tmp.tokens.insert(tmp.tokens.begin(), lastOpenBracket+1, pos);
            *lastOpenBracket = Token(Token::tokenRealReal, "", tmp.calculate() * (lastOpenBracket->str[0] == '-' ? -1 : 1));
            tokens.erase(lastOpenBracket+1, pos+1);

            // It might be necessary for this function to be called again
            return true;
        }

        bool calc::calcDoFunctions()
        {
            // Note that this function relies on calcDoBrackets() being called first untill it returns false
            // Otherwise this function might not find and execute all functions in the vector with tokens

            // Initialise some variables needed for the handling of the brackets
            unsigned int bracketsOpen = 0;                                      // How many brackets are opened
            std::vector<Token>::iterator pos = tokens.begin();                  // The current position in the vector of tokens, needs to be visible after the loop
            std::vector<Token>::iterator functionStart = tokens.end();          // Points to the token that contains the start of the function

            // Loop through all tokens
            for(; pos!=tokens.end(); ++pos)
            {
                // If we found the start of a function and we didn't find one before we remember where the function starts
                if(pos->type == Token::tokenFunctionStart && functionStart == tokens.end())
                {
                    functionStart = pos++;                                      // Increasing pos because a tokenFunctionStart is always followed by a tokenOpenBracket
                    ++bracketsOpen;
                    continue;
                }
                // Keep track of the number of opened brackets
                else if(pos->type == Token::tokenOpenBracket)
                    ++bracketsOpen;
                // and if they're all closed we're done
                else if(pos->type == Token::tokenCloseBracket && --bracketsOpen == 0)
                    break;
            }

            // If no function is found, this function won't need to be called any more
            if(functionStart == tokens.end())
                return false;

            // Check whether the function is preceded with an unary minus
            // If so, erase the minus from the string
            const bool unaryMin = (functionStart->str[0] == '-');
            if(unaryMin)
                functionStart->str.erase(0, 1);

            // Check if the function exists, if it doesn't we throw an error
            if(calc::currFunctions.count(functionStart->str) == 0)
                throw calcError("Unknown function", calcError::unknownName, functionStart->str);

            // Create a temporary calculator to calculate the arguments of the function
            // We don't want the temporary calculator to clean the math functions up, so we pass false as the second argument
            calc tmp("", false);
            tmp.expressionParsed = true;
            argList functionVarList;
            bracketsOpen = 0;

            // Loop through all arguments in the function,
            // so we're looping from the first token after the opening bracket of the function (functionStart+2)
            // untill the closing bracket of the function (exclusive), that is the position stored in pos
            for(std::vector<Token>::const_iterator argPos = functionStart+2; argPos != pos; ++argPos)
            {
                if(argPos->type == Token::tokenOpenBracket)
                    ++bracketsOpen;
                else if(argPos->type == Token::tokenCloseBracket)
                    --bracketsOpen;
                else if(argPos->type == Token::tokenComma && bracketsOpen == 0)
                {
                    functionVarList.push_back(tmp.calculate());
                    tmp.tokens.clear();
                    continue;
                }
                tmp.tokens.push_back(*argPos);
            }
            // Make sure we don't forget to add the last argument
            if(tmp.tokens.size())
                functionVarList.push_back(tmp.calculate());

            // Replace the token at the start of the function with the result of the function and remove all tokens untill the closing bracket (inclusive)
            *functionStart = Token(Token::tokenRealReal, "", calc::currFunctions[functionStart->str]->execute(functionVarList, functionStart->str) * (unaryMin ? -1 : 1) );
            tokens.erase(functionStart+1, pos+1);

            // It might be necessary to call this function again
            return true;
        }

        bool calc::calcDoPowers()
        {
            // Loop through all tokens
            for(std::vector<Token>::iterator pos = tokens.begin(); pos != tokens.end(); ++pos)
            {
                // If the current token is an operator and it's an power (^) or root (~) there's something to do
                if(pos->type == Token::tokenOperator && (pos->str=="^" || pos->str=="~"))
                {
                    // Get the operands
                    const real firstVal = tokenToValue(*(pos-1));
                    const real secondVal = tokenToValue(*(pos+1));

                    // Only allow integer powers of negative numbers
                    if(firstVal < 0)
                    {
                        if(pos->str[0] != '^')
                            throw calcError("No negative roots allowed", calcError::invalidOperands);

                        if(std::floor(secondVal) != secondVal)
                            throw calcError("Only integer powers of negative numbers", calcError::invalidOperands);
                    }

                    // Replace the current token by the result of it's operation and erase the operands
                    *pos = Token(Token::tokenRealReal, "", std::pow(firstVal, pos->str[0]=='^' ? secondVal : 1/secondVal) );
                    pos = tokens.erase(pos-1);
                    tokens.erase(pos+1);

                    // It might be necessary to call this function again
                    return true;
                }
            }

            // There is no need to call this function again, since there are no power or root operators left
            return false;
        }

        bool calc::calcDoMultiply()
        {
            // Loop through all tokens
            for(std::vector<Token>::iterator pos = tokens.begin(); pos != tokens.end(); ++pos)
            {
                // If the current token is an operator
                if(pos->type == Token::tokenOperator)
                {
                    // Get the value of the second operand
                    const real secondVal = tokenToValue(*(pos+1));

                    // In case it's a multiply operator
                    if(pos->str == "*")
                    {
                        // Replace the current token by the result of it's operation
                        *pos = Token(Token::tokenRealReal, "", tokenToValue(*(pos-1))*secondVal);

                        // Remove the tokens that were holding the first and second operand
                        pos = tokens.erase(pos-1);
                        tokens.erase(pos+1);

                        // It might be necessary to call this function again
                        return true;
                    }
                    // In case it's a division or modulo operator
                    if(pos->str == "%" || pos->str == "/")
                    {
                        // Division by zero or modulo by 0 is not allowed
                        if(secondVal == 0)
                            throw calcError(pos->str == "%" ? "Modulo by 0" : "Division by 0", calcError::invalidOperands, secondVal);

                        // Replace the current token by the result of it's operation
                        *pos = Token(Token::tokenRealReal, "", pos->str[0]=='/' ? tokenToValue(*(pos-1))/secondVal : std::fmod(tokenToValue(*(pos-1)), secondVal));

                        // Remove the tokens that were holding the first and second operand
                        pos = tokens.erase(pos-1);
                        tokens.erase(pos+1);

                        // It might be necessary to call this function again
                        return true;
                    }
                }
            }

            // There is no need to call this function again, since there are no multiply, division or modulo operators left
            return false;
        }

        bool calc::calcDoPlusmin()
        {
            // Loop through all tokens
            for(std::vector<Token>::iterator pos = tokens.begin(); pos != tokens.end(); ++pos)
            {
                // In case of a plus or minus operator
                if(pos->type == Token::tokenOperator && (pos->str == "+" || pos->str == "-"))
                {
                    // Replace the current token by the result of it's operation
                    *pos = Token(Token::tokenRealReal, "", pos->str[0] == '+' ? tokenToValue(*(pos-1)) + tokenToValue(*(pos+1)) : tokenToValue(*(pos-1)) - tokenToValue(*(pos+1)) );

                    // Remove the tokens that were holding the first and second operand
                    pos = tokens.erase(pos-1);
                    tokens.erase(pos+1);

                    // It might be necessary to call this function again
                    return true;
                }
            }

            // There is no need to call this function again, since there are no plus or minus operators left
            return false;
        }

        bool calc::calcDoCompare()
        {
            // Loop through all tokens
            for(std::vector<Token>::iterator pos = tokens.begin(); pos != tokens.end(); ++pos)
            {
                // In case of a comparison operator
                if(pos->type == Token::tokenOperator && (pos->str==">" || pos->str=="<"))
                {
                    // Replace the current token by the result of it's operation
                    *pos = Token(Token::tokenRealReal, "", pos->str[0]=='>' ? tokenToValue(*(pos-1)) > tokenToValue(*(pos+1)) : tokenToValue(*(pos-1)) < tokenToValue(*(pos+1)) );

                    // Remove the tokens that were holding the first and second operand
                    pos = tokens.erase(pos-1);
                    tokens.erase(pos+1);

                    // It might be necessary to call this function again
                    return true;
                }
            }

            // There is no need to call this function again, since there are no plus or minus operators left
            return false;
        }

        bool calc::calcDoBitwise()
        {
            // Loop through all tokens
            for(std::vector<Token>::iterator pos = tokens.begin(); pos != tokens.end(); ++pos)
            {
                // In case of a bitwise operator
                if(pos->type == Token::tokenOperator && (pos->str=="|" || pos->str=="&"))
                {
                    // Replace the current token by the result of it's operation
                    *pos = Token(Token::tokenRealReal, "",
                                 pos->str[0]=='|' ?
                                 static_cast<long int>(round(tokenToValue(*(pos-1)))) | static_cast<long int>(round(tokenToValue(*(pos+1)))) :
                                 static_cast<long int>(round(tokenToValue(*(pos-1)))) & static_cast<long int>(round(tokenToValue(*(pos+1)))) );

                    // Remove the tokens that were holding the first and second operand
                    pos = tokens.erase(pos-1);
                    tokens.erase(pos+1);

                    // It might be necessary to call this function again
                    return true;
                }
            }

            // There is no need to call this function again, since there are no plus or minus operators left
            return false;
        }
        bool calc::calcDoAssignments()
        {
            Token* lastToken = 0;                                               // Remembers the last token

            // Loop through all tokens
            for(std::vector<Token>::iterator pos = tokens.begin(); pos != tokens.end(); ++pos)
            {
                // If we aren't at the beginning of the vector and the current token is an assignment operator
                if(lastToken != 0 && pos->type == Token::tokenAssignmentOperator)
                {
                    // Set the value of the variable
                    setVar(lastToken->str, tokenToValue(*(pos+1)));

                    // Erase the assignment operator and the value behind it, letting the variable before the token be the result of this operation
                    tokens.erase(pos, pos+2);

                    // It might be necessary to call this function again
                    return true;
                }

                // Remember the last token
                lastToken = &(*pos);
            }

            // There is no need to call this function again, since there are no assignment operators left
            return false;
        }

// Functions:
    real str2real(const string& str, const bool& throwError)
    {
        // Determine the type of the real and parse it
        if(str.find("0x") == (str[0]=='-'))
            return calcPrivate::hexStr2real(str, throwError);
        if(str.find('0') == (str[0]=='-') && str[str.find('0')+1]!='.')
            return calcPrivate::octStr2real(str, throwError);
        return calcPrivate::decStr2real(str, throwError);
    }

    real timestr2real(const string& str, const bool& throwError)
    {
        // Initialise some variables needed for the parsing of the string
        real out = 0;                                                           // This will be returned at the of the function
        std::vector<real> splittedString;                                       // The string will be splitted in 1 to 3 parts containing the hours, minutes and seconds

        // Search for every ':' in the string, and convert the numbers between them into reals which are stored in splittedString
        size_t prevPos = (str[0]=='-' ? 1 : 0);                                 // The position where the last ':' was found
        size_t pos = string::npos;                                              // The current position where a ':' is found
        while((pos=str.find(':', prevPos)) != string::npos)
        {
            splittedString.push_back(calcPrivate::decStr2real(str.substr(prevPos, pos-prevPos), throwError));
            prevPos = pos+1;
        }
        // Make sure we don't miss the last real in the string
        if(prevPos < str.size())
            splittedString.push_back(calcPrivate::decStr2real(str.substr(prevPos), throwError));

        // There can't be more than 3 reals (i.e. hours, minutes and seconds), if there are more than 3 something's wrong
        if(splittedString.size() > 3)
        {
            if(throwError)
                throw calcError("Invalid time string", calcError::invalidExpression);
            splittedString.resize(3);
        }

        // Loop through the reals we extracted from the string and add there value in seconds to the output variable
        for(unsigned int i = 0; i < splittedString.size(); ++i)
            out += splittedString[i]*std::pow(60, 2-i);

        // If the string was preceded by an unary minus we make it negative
        if(str[0]=='-')
            out*=-1;

        // Return the result
        return out;
    }

    string real2str(const real& val, const realOutputType& outputType, const int& precision)
    {
        // Determine the desired output type and call the right function
        switch(outputType)
        {
            case outputType_time:
            return calcPrivate::real2timeStr(val);
            case outputType_bin:
            return calcPrivate::real2binStr(val);
            case outputType_oct:
            return calcPrivate::real2octStr(val);
            case outputType_hex:
            return calcPrivate::real2hexStr(val);
            default:
            return calcPrivate::real2decStr(val, outputType, precision < 0 ? std::numeric_limits<real>::digits10 : precision);
        }
    }
}
