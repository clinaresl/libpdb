// -*- coding: utf-8 -*-
// TSTinpdbfixture.h
// -----------------------------------------------------------------------------
//
// Started on <dom 11-05-2025 17:03:12.229401689 (1746975792)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Fixture for testing input PDBs
//

#ifndef _TSTINPDBFIXTURE_H_
#define _TSTINPDBFIXTURE_H_

#include<filesystem>

#include "gtest/gtest.h"

#include "../TSTdefs.h"
#include "../TSThelpers.h"
#include "../../src/algorithm/PDBinpdb.h"
#include "../../src/algorithm/PDBoutpdb.h"
#include "../../domains/n-pancake/npancake_t.h"

// Class definition
//
// Defines a Google test fixture for testing inPDBs
class InPDBFixture : public ::testing::Test {

protected:

    void SetUp () override {

        // just initialize the random seed to make sure that every iteration is
        // performed over different random data
        srand (time (nullptr));
    }

    // return a string with a random path which does not necessarily exist
    std::filesystem::path generate_random_path(const std::string& prefix = "file_") {
        static const char chars[] =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789";

        static thread_local std::mt19937 rng(
            static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<> char_dist(0, sizeof(chars) - 2); // exclude null terminator

        std::ostringstream filename;
        for (int i = 0; i < 12; ++i) {
            filename << chars[char_dist(rng)];
        }

        // Optional: Add pseudo-random subdirectory
        std::ostringstream path;
        path << "/tmp/" << prefix << filename.str();

        std::filesystem::path pathdir = "/tmp";
        return pathdir /  filename.str ();
    }

};

#endif // _TSTINPDBFIXTURE_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
