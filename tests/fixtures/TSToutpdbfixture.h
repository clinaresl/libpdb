// -*- coding: utf-8 -*-
// TSTpdbfixture.h
// -----------------------------------------------------------------------------
//
// Started on <dom 04-05-2025 01:08:18.302163524 (1746313698)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Fixture used to test the creation of PDBs
//

#ifndef _TSTOUTPDBFIXTURE_H_
#define _TSTOUTPDBFIXTURE_H_

#include<filesystem>
#include<stdexcept>

#include "gtest/gtest.h"

#include "../TSTdefs.h"
#include "../TSThelpers.h"
#include "../../src/algorithm/PDBoutpdb.h"
#include "../../domains/n-pancake/npancake_t.h"

// Class definition
//
// Defines a Google test fixture for testing outPDBs
class OutPDBFixture : public ::testing::Test {

protected:

    void SetUp () override {

        // just initialize the random seed to make sure that every iteration is
        // performed over different random data
        srand (time (nullptr));
    }

    // given a pattern defined with respect to a goal, get the default cost
    // defined as the minimum value among the symbols being abstracted.
    // Obviously, if all symbols are abstracted, then the minimum integer
    // defined in the goal is returned as the default cost
    pdb::pdbval_t get_default_cost_npancake (const std::vector<int>& goal, const std::string pattern) {

        // Even if by default the goal is defined as the identity permutation,
        // compute the default cost without making this assumption. It is
        // assumed however, that both the goal and the pattern are not empty
        pdb::pdbval_t cost = std::numeric_limits<pdb::pdbval_t>::max();
        for (auto i = 0 ; i < int (goal.size ()) ; i++) {

            // Update the default cost if this symbol is abstracted and its
            // value is less than the incumbent value
            cost = (pattern[i] == '*' && goal[i] < cost) ? goal[i] : cost;
        }

        return cost;
    }
};

#endif // _TSTOUTPDBFIXTURE_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
