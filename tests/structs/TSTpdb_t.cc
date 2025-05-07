// -*- coding: utf-8 -*-
// PDBpdb_t.h
// -----------------------------------------------------------------------------
//
// Started on <dom 04-05-2025 01:14:51.331654889 (1746314091)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Unit tests for testing pdbs
//

#include "../fixtures/TSTpdbfixture.h"

using namespace std;

// check that PDBs are properly created
// ----------------------------------------------------------------------------
TEST_F (PDBFixture, Empty) {

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // create an empty PDB (of pancakes) and verify its size and capacity
        // are correct. PDBs, when being created have to know the number of
        // items to host. An arbitrary value is given here.
        pdb::pdb_t<npancake_t> pdb (MAX_VALUES);

        ASSERT_EQ (pdb.capacity (), MAX_VALUES);
        ASSERT_EQ (pdb.size (), 0);
    }
}

// Check that ranking partial permutations, i.e., masked before being ranked, is
// correct
// ----------------------------------------------------------------------------
TEST_F (PDBFixture, Masking) {

    // Test all the full permutations of the N-Pancake with 4<= N <= 8
    for (auto length = 4 ; length <= 8 ; length++) {

        // compute all the full permutations with this length
        auto permutations = generatePermutations (length);

        // test all possible patterns
        for (auto nbsymbols = 0; nbsymbols <= length ; nbsymbols++) {

            // compute all patterns with length symbols, nbsymbols of them being
            // preserved
            auto patterns = generatePatterns (nbsymbols, length-nbsymbols);

            // test every pattern separately
            for (auto ipattern : patterns) {

                // create a PDB for processing n-Pancakes with n=length, and
                // initialize it with the identity permutation and the i-th pattern
                auto goal = succListInt (length);
                pdb::pdboff_t space_size = pdb::pdb_t<npancake_t>::address_space (ipattern);
                pdb::pdb_t<npancake_t> pdb (space_size);
                pdb.init (goal, ipattern);

                // now, verify that the masking operation is correct
                for (const auto iperm :  permutations) {

                    ASSERT_EQ (mask (iperm, goal, ipattern),
                               pdb.mask (iperm));
                }
            }
        }
    }
}

// Check that the ranking of full permutations is correct with full patterns
// ----------------------------------------------------------------------------
TEST_F (PDBFixture, FullPermutationsFullPattern) {

    // Test all the full permutations of the N-Pancake with 4<= N <= 10
    for (auto length = 4 ; length <= 10 ; length++) {

        auto permutations = generatePermutations (length);

        // Verify first that the size of the address space equals the number of
        // permutations generated, using a pattern which preserves all symbols
        string pattern = string (length, '-');
        pdb::pdboff_t space_size = pdb::pdb_t<npancake_t>::address_space (pattern);
        ASSERT_EQ (permutations.size (), space_size);

        // create a PDB for processing n-Pancakes with n=length, and initialize
        // it with the identity permutation
        pdb::pdb_t<npancake_t> pdb (space_size);
        pdb.init (succListInt (length), pattern);

        // create an instance of the 10-Pancake with each permutation and verify
        // that a unique index is generated in the range [0, n!)
        vector<int> ranked (space_size, 0);
        for (const auto& ipermutation: permutations) {

            // create an instance with this permutation
            npancake_t instance (ipermutation);

            // and verify now that its index has not been generated before
            auto index = pdb.rank (instance.get_perm ());
            ASSERT_FALSE (ranked [index]);

            // and now set it to true to check against the next values
            ranked[index] = 1;
        }
    }
}

// Check that the ranking of full permutations is correct with partial patterns
// ----------------------------------------------------------------------------
TEST_F (PDBFixture, FullPermutationsPartialPattern) {

    // Test all the full permutations of the N-Pancake with 4<= N <= 8
    for (auto length = 4 ; length <= 8 ; length++) {

        // compute all the full permutations with this length
        auto permutations = generatePermutations (length);

        // test also all patterns with a number of symbols being preserved from
        // 1 until length-1 (the case with length symbols being preserved is
        // already tested with FullPermutationsFullPattern)
        for (auto nbsymbols = 1; nbsymbols < length ; nbsymbols++) {

            // compute all patterns with length symbols, nbsymbols of them being
            // preserved
            auto patterns = generatePatterns (nbsymbols, length-nbsymbols);

            // test every pattern separately
            for (auto ipattern : patterns) {

                // create a PDB for processing n-Pancakes with n=length, and
                // initialize it with the identity permutation and the i-th pattern
                auto goal = succListInt (length);
                pdb::pdboff_t space_size = pdb::pdb_t<npancake_t>::address_space (ipattern);
                pdb::pdb_t<npancake_t> pdb (space_size);
                pdb.init (goal, ipattern);

                // now test every full permutation with this pattern. A map is
                // used to register the first full permutation which has a
                // specific value. Because patterns are used, it is expected for
                // other permutations (when being abstracted) to have the same
                // rank. The test consists then of verifying that both the new
                // permutation and the recorded one represent the same abstract
                // state
                map<int, vector<int>> mapping;
                for (const auto iperm :  permutations) {

                    // create an instance with this permutation and compute its
                    // ranking
                    npancake_t instance (iperm);
                    auto index = pdb.rank (instance.get_perm ());

                    // check whether a permutation with the same rank index has
                    // been generated before
                    if ( auto it = mapping.find (index); it == mapping.end ()) {

                        // if not, annotate this permutation in the map
                        mapping [index] = iperm;
                    } else {

                        // otherwise, verify that both permutations, the one
                        // recorded before, and this one represent the same
                        // abstract state
                        ASSERT_TRUE (equal_abstract (it->second, iperm, goal, ipattern));
                    }
                }
            }
        }
    }
}

// Check that the ranking of partial (masked) permutations is correct
// ----------------------------------------------------------------------------
TEST_F (PDBFixture, PartialPermutations) {

    // Test all the full permutations of the N-Pancake with 4<= N <= 8
    for (auto length = 4 ; length <= 8 ; length++) {

        // compute all the full permutations with this length
        auto permutations = generatePermutations (length);

        // test also all patterns with a number of symbols being preserved from
        // 1 until length-1 (the case with length symbols being preserved is
        // already tested with FullPermutationsFullPattern)
        for (auto nbsymbols = 1; nbsymbols < length ; nbsymbols++) {

            // compute all patterns with length symbols, nbsymbols of them being
            // preserved
            auto patterns = generatePatterns (nbsymbols, length-nbsymbols);

            // test every pattern separately
            for (auto ipattern : patterns) {

                // create a PDB for processing n-Pancakes with n=length, and
                // initialize it with the identity permutation and the i-th pattern
                auto goal = succListInt (length);
                pdb::pdboff_t space_size = pdb::pdb_t<npancake_t>::address_space (ipattern);
                pdb::pdb_t<npancake_t> pdb (space_size);
                pdb.init (goal, ipattern);

                // now test every full permutation with this pattern. A map is
                // used to register the first full permutation which has a
                // specific value. Because patterns are used, it is expected for
                // other permutations (when being abstracted) to have the same
                // rank. The test consists then of verifying that both the new
                // permutation and the recorded one represent the same abstract
                // state
                map<int, vector<int>> mapping;
                for (const auto iperm :  permutations) {

                    // create an instance with the masked permutation and rank
                    // it
                    npancake_t instance (pdb.mask (iperm));
                    auto index = pdb.rank (instance.get_perm ());

                    // check whether a permutation with the same rank index has
                    // been generated before
                    if ( auto it = mapping.find (index); it == mapping.end ()) {

                        // if not, annotate this permutation in the map
                        mapping [index] = iperm;
                    } else {

                        // otherwise, verify that both permutations, the one
                        // recorded before, and this one represent the same
                        // abstract state
                        ASSERT_TRUE (equal_abstract (it->second, iperm, goal, ipattern));
                    }
                }
            }
        }
    }
}

// check that instances of the N-Pancake can be inserted in PDBs
// ----------------------------------------------------------------------------
TEST_F (PDBFixture, InsertNPancake) {

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // create an empty PDB (of n-pancakes) with capacity enough for storing
        // nbpancakes of length NB_DISCS/2
        int length = NB_DISCS/2;
        auto pattern = string (length, '-');
        int capacity = pdb::pdb_t<npancake_t>::address_space (pattern);
        int nbpancakes = 1 + (rand ()%MAX_VALUES);
        pdb::pdb_t<npancake_t> pdb (capacity);

        // and initialize it to accept permutations of length NB_DISCS/2
        auto goal = succListInt (length);
        pdb.init (goal, pattern);

        // create a random number of different nodes of npancakes. Since PDBs
        // do not store duplicates make sure that all nodes are unique
        auto values = randNodes (nbpancakes, length);

        // Insert them into the PDB
        int idx = 0;
        for (auto& value : values) {
            auto ptr = pdb.insert (value);

            // verify the size is correct
            idx++;
            ASSERT_EQ (pdb.size (), idx);
        }
    }
}

// checks that lookups work correctly in PDBs of of n-pancakes
// ----------------------------------------------------------------------------
TEST_F (PDBFixture, LookupNPancake) {

    for (auto i = 0 ; i < NB_TESTS/10 ; i++) {

        // create an empty PDB (of n-pancakes) with capacity enough for storing
        // nbpancakes of length NB_DISCS/2
        int length = NB_DISCS/2;
        auto pattern = string (length, '-');
        int capacity = pdb::pdb_t<npancake_t>::address_space (pattern);
        int nbpancakes = 2 * (1 + (rand ()%MAX_VALUES));
        pdb::pdb_t<npancake_t> pdb (capacity);

        // and initialize it to accept permutations of length NB_DISCS/2
        auto goal = succListInt (length);
        pdb.init (goal, pattern);

        // create a random number of diffferent nodes of npancakes
        auto values = randNodes (nbpancakes, length);

        // Insert only half of the nodes in the PDB
        auto idx = 0;
        for (auto item : values) {
            pdb.insert (item);

            // and verify the size is correct
            idx++;
            ASSERT_EQ (pdb.size (), idx);

            // in case that half of the nodes have been already processed, exit
            if (idx >= values.size ()/2) {
                break;
            }
        }

        // Now, ensure that all those nodes that have been inserted are actually
        // found. Likewise, that those not being inserted are not found
        idx = 0;
        for (auto item : values) {

            // lookup for this value in the PDB
            auto lookup = pdb.find (item);

            // inserted values
            if (idx < values.size ()/2) {

                // verify the information returned is correct
                ASSERT_TRUE (lookup != string::npos);
                ASSERT_EQ (pdb[lookup], item.get_g ());
            } else {

                // unexisting values. Note that when an entry does not exist
                // there is no need to check the value of the iterator
                ASSERT_TRUE (lookup == string::npos);
            }

            // and increment the number of nodes processed so far
            idx++;
        }
    }
}


// Local Variables:
// mode:cpp
// fill-column:80
// End:
