#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "utils.hpp"


// Throw error and terminate program
void throw_error(const std::string& message) {
    std::cerr << message << std::endl;
    std::exit(-1);
}

// Spit string with delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Split string with blank chars
std::vector<std::string> split_by_blank(const std::string& str) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_token = false;

    for (char ch : str) {
        if (std::isspace(ch)) {
            if (in_token) {
                tokens.push_back(token);
                token.clear();
                in_token = false;
            }
        } else {
            token += ch;
            in_token = true;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

// Remove l/r blank chars from string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

// replace tab into blanks in string
std::string replace_tab(const std::string& str) {
    std::string result = str;
    std::replace(result.begin(), result.end(), '\t', ' ');
    return result;
}

// convert string into lower case
std::string to_lowercase(const std::string& input) {
    std::string output = input;
    std::transform(output.begin(), output.end(), output.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return output;
}