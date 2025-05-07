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

#include<iomanip>

#include "../TSTdefs.h"
#include "../fixtures/TSTnpancakefixture.h"

using namespace std;

// Check that instances are correctly created with either the explicit
// constructor or the copy constructor
// ----------------------------------------------------------------------------
TEST_F (NPancakeFixture, DefaultInstance) {

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // Explicit construction

        // create a random instance with at least 10 discs and no more than
        // NB_DISCS + 10
        auto length = 10 + rand () % (NB_DISCS-10);
        npancake_t instance1 = npancake_t (randVectorInt (length, length, true));

        // and verify the size is the chosen one
        ASSERT_EQ (length, npancake_t::get_n ());

        // Copy constructor
        npancake_t instance2 = randInstance (length);

        // and verify again the size is the chosen one
        ASSERT_EQ (length, npancake_t::get_n ());
    }
}

// Check that all successors are correctly generated in the unit variant with
// real states
// ----------------------------------------------------------------------------
TEST_F (NPancakeFixture, SuccessorsUnitReal) {

    // perform expansions under the unit variant
    npancake_t::init (npancake_variant::unit);

    for (auto i = 0 ; i < NB_TESTS ; i++ ) {

        // first, generate a random instance
        auto length = 10 + rand () % (NB_DISCS-10);
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
// with real states
// ----------------------------------------------------------------------------
TEST_F (NPancakeFixture, SuccessorsHeavyCostReal) {

    // perform expansions under the heavy-cost variant
    npancake_t::init (npancake_variant::heavy_cost);

    for (auto i = 0 ; i < NB_TESTS ; i++ ) {

        // first, generate a random instance
        auto length = 10 + rand () % (NB_DISCS-10);
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

// Check that all successors are correctly generated in the unit variant with
// abstract states
// ----------------------------------------------------------------------------
TEST_F (NPancakeFixture, SuccessorsUnitAbstract) {

    // perform expansions under the unit variant
    npancake_t::init (npancake_variant::unit);

    for (auto i = 0 ; i < NB_TESTS ; i++ ) {

        // randomly choose the size of the next pancake and always use the
        // identity permutation as the goal in the forthcoming tests. To avoid
        // creating very large abstract state spaces, the size of the pancakes
        // is restricted to [4, 10] and the number of symbols being preserved is
        // aproximately half the length of the pancake
        auto length = 4 + rand () % (7);
        auto goal = succListInt (length);

        // randomly try up to 10 different patterns
        auto patterns = randPatterns (10, length);
        for (auto ipattern : patterns) {

            // create an abstract state. For using the masking services provided
            // by the PDBs it is necessary to initialize it
            auto address_space = pdb::pdb_t<npancake_t>::address_space (ipattern);
            pdb::pdb_t<npancake_t> pdb (address_space);
            pdb.init (goal, ipattern);

            // and create an abstract state of the n-pancake
            npancake_t state = randInstance (length);
            npancake_t instance {pdb.mask (state.get_perm ())};

            // now, expand this node and generate all children
            vector<tuple<uint8_t, npancake_t>> successors;
            instance.children (successors);

            // first, verify the number of descendants equals its length minus one
            ASSERT_EQ (successors.size (), npancake_t::get_n ()-1);
            ASSERT_EQ (successors.size (), length-1);

            // verify that all children have been correctly generated, ie., they
            // are correct abstract states and the operator cost is always equal
            // to 1
            for (auto op = 1 ; i < length ; i++) {

                // get the op-th child and its cost
                auto [g, child] = successors[op-1];

                // verify that all symbols in the range [0, op] are reversed
                for (auto rev = 0 ; rev <= op/2; rev++) {
                    ASSERT_EQ (instance[rev], child[op-rev]);
                }

                // and that all symbols in the range [op+1, length) are equal
                for (auto idx = op+1; idx < length ; idx++) {
                  ASSERT_EQ (instance[idx], child[idx]);
                }

                // verify also the cost is always equal to one
                ASSERT_EQ (g, 1);
            }
        }
    }
}



// Local Variables:
// mode:cpp
// fill-column:80
// End:
