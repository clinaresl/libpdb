// -*- coding: utf-8 -*-
// PDBpdb.h
// -----------------------------------------------------------------------------
//
// Started on <jue 08-05-2025 10:10:24.932497743 (1746691824)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Generation of PDBs over state spaces
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

    private:

        // INVARIANT: PDBs run a backwards breadth-first search from a _goal
        // in the abstract state space abstracted with a given _p_pattern.
        // To do this, they use a closed list implemented as another PDB
        // which is initialized with its own _c_pattern. In case _p and
        // _c_pattern are different, then every symbol masked (or
        // abstracted) in _c_pattern must be abstracted as well in
        // _p_pattern. In other words, the abstract space induced by
        // _c_pattern must be a superset or equal to the abstract state
        // space induced by _p_pattern.
        std::vector<int> _goal;
        std::string_view _c_pattern;
        std::string_view _p_pattern;

        // PDBs generation also gather some statistics
        std::chrono::duration<double, std::milli> _elapsed_time;
        size_t _nbexpansions;

    public:

        // Default constructors are forbidden
        pdb () = delete;

        // Explicit constructor ---it is mandatory to provide the goal and both
        // patterns, those used for generating the pattern (p_pattern) and also
        // the one used to search (c_pattern)
        pdb (const std::vector<int>& goal,
             const std::string_view cpattern,
             const std::string_view ppattern) :
            _goal         {     goal },
            _c_pattern    { cpattern },
            _p_pattern    { ppattern },
            _nbexpansions {        0 }
            {}

        // getters
        const std::vector<int>& get_goal () const {
            return _goal;
        }
        const std::string_view get_cpattern () const {
            return _c_pattern;
        }
        const std::string_view get_ppattern () const {
            return _p_pattern;
        }
        const size_t get_nbexpansions () const {
            return _nbexpansions;
        }
        const std::chrono::duration<double, std::milli> get_elapsed_time () const {
            return _elapsed_time;
        }

        // methods

        // generate returns a PDB with the minimum cost to reach the goal
        // defined in this instance from every abstract state as given in the
        // ppatern used to create this instance.
        //
        // If cpattern induces a superset of the abstract state space induced by
        // ppatern, then the resulting PDB contains the minimum cost among all
        // entries in the state space induced by cpattern that are mapped to the
        // same abstract state in the state space induced by ppatern.
        void generate () {

            // start the chrono
            auto start = std::chrono::high_resolution_clock::now();

            // first of all, create an initialize the PDBs used for searching
            // (_c_pattern) and also to store the minimum cost (_p_pattern). The
            // initialization of both PDBs is done wrt the same goal
            // description. Note that the goal description should be explicit,
            // i.e., no state should be abstracted
            pdboff_t pspace = pdb_t<node_t<T>>::address_space (_p_pattern);
            // std::cout << " address space (ppattern): " << pspace << std::endl; std::cout.flush ();
            pdb_t<node_t<T>> ppdb (pspace);
            ppdb.init (_goal, _p_pattern);

            pdboff_t cspace = pdb_t<node_t<T>>::address_space (_c_pattern);
            // std::cout << " address space (cpattern): " << cspace << std::endl; std::cout.flush ();
            pdb_t<node_t<T>> cpdb (cspace);
            cpdb.init (_goal, _c_pattern);

            // next, abstract the goal state. The _c_pattern is used here, since
            // this is the pattern used during the search
            std::vector<int> agoal = cpdb.mask (_goal);
            // std::cout << " goal: ";
            // for (auto iitem : agoal) {
            //     std::cout << iitem << " ";
            // }
            // std::cout << std::endl; std::cout.flush ();

            // and seed the open list with this abstract state and g=1. The
            // g-value of all annotations in a PDB are incremented in one unit
            // to be distinguished with pdbzero (which is zero)
            open_t<node_t<T>> open;
            open.insert (node_t (T (agoal), 1));

            // and compute the cost of all nodes in the abstract state space
            // induced by ppattern ---we search in the abstract state space
            // induced by cpattern, but select those entries which are a subset
            // of it, the abstract state space defined by ppatern
            while (open.size () > 0) {

                // take the first node from open. Nodes are extracted in
                // increasing order of their g-value
                node_t<T> node = open.pop_front ();

                // auto perm = node.get_state ().get_perm ();
                // std::cout << " \t Expanding node " << std::endl;
                // std::cout << " \t perm: ";
                // for (auto iitem : perm) {
                //     std::cout << iitem << " ";
                // }
                // std::cout << std::endl; std::cout.flush ();
                // std::cout << " \t    g: " << int (node.get_g ()) << std::endl << std::endl; std::cout.flush ();

                // first, look for it in closed
                // std::cout << " \t cPDB: " << std::endl; std::cout.flush ();
                // for (auto i = 0 ; i < cspace ; i++) {
                //     std::cout << " \t\t cPDB[" << i << "]: " << int (cpdb[i]) << std::endl;
                // }
                // std::cout.flush ();
                if (cpdb.find (node) != std::string::npos) {

                    // std::cout << " \t node found in closed!" << std::endl << std::endl; std::cout.flush ();

                    // If found, then skip it. The state space of the closed
                    // list is a superset (or equal) to the state space of the
                    // final pdb. Thus, if it is in closed, then a corresponding
                    // node in the pdb already has a value. In addition, if it
                    // has been expanded before, all its children are already
                    // under consideration
                    continue;
                }

                // in case it does not exist, add it!
                cpdb.insert (node);

                // next, look for it in the pdb. First, abstract this state
                // according to the pattern given for the creation of the PDB
                // (recall that nodes as traversed by the search algorithm are
                // masked with the pattern given to the closed list!). Use the
                // resulting permutation to create a node_t
                std::vector<int> pperm = ppdb.mask (node.get_state ().get_perm ());
                node_t<T> pnode = node_t<T>(T (pperm), node.get_g ());
                if (ppdb.find (pnode) == std::string::npos) {

                    // if it is not found, then annotate the g-value of this
                    // node (which was incremented in one unit) in the PDB
                    ppdb.insert (pnode);
                }

                // now, expand this abstract state and generate all children
                _nbexpansions++;
                std::vector<std::tuple<pdbval_t, T>> successors;
                T state = node.get_state ();
                state.children (successors);

                // and add all (abstract) children to open
                for (auto const& isuccessor : successors) {

                    // get the g value and also the new descendant
                    auto [g, ichild] = isuccessor;

                    // search in an abstract state generates many duplicates,
                    // thus it is worth preventing the generation of nodes that
                    // have been expanded before
                    //
                    // If this node has been expanded before
                    if (cpdb.find (node_t<T> (ichild)) != std::string::npos) {

                        // then do not add it to the open list
                        continue;
                    }

                    // otherwise, add it to open
                    open.insert (node_t (ichild, g + node.get_g ()));
                }
            }

            // stop the chrono and register the elapsed time
            auto stop = std::chrono::high_resolution_clock::now();
            _elapsed_time = stop - start;
        }

    }; // class pdb<node_t<T>>
}

#endif // _PDBPDB_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
