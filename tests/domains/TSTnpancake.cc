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

// Check that instances are correctly recognized
TEST_F (NPancakeFixture, DefaultInstance) {

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // create a random instance with at least 10 discs and no more than
        // NB_DISCS + 10
        auto length = 10 + rand () % (NB_DISCS-10);
        npancake_t instance = randInstance (length);

        // and verify the size is the chosen one
        ASSERT_EQ (length, npancake_t::get_n ());
    }
}

// Check that all successors are correctly generated in the unit variant
TEST_F (NPancakeFixture, SuccessorsUnit) {

    for (auto i = 0 ; i < NB_TESTS ; i++ ) {

        // first, generate a random instance
        npancake_t instance = randInstance (10 + rand () % (NB_DISCS-10));

        // initialize the unit variant
        npancake_t::init (npancake_variant::unit);

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
TEST_F (NPancakeFixture, SuccessorsHeavyCost) {

    for (auto i = 0 ; i < NB_TESTS ; i++ ) {

        // first, generate a random instance
        npancake_t instance = randInstance (10 + rand () % (NB_DISCS-10));

        // initialize the heavy-cost variant
        npancake_t::init (npancake_variant::heavy_cost);

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


// Local Variables:
// mode:cpp
// fill-column:80
// End:
