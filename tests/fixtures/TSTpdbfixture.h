// -*- coding: utf-8 -*-
// TSTpdbfixture.h
// -----------------------------------------------------------------------------
//
// Started on <dom 04-05-2025 01:08:18.302163524 (1746313698)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Fixture used to test PDBs
//

#ifndef _TSTPDBFIXTURE_H_
#define _TSTPDBFIXTURE_H_

#include<algorithm>
#include<cstdint>
#include<cstdlib>
#include<ctime>
#include<random>
#include<vector>

#include "gtest/gtest.h"

#include "../TSTdefs.h"
#include "../TSThelpers.h"
#include "../../src/structs/PDBpdb_t.h"
#include "../../domains/n-pancake/npancake_t.h"

// Class definition
//
// Defines a Google test fixture for testing PDBs
class PDBFixture : public ::testing::Test {

protected:

    void SetUp () override {

        // just initialize the random seed to make sure that every iteration is
        // performed over different random data
        srand (time (nullptr));
    }

    // Generate a vector with n random different instances of nodes of the
    // N-pancake with up to length discs, each with a different strictly
    // positive g-value
    std::vector<pdb::node_t<npancake_t>> randNodes (const int n, const int length) {

        std::set<npancake_t> prev;
        std::vector<pdb::node_t<npancake_t>> instances;

        // now, create as many random instances as requested of the given length
        for (auto i = 0 ; i < n ; i++){

            // create a random instance of a pancake of the given length
            auto iperm = randInstance (length);

            // and ensure it is unique
            while (prev.find (iperm) != prev.end ()) {
                iperm = randInstance (length);
            }

            // remember it has been generated
            prev.insert (iperm);

            // and add it to the vector of instances to return
            pdb::node_t<npancake_t> ipancake {iperm, uint8_t (1 + (rand ()%MAX_VALUES))};
            instances.push_back (ipancake);
        }

        return instances;
    }

    // Generate a vector with all different instances of nodes of the N-pancake
    // with up to length discs, each with a different strictly positive g-value,
    // being masked with the given combination of goal and pattern
    std::vector<pdb::node_t<npancake_t>> randMaskedNodes (const int length,
                                                          const std::vector<int>& goal, const std::string pattern) {

        std::set<npancake_t> prev;
        std::vector<pdb::node_t<npancake_t>> instances;

        // first, create all permutations of the given length
        std::vector<std::vector<int>> perms = generatePermutations (length);

        // and process them all
        for (const auto& iperm: perms) {

            // mask this permutation with the given combination of goal and
            // pattern
            npancake_t instance (mask (iperm, goal, pattern));

            // if it already exists, then skip it
            if (prev.find (instance) != prev.end ()) {
                continue;
            }

            // remember this partial permutation has been generated
            prev.insert (instance);

            // and add it to the vector of instances to return
            pdb::node_t<npancake_t> ipancake {instance, uint8_t (1 + (rand ()%MAX_VALUES))};
            instances.push_back (ipancake);
        }

        return instances;
    }

    // return whether the two given (full) permutations are equal or not when
    // being abstracted according to the given pattern defined over the
    // specified goal state. Both permutations are assumed to have the same
    // length and also the same symbols but posibly arranged in a different
    // order. Also the pattern and goal have the same length than the
    // permutations
    bool equal_abstract (const std::vector<int> p1, const std::vector<int> p2,
                         const std::vector<int> goal, const std::string pattern) {

        // compute the inverse of each permutation
        auto maxi = std::max_element (p1.begin (), p1.end ());
        std::vector<int> q1 (1 + *maxi);
        for (auto i = 0 ; i < p1.size () ; i++) {
            q1[p1[i]] = i;
        }
        std::vector<int> q2 (1 + *maxi);
        for (auto i = 0 ; i < p2.size () ; i++) {
            q2[p2[i]] = i;
        }

        // now, verify whether both permutations are indeed equal or not
        for (auto i = 0 ; i < pattern.size () ; i++) {

            if (pattern[i]=='-') {

                // in case the i-th symbol in the goal is being preserved
                if (q1[goal[i]] != q2[goal[i]]) {

                    // then verify they contain the i-th symbol in goal in the
                    // same location
                    return false;
                }
            }
        }

        // at this point, they both have been proved to be the same abstract
        // state
        return true;
    }

    // given a pattern defined wrt to a goal state, return the partial
    // permutation that results after applying the pattern to it
    std::vector<int> mask (const std::vector<int>& perm,
                           const std::vector<int>& goal, const std::string pattern) {

        // Intentionally, this operation is performed in a way which is
        // different than the algorithm implemented in pdb_t
        std::vector<int> result;

        // compute the inverse of the goal --which most likely will be itself,
        // because the identity is commonly used
        auto maxi = std::max_element (goal.begin (), goal.end ());
        std::vector<int> q = std::vector<int>(1 + *maxi);
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
};

#endif // _TSTPDBFIXTURE_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
