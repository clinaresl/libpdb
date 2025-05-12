// -*- coding: utf-8 -*-
// helpers.h
// -----------------------------------------------------------------------------
//
// Started on <dom 27-02-2022 19:38:13.192951816 (1645987093)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>   Ian Herman <iankherman@gmail.com>   Ian Herman <iankherman@gmail.com>

//
// Common definitions for the implementation of different domains
//

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include<iostream>
#include<ranges>
#include<regex>
#include<string>
#include<vector>

#include "../src/pdb.h"

// Left trimming a string
static inline std::string& ltrim (std::string& s) {
    s.erase (s.begin(), std::find_if (s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));
    return s;
}

// Right trimming a string
static inline std::string& rtrim (std::string& s) {
    s.erase (std::find_if (s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), s.end());
    return s;
}

// Right and left trim a string
static inline std::string& trim (std::string& s) {
    return ltrim (rtrim (s));
}

// given a vector of any type show its contents on the standard output separated
// by a blank
template<typename T>
void print (const std::vector<T>& v) {
    for (const auto& item: v) {
        std::cout << item << " ";
    }
}

// Return the output of the command "git describe"
const std::string git_describe ();

// transform the input string to lower case and return it
const std::string tolower (std::string& input);

// Given a list of choices, update the first parameter to the one matching one
// in choices, and return true. If there is no match, return false
//
// A match happens when the choice and any of the choices are exactly the same.
// Two characters are the same even if they are shown in different case.
bool get_choice (std::string& choice, const std::vector<std::string>& choices);

// given a string with a blank separated list of ints, return a vector of ints
// with its contents
std::vector<int> string_to_int (const std::string& params);

// given a string with a blank separated list of strings, return a vector of
// strings with its contents
std::vector<std::string> string_to_string (const std::string& params);

// return true if the first string contains only characters given in the second
// string
bool in (const std::string& str, const std::string& chrs);

#endif // _HELPERS_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
