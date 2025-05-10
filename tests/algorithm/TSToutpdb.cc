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

#include "../fixtures/TSToutpdbfixture.h"

using namespace std;

// check that outPDBs are properly created
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, Empty) {

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

            // in the n-pancake both the ppattern and the cpattern are equal.
            // For the purpose of this test, any pdb mode can be used
            pdb::outpdb<pdb::node_t<npancake_t>> pdb (pdb::pdb_mode::max, goal, ipattern, ipattern);

            // and verify that the goal, and both patterns are correctly stored
            ASSERT_EQ (goal, pdb.get_goal ());
            ASSERT_EQ (ipattern, pdb.get_cpattern ());
            ASSERT_EQ (ipattern, pdb.get_ppattern ());

            // and also that the size is null
            ASSERT_EQ (pdb.size (), 0);
        }
    }
}

// check that MAX PDBs are correctly generated in the N-Pancake domain
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeMaxGeneration) {

    // Use pancakes of length between 4 and 8
    for (auto length = 8 ; length <= 8 ; length++) {

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
                pdb::outpdb<pdb::node_t<npancake_t>> pdb (pdb::pdb_mode::max, goal, ipattern, ipattern);

                // and generate the pdb
                pdb.generate ();

                // and verify that the PDB has been correctly generated
                if (!pdb.doctor ()) {
                    cout << " Doctor: " << pdb.get_error_message () << endl; cout.flush ();
                    cout << "         Address space: " << pdb.size () << endl; cout.flush ();
                    cout << "         # expansions : " << pdb.get_nbexpansions () << endl; cout.flush ();
                    cout << "         ipattern     : " << ipattern << endl; cout.flush ();
                    ASSERT_TRUE (false);
                }

                // finally, check that the size of the PDB is equal to the size
                // of the abstract state space being traversed
                ASSERT_EQ (pdb.size (), pdb::pdb_t<pdb::node_t<npancake_t>>::address_space (ipattern));
            }
        }
    }
}

// check that MAX PDBs are correctly generated in the N-Pancake domain
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeMaxWrite) {

    // Use pancakes of length between 8
    int length = 8;

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
            pdb::outpdb<pdb::node_t<npancake_t>> pdb (pdb::pdb_mode::max, goal, ipattern, ipattern);

            // and generate the pdb
            pdb.generate ();

            // and verify that the PDB has been correctly generated
            if (!pdb.doctor ()) {
                cout << " Doctor: " << pdb.get_error_message () << endl; cout.flush ();
                cout << "         Address space: " << pdb.size () << endl; cout.flush ();
                cout << "         # expansions : " << pdb.get_nbexpansions () << endl; cout.flush ();
                cout << "         ipattern     : " << ipattern << endl; cout.flush ();
                ASSERT_TRUE (false);
            }

            // finally, check that the size of the PDB is equal to the size
            // of the abstract state space being traversed
            ASSERT_EQ (pdb.size (), pdb::pdb_t<pdb::node_t<npancake_t>>::address_space (ipattern));

            // and check whether it was possible to create the PDB
            //
            // first, determine a unique path filename
            std::filesystem::path temp_file;
            auto temp_dir = std::filesystem::temp_directory_path();
            do {
                auto now = std::chrono::system_clock::now().time_since_epoch().count();
                temp_file = temp_dir / (std::to_string(now) + "_" + ipattern + ".max");
            } while (std::filesystem::exists(temp_file));

            // second, check the operation was feasible
            ASSERT_TRUE (pdb.write (temp_file));

            // finally, remove the file
            error_code ec;
            ASSERT_TRUE (std::filesystem::remove(temp_file, ec));
        }
    }
}



// Local Variables:
// mode:cpp
// fill-column:80
// End:
