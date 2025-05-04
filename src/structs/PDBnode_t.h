// -*- coding: utf-8 -*-
// PDBnode_t.h
// -----------------------------------------------------------------------------
//
// Started on <vie 02-05-2025 18:29:46.680667245 (1746203386)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Definition of nodes as they are used by the PDB generator
//

#ifndef _PDBNODE_T_H_
#define _PDBNODE_T_H_

#include <stdint.h>
#include <vector>

#include "../PDBdefs.h"

namespace pdb {

    template <pdb_type T>
    class node_t {

    private:

        // INVARIANT: a pdbnode contains a state of the original problem and
        // also its g-value. The g-value is always assumed to fit in an unsigned
        // integer
        T _state;
        uint8_t _g;

    public:

        // Default constructors are forbidden
        node_t () = delete;

        // Explicit constructor
        node_t (const T& state, const uint8_t g=0) :
            _state { state},
            _g { g }
            {}

        // getters
        uint8_t get_g () const {
            return _g;
        }
        const T& get_state () const {
            return _state;
        }

    }; // class node_t<pdb_type T>

} // namespace pdb

#endif // _PDBNODE_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
