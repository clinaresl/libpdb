// -*- coding: utf-8 -*-
// TSTpdb.cc
// -----------------------------------------------------------------------------
//
// Started on <vie 09-05-2025 15:46:20.334168744 (1746798380)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Unit tests for testing the creation of PDBs
//

#include "../fixtures/TSTpdbsfixture.h"

using namespace std;

// check that PDBs are properly created
// ----------------------------------------------------------------------------
TEST_F (PDBSFixture, Empty) {

    // Use pancakes of length 8
    int length = 8;
    auto goal = succListInt (length);

    // test all possible patterns with at least 1 symbol and up to 7 symbols
    // being preserved
    for (auto nbsymbols = 1; nbsymbols <= 7 ; nbsymbols++) {

        // compute all patterns with length symbols, nbsymbols of them being
        // preserved
        auto patterns = generatePatterns (nbsymbols, length-nbsymbols);

        // test every pattern separately
        for (auto ipattern : patterns) {

            // in the n-pancake both the ppattern and the cpattern are equal
            pdb::pdb<pdb::node_t<npancake_t>> pdb (goal, ipattern, ipattern);

            // and verify that the goal, and both patterns are correctly stored
            ASSERT_EQ (goal, pdb.get_goal ());
            ASSERT_EQ (ipattern, pdb.get_cpattern ());
            ASSERT_EQ (ipattern, pdb.get_ppattern ());

            // and also that the size is null
            ASSERT_EQ (pdb.size (), 0);
        }
    }
}

// check that PDBs are correctly generated in the N-Pancake domain
// ----------------------------------------------------------------------------
TEST_F (PDBSFixture, NPancakeGeneration) {

    // Use pancakes of length between 4 and 8
    for (auto length = 4 ; length <= 4 ; length++) {

        // create a goal with length symbols explicitly given
        auto goal = succListInt (length);

        // test all possible patterns with at least 1 symbol and up to length-1
        // symbols being preserved
        for (auto nbsymbols = 1 ; nbsymbols <= length-1 ; nbsymbols++) {

            // compute all patterns with length symbols, nbsymbols of them being
            // preserved
            auto patterns = generatePatterns (nbsymbols, length-nbsymbols);

            // test every pattern separately
            for (auto ipattern : patterns) {

                // in the n-pancake both the ppattern and the cpattern are equal
                pdb::pdb<pdb::node_t<npancake_t>> pdb (goal, ipattern, ipattern);

                // and generate the pdb
                pdb.generate ();

                // and verify that the PDB has been correctly generated
                ASSERT_TRUE (pdb.doctor ());

                // finally, check that the size of the PDB is equal to the size
                // of the abstract state space being traversed
                ASSERT_EQ (pdb.size (), pdb::pdb_t<pdb::node_t<npancake_t>>::address_space (ipattern));
            }
        }
    }
}

// Local Variables:
// mode:cpp
// fill-column:80
// End:
