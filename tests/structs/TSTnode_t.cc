// -*- coding: utf-8 -*-
// TSTnode_t.cc
// -----------------------------------------------------------------------------
//
// Started on <lun 07-08-2023 16:35:56.825833552 (1691418956)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>

//
// Unit tests for the node_t class
//

#include<string>
#include<vector>

#include "../fixtures/TSTnodefixture.h"

using namespace std;

// Checks the creation of n-pancake nodes with default values
// ----------------------------------------------------------------------------
TEST_F (NodeFixture, DefaultValuesInt) {

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // create a node which holds a 5-Pancake
        npancake_t pancake = randInstance (5);
        pdb::node_t<npancake_t> node (pancake);

        // next, verify that all attributes of this node take the default values
        ASSERT_EQ (node.get_g (), 0);
    }
}


// Local Variables:
// mode:cpp
// fill-column:80
// End:
