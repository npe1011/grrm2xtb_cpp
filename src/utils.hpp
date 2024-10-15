#pragma once

#include <string>
#include <vector>

// Throw error and terminate program
void throw_error(const std::string&);

// Spit string with delimiter
std::vector<std::string> split(const std::string&, char);

// Split string with blank chars
std::vector<std::string> split_by_blank(const std::string&);

// replace tab into blanks in string
std::string replace_tab(const std::string&);

// Remove l/r blank chars from string
std::string trim(const std::string&);

// convert string into lower case
std::string to_lowercase(const std::string&);