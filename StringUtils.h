#pragma once

#include <vector>
#include <string>

// trim from end of string (right)
std::string& rtrim(std::string& s);

// trim from beginning of string (left)
std::string& ltrim(std::string& s);

// trim from both ends of string (right then left)
std::string& trim(std::string& s);

// split a string by tokens
std::vector<std::string> tokenise (const std::string &s, char sep );

