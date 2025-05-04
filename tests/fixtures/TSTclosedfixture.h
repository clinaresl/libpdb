// -*- coding: utf-8 -*-
// TSTclosedfixture.h
// -----------------------------------------------------------------------------
//
// Started on <dom 04-05-2025 01:08:18.302163524 (1746313698)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Fixture used to test closed lists
//

#ifndef _TSTCLOSEDFIXTURE_H_
#define _TSTCLOSEDFIXTURE_H_

#include<algorithm>
#include<cstdlib>
#include<ctime>
#include<random>
#include<vector>

#include "gtest/gtest.h"

#include "../TSTdefs.h"
#include "../TSThelpers.h"
#include "../../src/structs/PDBclosed_t.h"
#include "../../domains/n-pancake/npancake_t.h"

// Class definition
//
// Defines a Google test fixture for testing closed lists
class ClosedFixture : public ::testing::Test {

protected:

    void SetUp () override {

        // just initialize the random seed to make sure that every iteration is
        // performed over different random data
        srand (time (nullptr));
    }

    // Generate a vector with n random instances of nodes of the N-pancake with
    // up to length discs each
    std::vector<pdb::node_t<npancake_t>> randNodes (const int n, const int length) {

        std::set<npancake_t> prev;
        std::vector<pdb::node_t<npancake_t>> instances;

        // now, create as many random instances as requested of the given length
        for (auto i = 0 ; i < n ; i++){

            // create a random instance of a pancake of the given length
            auto iperm = randInstance (length);

            // and ensure it is unique
            while (prev.find (iperm) != prev.end ()) {
                iperm = randInstance (length);
            }

            // and add it to the vector of instances to return
            pdb::node_t<npancake_t> ipancake {iperm};
            instances.push_back (ipancake);
        }

        return instances;
    }
};

#endif // _TSTCLOSEDFIXTURE_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
