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
#include <vector>

namespace pdb {

    // Type definitions
    //
    // Types of PDBs
    enum class pdb_mode {max, add};

    // Plausible errors during the generation (outpdb/inpdb) of PDBs
    //
    //    no_error: everything *seems* okay
    //
    //    address_space: the number of values differes from the theoretical size
    //                   of the PDB which is computed with the static service
    //                   pdb_t::address_space
    //
    //    nb_ones: either zero or more than one entries have been found with the
    //             value 1
    //
    //             output PDBs (outpdb): since the g-value of all locations is
    //             incremented in one unit (to distinguish empty locations from
    //             those with a g-value equal to zero), there can be only one
    //             entry with a final value equal to one in MAX PDBs, the
    //             abstract state space ---if doctor is used before write. This
    //             error is ignored in the case of output ADD PDBs.
    //
    //             input PDBs (inpdb): this error is fully ignored in both MAX
    //             and ADD input PDBs
    //
    //    zero: entries found with the value zero
    //
    //          output PDBs (outpdb): since the g-value of all locations is
    //          incremented in one unit (to distinguish empty locations from
    //          those with a g-value equal to zero), there can not be ever any
    //          entry with a final value equal to zero ---if doctor is used
    //          before write
    //
    //          input PDBs (inpdb): when reading PDBs, there can be only one
    //          entry with a value equal to zero if it is a MAX PDB. In the case
    //          of ADD PDBs this error is ignored since there can be arbitrary
    //          number of entries with value zero.
    enum class error_message {
        no_error,
        address_space,
        nb_ones,
        zero
    };

    // Plausible errors while reading PDBs. Their meaning is self explanatory
    enum class in_error_message{
        no_error,
        file_does_not_exist,
        non_regular_file,
        file_could_not_be_opened,
        size_could_not_be_determined,
        pdb_mode_could_not_be_read,
        pdb_length_could_not_be_read,
        pdb_goal_could_not_be_read,
        pdb_ppattern_could_not_be_read,
        pdb_cpattern_could_not_be_read,
        pdb_incorrect_size,
        pdb_g_values_could_not_be_read
    };

    // indices to the pattern database are as long as size_t
    typedef unsigned long long int pdboff_t;

    // values stored in a pdb are (for the time being) unsigned bytes
    typedef uint8_t pdbval_t;

    // Type constraints
    //
    // PDB nodes are generated over a predefined type that has to provide a
    // number of services described below
    template<typename T>
    concept pdb_type = requires (T item, std::vector<std::tuple<pdbval_t, T>> successors) {

        // PDBs are created by running a backwards brute-force breadth-first search.
        // It is necessary for the items to be able to compute their predecessors
        // (here noted as successors)
        item.children (successors);

        // PDBs must rank permutations for accessing locations, and these must
        // be returned by those types used for creating PDBs
        { item.get_perm () } -> std::same_as<const std::vector<int>&>;
    };

    // Constants
    //
    // An entry equal to zero in the pattern database means unused entry. Because of
    // this, the g*-values of all entries in the PDB are incremented intentionally
    // in one unit and decreased only at the time they are written down to the file.
    constexpr pdbval_t pdbzero = 0;

    // An abstracted symbol is shown with NONPAT
    constexpr uint8_t NONPAT = 0xff;

} // namespace pdb

#endif // _PDBDEFS_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
