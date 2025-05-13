// -*- coding: utf-8 -*-
// PDBpdb.h
// -----------------------------------------------------------------------------
//
// Started on <jue 08-05-2025 10:10:24.932497743 (1746691824)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Base defintion of PDBs over permutation state spaces
//

#ifndef _PDBPDB_H_
#define _PDBPDB_H_

#include<chrono>
#include<tuple>
#include<vector>

#include "../PDBdefs.h"
#include "../structs/PDBopen_t.h"
#include "../structs/PDBpdb_t.h"

namespace pdb {

    // Custom deleter used to avoid calling a destructor
    template<typename T>
    struct _no_op_deleter {
        void operator()(T *ptr) const noexcept {
            // Do nothing: pass
        }
    };

    // Forward declaration
    template<typename PDBNodeT>
    class pdb;

    // Class definition
    //
    // PDBs use nodes of any type provided that they satisfy the type constraint
    // pdb_type, e.g., npancakes
    template<typename T>
    requires pdb_type<T>
    class pdb<node_t<T>> {

    protected:

        // INVARIANT: PDBs are defined given a pattern wrt the explicit
        // definition of a goal, the _p_pattern. To compute the minimum cost of
        // every abstract state according to the _p_pattern, a backwards
        // breadth-first search is conducted which abstract states according to
        // a second pattern, _c_pattern.
        //
        // They can be either generated traversing the abstract state space
        // (outpdb), or they can be loaded from a file (inpdb).
        std::vector<int> _goal;
        std::string _c_pattern;
        std::string _p_pattern;

        // the type of the PDB is stored in the following data member
        pdb_mode _mode;

        // As a result the PDB is stored internally
        pdb_t<node_t<T>> *_pdb;

        // Which, when being either generated (outPDBs) or read (inPDBs) could
        // have some errors
        error_message _error;

    public:

        // Default constructors are forbidden
        pdb () = delete;

        // Explicit constructor ---it is mandatory to provide the goal and the
        // pattern used for generating abstract states (p_pattern)
        pdb (pdb_mode mode,
             const std::vector<int>& goal,
             const std::string_view cpattern,
             const std::string_view ppattern) :
            _goal         {                   goal },
            _c_pattern    {               cpattern },
            _p_pattern    {               ppattern },
            _mode         {                   mode },
            _pdb          {                nullptr },
            _error        { error_message::no_error}
            {}

        // provide a destructor to free the memory allocated to _pdb
        ~pdb () {
            if (_pdb != nullptr) {
                delete _pdb;
                _pdb = nullptr;
            }
        }

        // getters
        const pdb_mode get_pdb_mode () const {
            return _mode;
        }
        const std::vector<int>& get_goal () const {
            return _goal;
        }
        const std::string get_cpattern () const {
            return _c_pattern;
        }
        const std::string get_ppattern () const {
            return _p_pattern;
        }
        const error_message get_error () const {
            return _error;
        }

        // operator overloading

        // Given a correct index to the address space in this PDB, return the
        // value at its location. In case the index is out of bounds, the
        // behaviour is undefined
        const pdbval_t& operator[] (const pdboff_t index) const {
            return (*_pdb)[index];
        }

        // get the value corresponding to the given permutation as a vector of
        // integers. The permutation is ranked according to the pattern given to
        // _pdb. Make sure to invoke this service on outpdbs only after invoking
        // 'generate' and with inpdbs only after using 'read'
        const pdbval_t operator[] (const std::vector<int>& perm) const {

            // rank the permutation with the p-pattern stored in this instance,
            // and return the value at that position
            return _pdb->at (_pdb->rank (perm));
        }


        // Given a correct index to the address space in this PDB, return a
        // reference to its location so it can be overwritten. In case the index
        // is out of bounds, an exception is raised.
        pdbval_t& operator[] (const pdboff_t index) {
            return (*_pdb)[index];
        }

        // methods

        // return a string representing the current error
        std::string get_error_message () const {
            std::string output;
            switch (_error) {
                case error_message::no_error:
                    output = "No error";
                    break;
                case error_message::address_space:
                    output = "Address space";
                    break;
                case error_message::nb_ones:
                    output = "Number of ones";
                    break;
                case error_message::zero:
                    output = "Zero entries found";
                    break;
            }
            return output;
        }

        // return the number of available positions in the PDB
        size_t capacity () const {
            return _pdb->capacity ();
        }

        // return the number of positions written in this PDB. Note that the
        // value returned refers to the number of times that _pdb->insert was
        // used because _pdb could also be written using operator[] but it does
        // not update its _size
        pdboff_t size () const {

            // take into account that the _pdb might have not been generated
            // yet!
            if (_pdb == nullptr) {
                return 0;
            }
            return _pdb->size ();
        }

    }; // class pdb<node_t<T>>
}

#endif // _PDBPDB_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
