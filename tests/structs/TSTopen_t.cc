// -*- coding: utf-8 -*-
// TSTopen_t.cc
// -----------------------------------------------------------------------------
//
// Started on <sáb 03-05-2025 22:01:32.367695123 (1746302492)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Unit tests for testing open lists
//

#include "../fixtures/TSTopenfixture.h"

using namespace std;

// Checks the creation of empty open lists is initialized correctly
// ----------------------------------------------------------------------------
TEST_F (OpenFixture, NPancakeEmpty) {

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // create an open list to store npancakes
        pdb::open_t<pdb::node_t<npancake_t>> bucket;

        // now, verify the bucket is properly initialized
        ASSERT_EQ (bucket.get_mini (), 1);
        ASSERT_EQ (bucket.get_maxi (), 1);
        ASSERT_EQ (bucket.get_nbbuckets (), 1);
        ASSERT_EQ (bucket.size (), 0);
    }
}

// Checks the number of buckets grows as the indices become larger
// ----------------------------------------------------------------------------
TEST_F (OpenFixture, NPancakeSize) {

    // resizing a bucket is a very costly operation and thus, this is done only
    // once

    // create a bucket to store strings
    pdb::open_t<pdb::node_t<npancake_t>> open;

    // now insert items at the positions which follow the serie of powers of
    // 2 until a very large number
    int idx = 1;
    while (idx <= MAX_NB_BUCKETS ) {

        // insert a node with a random instance of the 5-Pancake at this index
        npancake_t instance = randInstance (5);
        open.insert (pdb::node_t (instance, idx));

        // in this test, items are inserted in increasing order, thus, verify
        // that the maximum index progresses accordingly, whereas the minimum
        // index is always the same
        ASSERT_EQ (open.get_mini (), 1);
        ASSERT_EQ (open.get_maxi (), idx);

        // verify now that the number of buckets has doubled
        ASSERT_GE (open.get_nbbuckets (), 1+idx);
        idx *= 2;
    }
}

// Checks that open lists can be effectively resized
// ----------------------------------------------------------------------------
TEST_F (OpenFixture, NPancakeResize) {

    for (auto i = 0 ; i < NB_TESTS ; i++) {

        // create an open list to store instances of the 5-Pancake
        pdb::open_t<pdb::node_t<npancake_t>> open;

        // randomly determine the number of buckets. The maximum length is
        // roughly 1,000, the reason being that creating buckets which are
        // larger is a very costly operation
        auto length = 1 + random () % MAX_NB_BUCKETS/1'000;

        // and compute the minimum power of two which is greater or equal than
        // the random number of buckets
        auto pow2 = 1;
        while (pow2 < length) {
            pow2 *= 2;
        };

        // resize the bucket and verify the value returned is correct
        ASSERT_EQ (open.set_nbbuckets (length), pow2);

        // also verify the operation was correctly performed
        ASSERT_EQ (open.get_nbbuckets (), pow2);

        // in spite of the bucket being resized, no insertions were done, and
        // thus ...
        ASSERT_EQ (open.get_mini (), 1);
        ASSERT_EQ (open.get_maxi (), 1);
        ASSERT_EQ (open.size (), 0);
    }
}

// Checks that instances of the 5-Pancake are properly sorted in an open list in
// increasing order
// ----------------------------------------------------------------------------
TEST_F (OpenFixture, NPancakeInsert) {

    // function used for computing the minimum and maximum index
    auto comp = [this] (const pdb::node_t<npancake_t>& s1, const pdb::node_t<npancake_t>& s2) {
        return index (s1.get_state ()) < index (s2.get_state ());
    };

    for (auto i = 0 ; i < NB_TESTS/10 ; i++) {

        // create an open list for storing nodes with instances of the 5-Pancake
        pdb::open_t<pdb::node_t<npancake_t>> open;

        // populate the open list
        vector<pdb::node_t<npancake_t>> values = populate (open, MAX_VALUES);

        // get the minimum and maximum length which should be proven to be equal
        // to the minimum and maximum indexes in the bucket
        auto mini = min_element (values.begin (), values.end (), comp);
        auto maxi = max_element (values.begin (), values.end (), comp);
        ASSERT_EQ (index ((*mini).get_state ()), open.get_mini ());
        ASSERT_EQ (index ((*maxi).get_state ()), open.get_maxi ());

        // extract all nodes from the bucket and verify their index is given in
        // increasing order
        int idx = 0;
        int current = 0;
        while (open.size ()) {

            // extract the first element from the bucket
            pdb::node_t<npancake_t> item = open.pop_front ();
            ASSERT_GE (index (item.get_state ()), current);

            // and verify the size has decreased accordingly
            idx++;
            ASSERT_EQ (open.size (), values.size () - idx);

            // and update the length of the last string retrieved from the
            // bucket
            current = index (item.get_state ());
        }
    }
}

