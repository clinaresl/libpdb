// -*- coding: utf-8 -*-
// TSThelpers.h
// -----------------------------------------------------------------------------
//
// Started on <lun 07-08-2023 16:06:48.172249590 (1691417208)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>

//
// Helper functions for all tests
//
#ifndef _TSTHELPERS_H_
#define _TSTHELPERS_H_

#include<algorithm>
#include<random>
#include<set>
#include<string>
#include<vector>

#include "../src/structs/PDBnode_t.h"
#include "../domains/n-pancake/npancake_t.h"

// definition of a struct used to generate a list of numbers of the desired
// length. succ () returns the next integer index, starting from 1
struct index_t {
    int _index;
    index_t () : _index {0}{}
    int operator () () {return ++_index;}
};

// Return the binomial coefficient of n choose k
size_t binomial_coefficient (int n, int k);

// Generate a random string with length characters in the sequence ASCII(32) -
// ASCII(126) which do not appear in the string exclude
std::string randString (int length, std::string exclude="");

// Generate a random string with a number of characters randomly selected in the
// range [l0, l1) in the sequence ASCII(32) - ASCII(126) which do not appear in
// the string exclude
std::string randString (int l0, int l1, std::string exclude="");

// return a vector with a list of numbers from 0 up to n-1
std::vector<int> succListInt (int n);

// return a vector with n numbers randomly generated in the interval [0, m). If
// remove_duplicates takes the value true, then no values are duplicated
std::vector<int> randVectorInt (int n, int m, bool remove_duplicates=false);

// return a vector with n strings randomly generated each with m chars. If
// remove_duplicates takes the value true, then no values are duplicated
std::vector<std::string> randVectorString (int n, int m, bool remove_duplicates=false);

// create a random instance of the N-Pancake with the given length
const npancake_t randInstance (int length);

#endif // _TSTHELPERS_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
