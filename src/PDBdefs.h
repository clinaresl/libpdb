// -*- coding: utf-8 -*-
// PDBdefs.h
// -----------------------------------------------------------------------------
//
// Started on <sáb 03-05-2025 21:33:43.092676345 (1746300823)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Global definitions
//

#ifndef _PDBDEFS_H_
#define _PDBDEFS_H_

#include <cstdint>

namespace pdb {

    // Constants
    //
    // Types of PDBs
    enum class pdb_mode {max, add};

    // An entry equal to zero in the pattern database means unused entry. Because of
    // this, the g*-values of all entries in the PDB are incremented intentionally
    // in one unit and decreased only at the time they are written down to the file.
    constexpr uint8_t pdbzero = 0;

    // An abstracted symbol is shown with NONPAT
    constexpr uint8_t NONPAT = 0xff;

    // Type definitions
    //
    // indices to the pattern database are as long as size_t
    typedef unsigned long long int pdboff_t;

    // values stored in a pdb are (for the time being) unsigned bytes
    typedef uint8_t pdbval_t;

    // Type constraints
    //
    // PDB nodes are generated over a predefined type that has to provide a
    // number of services described below
    template<typename T>
    concept pdb_type = requires (T item, std::vector<std::tuple<uint8_t, T>> successors) {

        // PDBs are created by running a backwards brute-force breadth-first search.
        // It is necessary for the items to be able to compute their predecessors
        // (here noted as successors)
        item.children (successors);

        // items are stored in a pdb using a perfect ranking function (e.g.,
        // Myrvold&Ruskey or lexicographical ordering, which is worst ...). items
        // therefore must be able to compute their own ranking which must be given
        // as a value of type pdboff_t ---defined above
        { item.rank_pdb () } -> std::same_as<pdboff_t>;
    };

} // namespace pdb

#endif // _PDBDEFS_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
