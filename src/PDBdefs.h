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

// Types of PDBs
enum class pdb_mode {max, add};

// Type constraints
//
// PDB nodes are generated over a predefined type that has to provide a number
// of methods so that PDBs can use it to construct the pattern databases
template<typename T>
concept pdb_type = requires (T item, std::vector<std::tuple<uint8_t, T>> successors) {
    item.children (successors);
};


#endif // _PDBDEFS_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
