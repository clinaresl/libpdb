// -*- coding: utf-8 -*-
// helpers.cc
// -----------------------------------------------------------------------------
//
// Started on <dom 27-02-2022 19:43:18.800790041 (1645987398)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>   Ian Herman <iankherman@gmail.com>

//
// Common definitions for the implementation of different domains
//

#include "helpers.h"

using namespace std;

// Return the output of the command "git describe"
const string git_describe () {
    std::string cmd = "git describe --tags";
    std::string result = "";
    FILE* pipe = popen (cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error ("popen() failed!");
    try {
        while (feof (pipe) == 0) {
            char buffer[128];
            if (fgets (buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose (pipe);
        throw;
    }
    pclose (pipe);

    // remove the newline character
    result.erase(remove(result.begin(), result.end(), '\n'), result.cend());
    return result;
}

// transform the input string to lower case and return it
const string tolower (string& input) {

    for_each(input.begin(), input.end(), [](char & c){
        c = ::tolower(c);
    });
    return input;
}

// Given a list of choices, update the first parameter to the one matching one
// in choices, and return true. If there is no match, return false
//
// A match happens when the choice and any of the choices are exactly the same.
// Two characters are the same even if they are shown in different case.
bool get_choice (string& choice, const vector<string>& choices) {

    // for all choices
    for (auto ichoice : choices) {

        // if these two strings are the same in spite of the case, then return
        // the choice matching the variant
        if (tolower (choice) == tolower (ichoice)) {
            choice = ichoice;
            return true;
        }
    }

    // at this point, no match was found
    return false;
}

// given a string in a blank separated list of ints, return a vector of ints
// with its contents
std::vector<int> string_to_int (const std::string& params) {

    std::vector<int> result;

    regex regex("\\s+");
    sregex_token_iterator it(params.begin(), params.end(), regex, -1);
    sregex_token_iterator end;

    // and now process each integer separately
    for (; it != end; ++it) {

        try {

            // get its value and add it to the vector of integers
            auto value = stoi (*it);
            result.push_back (value);
        } catch (const char *msg) {
            throw invalid_argument (" error while processing a content: " + string (msg));
        }
    }

    // return the vector with all digits found in the input string
    return result;
}

// return true if the first string contains only characters given in the second
// string
bool in (const std::string& str, const std::string& chrs) {

    return ranges::all_of(str, [chrs](char c) {
        return chrs.find (c) != string::npos;
    });
}

// Local Variables:
// mode:cpp
// fill-column:80
// End:
