// -*- coding: utf-8 -*-
// TSTinpdb.cc
// -----------------------------------------------------------------------------
//
// Started on <dom 11-05-2025 17:04:12.096094180 (1746975852)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Unit tests for testing inPDBs
//

#include "../fixtures/TSTinpdbfixture.h"

using namespace std;

// check that inPDBs are properly created
// ----------------------------------------------------------------------------
TEST_F (InPDBFixture, Empty) {

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // randomly choose the name of a file in /tmp (that does not necessarily
        // exist)
        auto path = generate_random_path ();

        // create an input PDB with this hypothetical filename
        pdb::inpdb<pdb::node_t<npancake_t>> pdb (path);

        // and verify it takes the default values
        ASSERT_EQ (pdb.get_pdb_mode (), pdb::pdb_mode::max);
        ASSERT_EQ (pdb.get_goal ().size (), 0);
        ASSERT_EQ (pdb.get_cpattern ().size (), 0);
        ASSERT_EQ (pdb.get_ppattern ().size (), 0);
        ASSERT_EQ (pdb.get_error (), pdb::error_message::no_error);
        ASSERT_EQ (pdb.get_in_error (), pdb::in_error_message::no_error);
    }
}

// check that MAX inPDBs have been correctly retrieved
TEST_F (InPDBFixture, NPancakeMaxRead) {

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

            // generate a MAX PDB with this combination of goal and pattern
            pdb::outpdb<pdb::node_t<npancake_t>> outpdb (pdb::pdb_mode::max, goal, ipattern, ipattern);
            outpdb.generate ();

            // the PDB has been correctly generated
            if (!outpdb.doctor ()) {
                cout << " Doctor: " << outpdb.get_error_message () << endl; cout.flush ();
                cout << "         Address space: " << outpdb.size () << endl; cout.flush ();
                cout << "         # expansions : " << outpdb.get_nbexpansions () << endl; cout.flush ();
                cout << "         ipattern     : " << ipattern << endl; cout.flush ();
                ASSERT_TRUE (false);
            }

            // and write it to a unique filepath
            std::filesystem::path temp_file;
            auto temp_dir = std::filesystem::temp_directory_path();
            do {
                auto now = std::chrono::system_clock::now().time_since_epoch().count();
                temp_file = temp_dir / (std::to_string(now) + "_" + ipattern + ".max");
            } while (std::filesystem::exists(temp_file));

            // and ensure it was properly written
            ASSERT_TRUE (outpdb.write (temp_file));

            // Now, read the PDB from the file. Check that the value returned is
            // not zero, if so, write the error message and fail
            pdb::inpdb<pdb::node_t<npancake_t>> inpdb (temp_file);
            auto ret = inpdb.read ();
            if (!ret) {
                cout << inpdb.get_in_error_message () << endl; cout.flush ();
                ASSERT_TRUE (false);
            }

            // and check it is a MAX PDB
            ASSERT_EQ (inpdb.get_pdb_mode (), pdb::pdb_mode::max);

            // verify that the goal, ppattern and cpattern are the same used
            // during the generation
            ASSERT_EQ (inpdb.get_goal (), goal);
            ASSERT_EQ (inpdb.get_cpattern (), ipattern);
            ASSERT_EQ (inpdb.get_ppattern (), ipattern);

            // compare the outPDB with the inPDB
            //
            // First, compare they both have the same capacity. The size can not
            // be used because inPDBs do not use insert to write data
            ASSERT_GT (outpdb.capacity (), 0);
            ASSERT_GT (inpdb.capacity (), 0);
            ASSERT_EQ (outpdb.capacity (), inpdb.capacity ());

            // Second, compare the values stored in each PDB are strictly the same
            for (auto idx = 0 ; idx < outpdb.size () ; idx++) {
                ASSERT_EQ (outpdb[idx], inpdb[idx]);
            }

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
