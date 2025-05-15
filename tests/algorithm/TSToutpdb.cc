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

// check that MAX PDBs are correctly generated in the unit variant of the
// N-Pancake domain
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeUnitMaxGeneration) {

    // Set the unit variant with a default cost equal to one
    npancake_t::init (npancake_variant::unit, 1);

    // Use pancakes of length 8
    auto length = 8;

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

// Verify that data is *seemingly* well computed in the unit variant of the
// N-Pancake domain, i.e., that a PDB preserving all symbols produces g-values
// which are either larger or equal than other PDBs which abstract a positive
// number of symbols.
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeUnitDominance) {

    // Set the unit variant with a default cost equal to one
    npancake_t::init (npancake_variant::unit, 1);

    // Use pancakes of length 8
    auto length = 8;

    // create a goal with length symbols explicitly given
    auto goal = succListInt (length);

    // first generate a PDB which preserves all symbols
    string full_pattern (length, '-');
    pdb::outpdb<pdb::node_t<npancake_t>> full_pdb (pdb::pdb_mode::max, goal, full_pattern, full_pattern);
    full_pdb.generate ();

    // and verify that the PDB has been correctly generated
    if (!full_pdb.doctor ()) {
        cout << " Doctor: " << full_pdb.get_error_message () << endl; cout.flush ();
        cout << "         Address space: " << full_pdb.size () << endl; cout.flush ();
        cout << "         # expansions : " << full_pdb.get_nbexpansions () << endl; cout.flush ();
        cout << "         ipattern     : " << full_pattern << endl; cout.flush ();
        ASSERT_TRUE (false);
    }

    // Next generate all the different PDBs that result from abstracting a
    // strictly positive number of symbols. 'nbsymbols' is the number of symbols
    // being preserved
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

            // in passing, check that the size of the PDB is equal to the size
            // of the abstract state space being traversed
            ASSERT_EQ (pdb.size (), pdb::pdb_t<pdb::node_t<npancake_t>>::address_space (ipattern));

            // next, try all permutations with the given length
            vector<vector<int>> perms = generatePermutations (length);
            for (const auto& iperm: perms) {

                // mask this permutation according to the i-th pattern
                vector<int> mperm = mask (iperm, goal, ipattern);

                // and now verify that the value returned in the full PDB is
                // larger or equal than the value computed in the more
                // restricted PDB
                ASSERT_GE (full_pdb[iperm], pdb[mperm]);
            }
        }
    }
}

// check that MAX PDBs generated for the unit variant of the N-Pancake can be
// randomly accesed and updated
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeUnitMaxRandAccess) {

    // Set the unit variant with a default cost equal to one
    npancake_t::init (npancake_variant::unit, 1);

    // Use pancakes of length 8
    auto length = 8;

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

            // selectively choose a number of locations of this PDB
            for (auto idx = 0 ; idx < MAX_VALUES ; idx++) {

                // randomly choose a location and read its value
                auto loc = rand () % pdb.size ();
                pdb::pdbval_t val = pdb[loc];

                // update its value with a random value (which might be the
                // same than the previous one or not)
                pdb::pdbval_t newval = pdb::pdbval_t (rand ()%std::numeric_limits<pdb::pdbval_t>::max());
                pdb[loc] = newval;

                // and verify the new value has been correctly set
                ASSERT_EQ (newval, pdb[loc]);
            }
        }
    }
}

// check that MAX PDBs are correctly generated and saved in the unit variant of
// the N-Pancake domain
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeUnitMaxWrite) {

    // Set the unit variant with a default cost equal to one
    npancake_t::init (npancake_variant::unit, 1);

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

// check that MAX PDBs are correctly generated in the heavy-cost variant of the
// N-Pancake domain
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeHeavyCostMaxGeneration) {

    // Use pancakes of length 8
    auto length = 8;

    // Set the heavy-cost variant with a default cost randomly chosen in the
    // interval [1, length]
    npancake_t::init (npancake_variant::heavy_cost, 1 + rand () % length);

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

// Verify that data is *seemingly* well computed in the heavy-cost variant of
// the N-Pancake domain, i.e., that a PDB preserving all symbols produces
// g-values which are either larger or equal than other PDBs which abstract a
// positive number of symbols.
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeHeavyCostDominance) {

    // Use pancakes of length 8
    auto length = 8;

    // Set the heavy-cost variant with a default cost equal to its default
    // value, 1 since no symbols are abstracted in the full PDB
    npancake_t::init (npancake_variant::heavy_cost);

    // create a goal with length symbols explicitly given
    auto goal = succListInt (length);

    // first generate a PDB which preserves all symbols
    string full_pattern (length, '-');
    pdb::outpdb<pdb::node_t<npancake_t>> full_pdb (pdb::pdb_mode::max, goal, full_pattern, full_pattern);
    full_pdb.generate ();

    // and verify that the PDB has been correctly generated
    if (!full_pdb.doctor ()) {
        cout << " Doctor: " << full_pdb.get_error_message () << endl; cout.flush ();
        cout << "         Address space: " << full_pdb.size () << endl; cout.flush ();
        cout << "         # expansions : " << full_pdb.get_nbexpansions () << endl; cout.flush ();
        cout << "         ipattern     : " << full_pattern << endl; cout.flush ();
        ASSERT_TRUE (false);
    }

    // Next generate all the different PDBs that result from abstracting a
    // strictly positive number of symbols. 'nbsymbols' is the number of symbols
    // being preserved
    for (auto nbsymbols = 1 ; nbsymbols <= length-1 ; nbsymbols++) {

        // compute all patterns with length symbols, nbsymbols of them being
        // preserved
        auto patterns = generatePatterns (nbsymbols, length-nbsymbols);

        // test every pattern separately
        for (auto ipattern : patterns) {

            // set the default cost corresponding to this pattern. Care has to
            // be taken here, otherwise, the PDB would not be generated
            // correctly
            npancake_t::init (npancake_variant::heavy_cost, get_default_cost_npancake (goal, ipattern));

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

            // in passing, check that the size of the PDB is equal to the size
            // of the abstract state space being traversed
            ASSERT_EQ (pdb.size (), pdb::pdb_t<pdb::node_t<npancake_t>>::address_space (ipattern));

            // next, try all permutations with the given length
            vector<vector<int>> perms = generatePermutations (length);
            for (const auto& iperm: perms) {

                // mask this permutation according to the i-th pattern
                vector<int> mperm = mask (iperm, goal, ipattern);

                // and now verify that the value returned in the full PDB is
                // larger or equal than the value computed in the more
                // restricted PDB
                ASSERT_GE (full_pdb[iperm], pdb[mperm]);
            }
        }
    }
}

// check that MAX PDBs generated for the heavy-cost variant of the N-Pancake can
// be randomly accesed and updated
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeHeavyCostMaxRandAccess) {

    // Use pancakes of length 8
    auto length = 8;

    // Set the heavy-cost variant with a default cost equal to its default
    // value, 1
    npancake_t::init (npancake_variant::heavy_cost);

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

            // set the default cost corresponding to this pattern
            npancake_t::init (npancake_variant::heavy_cost, get_default_cost_npancake (goal, ipattern));

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

            // selectively choose a number of locations of this PDB
            for (auto idx = 0 ; idx < MAX_VALUES ; idx++) {

                // randomly choose a location and read its value
                auto loc = rand () % pdb.size ();
                pdb::pdbval_t val = pdb[loc];

                // update its value with a random value (which might be the
                // same than the previous one or not)
                pdb::pdbval_t newval = pdb::pdbval_t (rand ()%std::numeric_limits<pdb::pdbval_t>::max());
                pdb[loc] = newval;

                // and verify the new value has been correctly set
                ASSERT_EQ (newval, pdb[loc]);
            }
        }
    }
}

// check that MAX PDBs are correctly generated and saved in the heavy-cost
// variant of the N-Pancake domain
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeHeavyCostMaxWrite) {

    // Use pancakes of length 8
    auto length = 8;

    // Set the heavy-cost variant with a default cost equal to its default
    // value, 1
    npancake_t::init (npancake_variant::heavy_cost);

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

            // set the default cost corresponding to this pattern
            npancake_t::init (npancake_variant::heavy_cost, get_default_cost_npancake (goal, ipattern));

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

// check that the generation of MAX PDBs correctly detects the case out of
// bounds returning an exception in the heavy-cost variant of the N-Pancake.
// Note it is not possible to run this test for the unit variant because the
// default cost is not used in that case
// ----------------------------------------------------------------------------
TEST_F (OutPDBFixture, NPancakeHeavyCostGenerationOutOfRange) {

    // Set the unit variant with a default cost equal to 150, enough to exceed
    // the range of pdbval_t
    npancake_t::init (npancake_variant::heavy_cost, 150);

    // Use pancakes of length 8
    auto length = 8;

    // create a goal with length symbols explicitly given
    auto goal = succListInt (length);

    // consider simply the case that preserves the first symbol in the goal
    // state
    string ipattern = "-" + string (length-1, '*');

    // in the n-pancake both the ppattern and the cpattern are equal
    pdb::outpdb<pdb::node_t<npancake_t>> pdb (pdb::pdb_mode::max, goal, ipattern, ipattern);

    // and generate the pdb. Check that a runtime_error is generated
    EXPECT_THROW (pdb.generate (), runtime_error);
 }


// Local Variables:
// mode:cpp
// fill-column:80
// End:
