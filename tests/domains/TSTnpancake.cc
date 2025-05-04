// -*- coding: utf-8 -*-
// TSTnpancake.cc
// -----------------------------------------------------------------------------
//
// Started on <dom 27-02-2022 23:42:57.992602429 (1646001777)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>

//
// Unit tests of the N-Pancake
//

#include<tuple>
#include<vector>

#include "../TSTdefs.h"
#include "../fixtures/TSTnpancakefixture.h"

using namespace std;

// Check that instances are correctly created with either the explicit
// constructor or the copy constructor
// ----------------------------------------------------------------------------
TEST_F (NPancakeFixture, DefaultInstance) {

    // Generate instances of the 40-Pancake. Since the panncake def contains a
    // static vector whose size depends on the number of items, it is not
    // possible to have several instances of Pancakes of different size
    auto length = 40;
    npancake_t::init (string (length, '-'));

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // Explicit construction

        // create a random instance with at least 10 discs and no more than
        // NB_DISCS + 10
        npancake_t instance1 = npancake_t (randVectorInt (length, length, true));

        // and verify the size is the chosen one
        ASSERT_EQ (length, npancake_t::get_n ());

        // Copy constructor
        npancake_t instance2 = randInstance (length);

        // and verify again the size is the chosen one
        ASSERT_EQ (length, npancake_t::get_n ());
    }
}

// Check that all successors are correctly generated in the unit variant
// ----------------------------------------------------------------------------
TEST_F (NPancakeFixture, SuccessorsUnit) {

    // Generate instances of the 40-Pancake. Since the panncake def contains a
    // static vector whose size depends on the number of items, it is not
    // possible to have several instances of Pancakes of different size
    auto length = 40;
    npancake_t::init (string (length, '-'), npancake_variant::unit);

    for (auto i = 0 ; i < NB_TESTS ; i++ ) {

        // first, generate a random instance
        npancake_t instance = randInstance (length);

        // now, expand this node and generate all children
        vector<tuple<uint8_t, npancake_t>> successors;
        instance.children (successors);

        // first, verify the number of descendants equals its length minus one
        ASSERT_EQ (successors.size (), npancake_t::get_n ()-1);

        // compute the number of discs flipped in every descendant
        vector<int> flips;
        transform (successors.begin (), successors.end (),
                   back_inserter (flips),
                   [&] (tuple<uint8_t, npancake_t> successor) {
                       return getPrefix (instance, get<1> (successor));
                   });

        // and next verify that flips are all numbers in the range [2, n) where
        // n is the size of the instance expanded
        for (auto i = 2 ; i <= npancake_t::get_n () ; i++) {
            ASSERT_EQ (i, flips[i-2]);
        }

        // To conclude verify that the cost of every operator is the same and
        // always equal to 1
        for (auto const& isuccessor: successors) {
            ASSERT_EQ (get<0>(isuccessor), 1);
        }
    }
}

// Check that all successors are correctly generated in the heavy-cost variant
// ----------------------------------------------------------------------------
TEST_F (NPancakeFixture, SuccessorsHeavyCost) {

    // Generate instances of the 40-Pancake. Since the panncake def contains a
    // static vector whose size depends on the number of items, it is not
    // possible to have several instances of Pancakes of different size
    auto length = 40;
    npancake_t::init (string (length, '-'), npancake_variant::heavy_cost);

    for (auto i = 0 ; i < NB_TESTS ; i++ ) {

        // first, generate a random instance
        npancake_t instance = randInstance (length);

        // now, expand this node and generate all children
        vector<tuple<uint8_t, npancake_t>> successors;
        instance.children (successors);

        // first, verify the number of descendants equals its length minus one
        ASSERT_EQ (successors.size (), npancake_t::get_n ()-1);

        // compute the number of discs flipped in every descendant
        vector<int> flips;
        transform (successors.begin (), successors.end (),
                   back_inserter (flips),
                   [&] (tuple<uint8_t, npancake_t> successor) {
                       return getPrefix (instance, get<1> (successor));
                   });

        // and next verify that flips are all numbers in the range [2, n) where
        // n is the size of the instance expanded
        for (auto i = 2 ; i <= npancake_t::get_n () ; i++) {
            ASSERT_EQ (i, flips[i-2]);
        }

        // To conclude verify that the cost of every operator is equal to the
        // radius of the first disc below the spatula, i.e., the radius of the
        // first disc not being transposed.
        for (auto i = 0 ; i < successors.size () ; i++) {

            // in case the whole permutation is transposed, then the cost is
            // equal to the length of the permutation plus one
            if (i == successors.size ()-1) {
                ASSERT_EQ (get<0>(successors[i]), 1+npancake_t::get_n ());
            } else {

                // otherwise, the cost of this operator is the radius of the
                // first disc immediately below the spatula. Because i=0 stands
                // for the first operator which flips the first two dics, i+2 is
                // used
                ASSERT_EQ (get<0>(successors[i]), instance[2+i]);
            }
        }
    }
}

// Check that the ranking of full permutations is correct
// ----------------------------------------------------------------------------
TEST_F (NPancakeFixture, FullPermutations) {

    // Test all the full permutations of the N-Pancake with 4<= N <= 10
    for (auto length = 4 ; length <= 10 ; length++) {

        auto permutations = generatePermutations (length);

        // initialize the unit variant using a pattern that represents the full
        // goal state
        npancake_t::init (string (length, '-'), npancake_variant::unit);

        // Verify first that the size of the address space equals the number of
        // permutations generated
        ASSERT_EQ (permutations.size (), npancake_t::address_space ());

        // create an instance of the 10-Pancake with each permutation and verify
        // that a unique index is generated in the range [0, n!)
        vector<int> ranked (npancake_t::address_space (), 0);
        for (const auto& ipermutation: permutations) {

            // create an instance with this permutation
            npancake_t instance (ipermutation);

            // and verify now that its index has not been generated before
            auto index = instance.rank_pdb ();
            ASSERT_FALSE (ranked [index]);

            // and now set it to true to check against the next values
            ranked[index] = 1;
        }
    }
}

// Local Variables:
// mode:cpp
// fill-column:80
// End:
