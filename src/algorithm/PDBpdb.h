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
        // definition of a goal. They can be either generated traversing the
        // abstract state space (outpdb), or they can be loaded from a file
        // (inpdb).
        std::vector<int> _goal;
        std::string_view _p_pattern;

        // As a result the PDB is stored internally
        pdb_t<node_t<T>> *_pdb;

    public:

        // Default constructors are forbidden
        pdb () = delete;

        // Explicit constructor ---it is mandatory to provide the goal and the
        // pattern used for generating abstract states (p_pattern)
        pdb (const std::vector<int>& goal,
             const std::string_view ppattern) :
            _goal         {     goal },
            _p_pattern    { ppattern },
            _pdb          {  nullptr }
            {}

        // provide a destructor to free the memory allocated to _pdb
        ~pdb () {
            if (_pdb != nullptr) {
                delete _pdb;
            }
        }

        // getters
        const std::vector<int>& get_goal () const {
            return _goal;
        }
        const std::string_view get_ppattern () const {
            return _p_pattern;
        }

        // methods

        // return the number of positions of this PDB
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