// Checks that nodes of 5-Pancake are properly removed from an open list
// ----------------------------------------------------------------------------
TEST_F (OpenFixture, NPancakeRemove) {

    // function used for computing the minimum and maximum index
    auto comp = [this] (const pdb::node_t<npancake_t>& s1, const pdb::node_t<npancake_t>& s2) {
        return index (s1.get_state ()) < index (s2.get_state ());
    };

    for (auto i = 0 ; i < NB_TESTS/10 ; i++) {

        // create an open list to store npancakes
        pdb::open_t<pdb::node_t<npancake_t>> open;

        // populate the open list
        vector<pdb::node_t<npancake_t>> values = populate (open, MAX_VALUES);

        // create a histogram with the observations of the indices of all
        // values. Indices range in the interval [0, 256)
        vector<int> histogram (256);
        for (auto v : values ) {
            histogram[index (v.get_state ())]++;
        }

        // verify the histogram corresponds with the data stored in the bucket
        for (auto j = 0 ; j < 256 ; j++) {
            ASSERT_EQ (open.size (j), histogram[j]);
        }

        // next, randomly remove some items from the bucket. For this, randomly
        // choose half the values generated and remove items from the open list
        const auto length = values.size ();
        for (auto j = 0 ; j < length ; j++) {
            auto loc = random () % values.size ();
            auto idx = index (values[loc].get_state ());

            // remove an item from the bucket with an index equal to the loc-th
            // item in values, and verify that the extracted element has
            // precisely that size
            auto item = open.remove (idx);
            ASSERT_EQ (index (item.get_state ()), idx);

            // verify the histogram corresponds with the data stored in the bucket
            histogram[idx]--;
            for (auto j = 0 ; j < 256 ; j++) {
                ASSERT_EQ (open.size (j), histogram[j]);
            }

            // make sure to remove this item from the vector of values to avoid
            // selecting it again
            values.erase (values.begin () + loc);

            // verify now that the total number of items has been properly
            // updated
            ASSERT_EQ (open.size (), values.size ());

            // get the minimum and maximum index which should be proven to be
            // equal to the minimum and maximum indexes in the open list unless
            // the open list is exhausted in which case min and max are equal to
            // 1
            auto mini = min_element (values.begin (), values.end (), comp);
            auto maxi = max_element (values.begin (), values.end (), comp);
            ASSERT_TRUE (open.size () == 0 ||
                         (index ((*mini).get_state ()) == open.get_mini ()));
            ASSERT_TRUE (open.size () == 0 ||
                         (index ((*maxi).get_state ()) == open.get_maxi ()));
        }

        // before running into the next try, verify the total number of items is
        // zero. This is redundant, I know! Pure paranoia ...!!
        ASSERT_EQ (open.size (), 0);
    }
}

// Checks that mixed insertions and removals from an open list of nodes with
// instances of the 5-Pancake work as expected
// ----------------------------------------------------------------------------
TEST_F (OpenFixture, NPancakeInsertRemove) {

    // function used for computing the minimum and maximum index
    auto comp = [this] (const pdb::node_t<npancake_t>& s1, const pdb::node_t<npancake_t>& s2) {
        return index (s1.get_state ()) < index (s2.get_state ());
    };

    for (auto i = 0 ; i < NB_TESTS/10 ; i++) {

        // create a bucket to store instances of the 5-Pancake
        pdb::open_t<pdb::node_t<npancake_t>> open;

        // populate the bucket and get the values inserted
        vector<pdb::node_t<npancake_t>> values = populate (open, MAX_VALUES);

        // create a histogram with the observations of the indices of all
        // values. Indices range in the interval [0, 256)
        vector<int> histogram (256);
        for (auto v : values ) {
            histogram[index (v.get_state ())]++;
        }

        // verify the histogram corresponds with the data stored in the bucket
        for (auto j = 0 ; j < 256 ; j++) {
            ASSERT_EQ (open.size (j), histogram[j]);
        }

        // now, create a vector of 5-Pancakes to insert into the open list
        vector<pdb::node_t<npancake_t>> data = randItems (MAX_VALUES);

        // and now, until the additional data is exhausted or the bucket is
        // exhausted
        while (data.size () && open.size ()) {

            // through a dice to determine whether to insert an additional item
            // or to remove an existing item from the open list
            int op = rand () % 2;
            if (!op) {

                // removing from the bucket a random element
                auto loc = random () % values.size ();
                auto idx = index (values[loc].get_state ());

                // and remove it. Verify next that the item removed has the
                // expected index
                auto item = open.remove (idx);
                ASSERT_EQ (index (item.get_state ()), idx);

                // make sure to remove this item from the vector of values to
                // avoid selecting it again
                values.erase (values.begin () + loc);

                // verify now that the total number of items has been properly
                // updated
                ASSERT_EQ (open.size (), values.size ());
            } else {

                // insert an item from the vector of additional data
                auto loc = random () % data.size ();
                open.insert (data[loc]);

                // ensure that values contains the same data stored in the bucket
                values.push_back (data[loc]);

                // make sure to remove this item from the vector of additional
                // data to avoid selecting it again
                data.erase (data.begin () + loc);

                // verify now that the total number of items has been properly
                // updated
                ASSERT_EQ (open.size (), values.size ());
            }

            // get the minimum and maximum index which should be proven to be
            // equal to the minimum and maximum indexes in the open list unless
            // the open list is exhausted in which case min and max are equal to
            // 1
            auto mini = min_element (values.begin (), values.end (), comp);
            auto maxi = max_element (values.begin (), values.end (), comp);
            ASSERT_TRUE (open.size () == 0 ||
                         (index ((*mini).get_state ()) == open.get_mini ()));
            ASSERT_TRUE (open.size () == 0 ||
                         (index ((*maxi).get_state ()) == open.get_maxi ()));
        }

        // extract all items from the bucket and verify their length is given in
        // increasing order
        int idx = 0;
        int current = 0;
        while (open.size ()) {

            // extract the first element from the bucket
            pdb::node_t<npancake_t> item = open.pop_front ();
            ASSERT_GE (index (item.get_state ()), current);

            // and verify the size has decreased accordingly
            idx++;
            ASSERT_EQ (open.size (), values.size () - idx);

            // and update the length of the last string retrieved from the
            // bucket
            current = index (item.get_state ());
        }
    }
}

// Local Variables:
// mode:cpp
// fill-column:80
// End:
