// -*- coding: utf-8 -*-
// gtest.cc
// -----------------------------------------------------------------------------
//
// Started on <lun 07-08-2023 16:04:30.433788026 (1691417070)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>

//
// Main entry point of Google Tests
//

#include "gtest/gtest.h"

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // and run the selection of tests
    return RUN_ALL_TESTS();
}

// Local Variables:
// mode:cpp
// fill-column:80
// End:
