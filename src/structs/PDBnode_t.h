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

namespace pdb {

    // Type constraints
    //
    // nodes are generated over a predefined type that has to provide a number
    // of methods so that PDBs can use it to construct the pattern database
    template<typename T>
    concept pdb_type = requires (T item, std::vector<std::tuple<uint8_t, T>> successors) {
        item.children (successors);
    };

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
        const uint8_t get_g () const {
            return _g;
        }

    }; // class node_t<pdb_type T>

} // namespace pdb

#endif // _PDBNODE_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
