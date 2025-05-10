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
};

#endif // _TSTOUTPDBFIXTURE_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
