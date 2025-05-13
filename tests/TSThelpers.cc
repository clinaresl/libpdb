// -*- coding: utf-8 -*-
// TSThelpers.cc
// -----------------------------------------------------------------------------
//
// Started on <lun 07-08-2023 16:09:23.879483705 (1691417363)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>

//
// Helper functions for all tests
//

#include "TSThelpers.h"

using namespace std;

// Return the binomial coefficient of n choose k
size_t binomial_coefficient (int n, int k) {

    // create a bidimensional matrix nxn
    vector<vector<size_t>> bc (n+1, vector<size_t> (n+1));

    // now, fill in the base cases
    for (auto i = 0 ; i < n+1 ; bc[i++][0] = 1);
    for (auto i = 0 ; i < n+1 ; bc[i][i] = 1, i++);

    // fill in the table
    for (auto i = 2 ; i < n+1 ; i++) {
        for (auto j = 1 ; j < i ; j++) {
            bc[i][j] = bc[i-1][j-1] + bc[i-1][j];
        }
    }

    // and return the binomial coefficient
    return bc[n][k];
}

// Generate a random string with length characters in the sequence ASCII(32) -
// ASCII(126) which do not appear in the string exclude
string randString (int length, string exclude) {

  // just randomly sample a character from the set until the string gets the
  // requested length
  char randchr;
  string randstr = "";
  for (int i = 0 ; i < length ; i++) {
    do {
      randchr = (char) (rand () % (126 - 32 + 1) + 32);
    } while (exclude.find (randchr) != string::npos);
    randstr += randchr;
  }

  return randstr;
}

// Generate a random string with a number of characters randomly selected in the
// range [l0, l1) in the sequence ASCII(32) - ASCII(126) which do not appear in
// the string exclude
string randString (int l0, int l1, string exclude)
{
    // randomly determine the length of the string
    int length = l0 + random () % (l1 - l0);

    // and return a string randomly generated with this length
    return randString (length, exclude);
}

// return a vector with a list of numbers from 1 up to n
std::vector<int> succListInt (int n) {

    // First create a vector of integers of the desired size
    vector<int> output = vector<int>(n, 0);

    // and next overwrite its contents using the succ function defined over
    // index_t
    generate (output.begin (), output.end (), index_t ());
    return output;
}

// return a vector with n numbers randomly generated in the interval [0, m). If
// remove_duplicates takes the value true, then no values are duplicated
vector<int> randVectorInt (int n, int m, bool remove_duplicates) {

    // create an empty vector
    set<int> lookup;
    vector<int> result;

    // generate n random numbers
    for (auto i = 0 ; i < n ; i++) {

        // generate a new random number avoiding duplicates if requested
        int item = rand () % m;
        while (remove_duplicates &&
               lookup.find (item) != lookup.end ()) {
            item = rand () % m;
        }

        // and insert it into the vector, and remember it for future insertions
        result.push_back (item);
        lookup.insert (item);
    }

    // and return the vector
    return result;
}

// return a vector with n strings randomly generated each with m chars. If
// remove_duplicates takes the value true, then no values are duplicated
vector<string> randVectorString (int n, int m, bool remove_duplicates) {

    // create an empty vector
    set<string> lookup;
    vector<string> result;

    // generate n random strings
    for (auto i = 0 ; i < n ; i++) {

        // generate a new random string avoiding duplicates if requested
        string item = randString (m);
        while (remove_duplicates &&
               lookup.find (item) != lookup.end ()) {
            item = randString (n);
        }

        // and insert it into the vector
        result.push_back (item);
    }

    // and return the vector
    return result;
}

// create a random instance of the N-Pancake with the given length
const npancake_t randInstance (int length) {

    // first, create the identity permutation of the given length
    vector<int> goal = succListInt (length);

    // and just shuffle the goal
    random_device rd;
    mt19937 generator (rd());
    shuffle (goal.begin (), goal.end (), generator);

    // and return the random instance
    npancake_t result {goal};
    return result;
}

// return a vector of vectors of integers with all permutations of the symbols
// in the range [1, N]
std::vector<std::vector<int>> generatePermutations(int n) {
    vector<vector<int>> result;
    vector<int> nums;

    // Initialize nums with [1, 2, ..., n]
    for (int i = 1; i <= n; ++i) {
        nums.push_back(i);
    }

    // Generate all permutations using std::next_permutation
    do {
        result.push_back(nums);
    } while (next_permutation(nums.begin(), nums.end()));

    return result;
}

// return a vector of vectors of strings with all patterns that contain n
// symbols '-' and m symbols '*'
vector<string> generatePatterns(int n, int m) {
    set<string> prev;
    vector<string> anc, result;
    string seed;

    // Initialize a string with the letters from 'a' up to the character a+(n+m)
    for (auto i = 0 ; i < n+m ; i++) {
        seed.push_back (char ('a'+i));
    }

    // Generate all permutations using the symbols in the range ['a', 'a'+n+m]
    do {
        anc.push_back(seed);
    } while (next_permutation(seed.begin(), seed.end()));

    // and now process all permutations making the proper substitutions
    for (const auto& iperm : anc) {

        // substitute in this permutation every symbol in the range ['a', 'a'+n]
        // with '-' and the rest with '*'
        string next;
        for (auto& ch: iperm) {
            if (ch -'a' < n) {
                next.push_back ('-');
            } else {
                next.push_back ('*');
            }
        }

        // in case this string has not been produced add it then
        if (prev.find (next) == prev.end ()) {
            result.push_back (next);
        }

        // and add it to the set of strings being produced
        prev.insert (next);
    }

    return result;
}

// given a pattern defined wrt to a goal state, return the partial permutation
// that results after applying the pattern to it
vector<int> mask (const vector<int>& perm,
                  const vector<int>& goal, const string pattern) {

    // Intentionally, this operation is performed in a way which is
    // different than the algorithm implemented in pdb_t
    vector<int> result;

    // compute the inverse of the goal --which most likely will be itself,
    // because the identity is commonly used
    auto maxi = max_element (goal.begin (), goal.end ());
    vector<int> q = vector<int>(1 + *maxi);
    for (auto i = 0 ; i < int (goal.size ()) ; i++) {
        q[goal[i]] = i;
    }

    // and now process the permutation applying the given pattern
    for (auto i = 0 ; i < int (perm.size ()) ; i++) {

        // in case this symbol is being preserved in the goal state
        if (pattern[q[perm[i]]] == '-') {
            result.push_back (perm[i]);
        } else {
            result.push_back (pdb::NONPAT);
        }
    }

    return result;
}


// Local Variables:
// mode:cpp
// fill-column:80
// End:
