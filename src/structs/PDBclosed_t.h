// -*- coding: utf-8 -*-
// PDBclosed_t.h
// -----------------------------------------------------------------------------
//
// Started on <dom 04-05-2025 00:32:07.795224855 (1746311527)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Definition of a closed list
//

#ifndef _PDBCLOSED_T_H_
#define _PDBCLOSED_T_H_

#include<iterator>
#include<memory>
#include<string>
#include<utility>
#include<vector>

#include "../PDBdefs.h"
#include "PDBnode_t.h"

namespace pdb {

    // Class definition
    //
    // The underlying type of closed lists are of any type satisfying the type
    // constraint pdb_type. Some ops, however, require that items are given
    // wrapped in a type node that, by default is node_t<T>
    template<pdb_type T, typename Node=node_t<T>>
    class closed_t {

    private:

        // INVARIANT: closed lists are used mostly for membership operations.
        // Because the number of items to store in closed is known in advance
        // and a perfect hashing function is used, it is implemented as a vector
        // of pdbval_t so that:
        //
        //    1. Non-empty positions contain their g*-value
        //    2. Empty entries are represented with pdbzero
        //    3. Empty positions are associated with the index string::npos
        std::vector<pdbval_t> _closed;

        // The container also counts the number of items stored in closed
        size_t _size;

    public:

        // Default constructors are forbidden
        closed_t () = delete;

        // Explicit constructor ---it is mandatory to provide the number of
        // items that will be stored in closed
        explicit closed_t (const int size) :
            _closed { std::vector<pdbval_t>(size, pdbzero) },
            _size { 0 }
            {}

        // Copy and assignment constructors are explicitly forbidden
        closed_t (const closed_t&) = delete;
        closed_t (closed_t&&) = delete;

        // Assignment and move operators are disallowed as well
        closed_t& operator=(const closed_t&) = delete;
        closed_t& operator=(closed_t&&) = delete;

        // modifiers

        // insert an item given within a Node into closed and return a stable
        // index to it in closed. Inserting an item means just writing down its
        // g*-value
        pdboff_t insert (const Node& item) {

            // The position where it has to be inserted must be given by a
            // ranking function provided by the item itself ---see the
            // definition of the type constraint pdb_type
            pdboff_t index = item.get_state ().rank_pdb ();

            // Insert this item at the right location (just simply by storing
            // its g-value, which is a service provided by the Node) and
            // increment the count of the number of alive items in closed
            _closed[index] = item.get_g ();
            _size++;

            // and return the location of this item into the vector
            return index;
        }

        // Lookup

        // given a stable index into closed, return the value stored at that
        // location. In case the index is incorrect, the behaviour is undefined
        pdbval_t operator[] (const pdboff_t index) {
            return _closed[index];
        }

        // return a stable index to the item given (as a node_t) in case it is
        // found in the vector of the closed list. Otherwise, return
        // string::npos
        pdboff_t find (const Node& item) const {

            // get the location where the item should be found
            pdboff_t index = item.get_state ().rank_pdb ();

            // in case this location has not been written yet, return
            // string::npos
            if (_closed[index] == pdbzero) {
                return std::string::npos;
            }

            // Otherwise, return a stable index to its location in closed
            return index;
        }

        // return the number of locations in closed
        size_t capacity () const {
            return _closed.capacity ();
        }

        // return the number of elements written in closed
        size_t size () const
            { return _size; }

    }; // class closed_t<T>

} // namespace pdb

#endif // _PDBCLOSED_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
