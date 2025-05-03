// -*- coding: utf-8 -*-
// TSTopenfixture.h
// -----------------------------------------------------------------------------
//
// Started on <sáb 03-05-2025 21:59:33.215937839 (1746302373)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Unit tests for testing open_t
//

#ifndef _TSTOPENFIXTURE_H_
#define _TSTOPENFIXTURE_H_

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "../TSTdefs.h"
#include "../TSThelpers.h"
#include "../../src/structs/PDBopen_t.h"

// Class definition
//
// Defines a Google test fixture for testing open lists
class OpenFixture : public ::testing::Test {

    protected:

        void SetUp () override {

            // just initialize the random seed to make sure that every iteration
            // is performed over different random data
            srand (time (nullptr));
        }

        // compute the index of a 5-Pancake as its contents read as an int
        int index (npancake_t pancake) const {
            return pancake[0]*10'000 +
                pancake[1]*1'000 +
                pancake[2]*100 +
                pancake[3]*10 +
                pancake[4];
        }

        // return a vector with nbitems random instances of the 5-Pancake
        std::vector<npancake_t> randItems (const int nbitems) {

            std::vector<npancake_t> values;

            // add the random instances
            for (auto j = 0 ; j < nbitems ; j++) {
                values.push_back (randInstance (5));
            }

            return values;
        }

        // Populate an open list with a random sequence of instances of the
        // 5-Pancake in increasing order ---in this experiment, a total order is
        // created by reading the contents of a N-Pancake as an integer value
        std::vector<npancake_t> populate (pdb::open_t<npancake_t>& open,
                                          const int nbitems) {

            // Create the random sequence
            std::vector<npancake_t> values = randItems (nbitems);

            // insert all the 5-Pancakes in the open list sorting them in
            // increasing order of their index which is given by their contents
            // read as an int.
            for (auto v : values) {
                open.insert (v, index (v));
            }

            // and return the vector of values inserted in the open list
            return values;
        }
};

#endif // _TSTOPENFIXTURE_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
