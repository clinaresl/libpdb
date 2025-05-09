// -*- coding: utf-8 -*-
// PDBopen_t.h
// -----------------------------------------------------------------------------
//
// Started on <sáb 03-05-2025 21:25:58.441876637 (1746300358)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Definition of an open list for storing nodes
//

#ifndef _PDBOPEN_T_H_
#define _PDBOPEN_T_H_

#include<algorithm>
#include<iostream>
#include<stddef.h>
#include<stdexcept>
#include<vector>

#include "../PDBdefs.h"
#include "PDBnode_t.h"

namespace pdb {

    // Forward declaration
    template<typename NodeT>
    class open_t;

    // Class definition
    //
    // Open lists contain nodes of any type provided that they satisfy the type
    // constraint pdb_type, e.g., npancakes
    template<typename T>
    requires pdb_type<T>
    class open_t<node_t<T>> {

    private:

        // INVARIANT: an open list consists of a vector of vectors of nodes of
        // any underlying type satisfying the requirement pdb_type. All
        // operations preserve the range [mini,maxi] of values containing items
        // unless size=0 in which case mini=maxi=1
        std::vector<std::vector<node_t<T>>> _queue; // f-indexes to the buckets
        size_t _size;                        // number of elements in the queue
        int _mini;                                             // minimum index
        int _maxi;                                             // maximum index

    public:

        // Default constructor
        open_t ():
            _queue  { std::vector<std::vector<node_t<T>>> (1, std::vector<node_t<T>>() ) },
            _size   { 0 },                              // there is only one bucket!
            _mini   { 1 },        // INVARIANT: _mini and _maxi have to be above the
            _maxi   { 1 }           // current number of items if the queue is empty
            { }

        // Explicit constructor - it is also possible to create a bucket giving
        // the initial number of buckets
        explicit open_t (size_t capacity):
            _queue  { std::vector<std::vector<node_t<T>>> (capacity, std::vector<node_t<T>>() ) },
            _size   { 0 },
            _mini   { 1 },       // INVARIANT: _mini and _maxi have to be above the
            _maxi   { 1 }          // current number of items if the queue is empty
            { }

        // Copy and assignment constructors are explicitly forbidden
        open_t (const open_t&) = delete;
        open_t (open_t&&) = delete;

        // Assignment and move operators are disallowed as well
        open_t& operator=(const open_t&) = delete;
        open_t& operator=(open_t&&) = delete;

        // get accessors
        const size_t get_nbbuckets () const
            { return _queue.size (); }
        const size_t size () const
            { return _size; }
        const size_t size (const int idx) const
            { return _queue[idx].size (); }
        const int get_mini () const
            { return _mini; }
        const int get_maxi () const
            { return _maxi; }

        // set accessors

        // set the number of buckets to the value specified. In case the number of
        // buckets requested is too large an exception is thrown
        const size_t set_nbbuckets (const size_t nbbuckets);

        // methods

        // add the given item to the bucket corresponding to its g-value. It
        // returns true if the operation was successful and false otherwise. It
        // also takes care of preserving the consistency of other internal data.
        // It raises an exception in case the index exceeds the maximum capacity
        // of the buckets
        //
        // It specifically verifies that the number of buckets is enough for
        // inserting the new item. If not, additional space is created
        bool insert (const node_t<T>& item);

        // remove returns and erases the first item with the specified index. If
        // the bucket is empty an exception is thrown. It also takes care of
        // preserving the consistency of other internal data
        node_t<T> remove (const int idx);

        // pop_front extracts the first item with the minimum index
        node_t<T> pop_front ()
            { return remove (_mini); }

        // returns the first element without popping it
        node_t<T> front () {
            return std::move(_queue[_mini].back ());
        }
    }; // class open_t<node_t<T>>

    // set the number of buckets to the value specified. In case the number of
    // buckets requested is too large an exception is thrown
    template<typename T>
    requires pdb_type<T>
    const size_t open_t<node_t<T>>::set_nbbuckets (const size_t nbbuckets) {

        // if too many buckets were requested an exception is thrown
        if (nbbuckets >= _queue.max_size ()) {
            throw std::range_error ("[bucketvd::set_nbbuckets] too many buckets!");
        }

        // if the current number of buckets is below the number requested then
        // resize the queue as needed
        auto sz = _queue.size ();
        while (sz < nbbuckets) {
            sz *= 2;
        }
        _queue.resize (sz);

        // and return the current number of buckets
        return _queue.size ();
    }

    // add the given item to the bucket corresponding to its g-value. It returns
    // true if the operation was successful and false otherwise. It also takes
    // care of preserving the consistency of other internal data. It raises an
    // exception in case the index exceeds the maximum capacity of the buckets
    //
    // It specifically verifies that the number of buckets is enough for
    // inserting the new item. If not, additional space is created
    template<typename T>
    requires pdb_type<T>
    bool open_t<node_t<T>>::insert (const node_t<T>& item)
    {

        // this node should be stored in the bucket corresponding to its g-value
        int idx = item.get_g ();

        // ensure this bucket can accomodate values in the i-th slot. 1 is added
        // because the first index is 0.
        set_nbbuckets (1+idx);

        // insertion --- bucketed vector-vectors are used to insert by the back
        _queue [idx].push_back (item);

        // update internal data
        _size++;
        _mini = (_size == 1) ? idx : std::min (idx, _mini);
        _maxi = (_size == 1) ? idx : std::max (idx, _maxi);

        // exit with success
        return true;
    }

    // remove returns and erases the first item with the specified index. If
    // the bucket is empty an exception is thrown. It also takes care of
    // preserving the consistency of other internal data
    template<typename T>
    requires pdb_type<T>
    node_t<T> open_t<node_t<T>>::remove (const int idx)
    {

        // in case the corresponding bucket is empty, throw an exception
        if (!_queue [idx].size ()) {
            throw std::domain_error ("[open_t::remove] The idx-th bucket is empty!");
        }

        // extract the first item from the idx bucket and erase it
        node_t<T> item = std::move(_queue[idx].back ());
        _queue [idx].pop_back ();

        // internal data
        _size--;                           // decrement the overall number of items

        // update the current range
        if (!_size) {            // if there are no more items in all the structure
            _mini = _maxi = 1;                      // then update the current bounds
        }
        else {
            if (!_queue [idx].size ()) {              // if this bucket becomes empty

                // locate the first non-empty bucket and update the corresponding bound
                if (idx <= _mini) {           // in case we are below the maximum index
                    for (_mini=idx;_mini<=_maxi && !_queue[_mini].size ();_mini++);// fwd
                }
                if (idx >= _maxi) {
                    for (_maxi=idx;_maxi>=_mini && !_queue[_maxi].size ();_maxi--);// bwd
                }
            }
        }

        // and exit
        return item;
    }

} // namespace pdb


#endif // _PDBOPEN_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
