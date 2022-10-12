#include "StringUtils.h"

#define TRIM_CHARS " \t\n\r\f\v"

// trim from end of string (right)
std::string& rtrim(std::string& s)
{
    s.erase(s.find_last_not_of(TRIM_CHARS) + 1);
    return s;
}

// trim from beginning of string (left)
std::string& ltrim(std::string& s)
{
    s.erase(0, s.find_first_not_of(TRIM_CHARS));
    return s;
}

// trim from both ends of string (right then left)
std::string& trim(std::string& s)
{
    return ltrim(rtrim(s));
}


std::vector<std::string> tokenise (const std::string &s, char sep )
{
    std::vector<std::string> tokens;

    std::string token;

    int start, end = 0;

    do {

        start = s.find_first_not_of(sep, end);

        end = s.find_first_of (sep, start);

        if (start == (int)s.length() || start == end) 
            break;

        if (end >= 0) 
            token = s.substr(start, end - start);
        else 
            token = s.substr(start, s.length() - start);

        tokens.push_back(token);

        start = end + 1;

    }
    while (end > 0);

    return tokens; 
}

