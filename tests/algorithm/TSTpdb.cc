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
        }
    }
}

// check that PDBs are correctly generated in the N-Pancake domain
// ----------------------------------------------------------------------------
TEST_F (PDBSFixture, NPancakeGeneration) {

    // Use pancakes of length 4
    int length = 8;
    auto goal = succListInt (length);

    // test all possible patterns with at least 1 symbol and up to 3 symbols
    // being preserved
    for (auto nbsymbols = 1 ; nbsymbols <= 3 ; nbsymbols++) {

        // compute all patterns with length symbols, nbsymbols of them being
        // preserved
        auto patterns = generatePatterns (nbsymbols, length-nbsymbols);

        // test every pattern separately
        for (auto ipattern : patterns) {

            // in the n-pancake both the ppattern and the cpattern are equal
            pdb::pdb<pdb::node_t<npancake_t>> pdb (goal, ipattern, ipattern);

            cout << " goal: ";
            for (auto idisc : goal) {
                cout << idisc << " ";
            }
            cout << endl; cout.flush ();
            cout << " pattern: " << ipattern << endl << endl; cout.flush ();

            // and generate the pdb
            pdb.generate ();

            // and verify that the number of expansions equal the size of the
            // abstract state space
            ASSERT_EQ (pdb.get_nbexpansions (), pdb::pdb_t<pdb::node_t<npancake_t>>::address_space (ipattern));
            cout << " # expansions: " << pdb.get_nbexpansions () << endl; cout.flush ();
            cout << " elapsed time: " << pdb.get_elapsed_time ().count () << " seconds" << endl; cout.flush ();
        }
    }
}

// Local Variables:
// mode:cpp
// fill-column:80
// End:
