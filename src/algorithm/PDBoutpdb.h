// -*- coding: utf-8 -*-
// PDBoutpdb.h
// -----------------------------------------------------------------------------
//
// Started on <sáb 10-05-2025 02:21:36.568442082 (1746836496)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Generation of PDBs in permutation state spaces
//

#ifndef _PDBOUTPDB_H_
#define _PDBOUTPDB_H_

#include "PDBpdb.h"

namespace pdb {

    enum class error_message {no_error, address_space, nb_ones, zero};

    // Forward declaration
    template<typename PDBNodeT>
    class outpdb;

    // Class definitionn
    //
    // outpdbs generate PDBS using nodes of any type provided that they satisfy
    // the type constraint pdb_type, e.g., npancakes
    template<typename T>
    requires pdb_type<T>
    class outpdb<node_t<T>> : public pdb<node_t<T>> {

    private:

        // INVARIANT: outPDBs run a backwards breadth-first search from a _goal
        // in the abstract state space abstracted with a given _p_pattern. To do
        // this, they use a closed list implemented as another PDB which is
        // initialized with its own _c_pattern. In case _p and _c_pattern are
        // different, then every symbol masked (or abstracted) in _c_pattern
        // must be abstracted as well in _p_pattern. In other words, the
        // abstract space induced by _c_pattern must be a superset or equal to
        // the abstract state space induced by _p_pattern.
        //
        // The explicit definition of the goal and also the ppatern are given in
        // the base class
        std::string_view _c_pattern;

        // outPDBs also gather some statistics
        std::chrono::duration<double, std::milli> _elapsed_time;
        size_t _nbexpansions;

        // among them, the type of error, if any
        error_message _error;

    public:

        // Default constructors are forbidden
        outpdb () = delete;

        // Explicit constructor ---it is mandatory to provide the goal and both
        // patterns, those used for generating the pattern (p_pattern) and also
        // the one used to search (c_pattern)
        outpdb (const std::vector<int>& goal,
                const std::string_view cpattern,
                const std::string_view ppattern) :
            pdb<node_t<T>>(goal, ppattern),
            _c_pattern                 {                cpattern },
            _nbexpansions              {                       0 },
            _error                     { error_message::no_error }
            { }

        // getters
        const std::string_view get_cpattern () const {
            return _c_pattern;
        }
        const std::chrono::duration<double, std::milli> get_elapsed_time () const {
            return _elapsed_time;
        }
        const size_t get_nbexpansions () const {
            return _nbexpansions;
        }
        const error_message get_error () const {
            return _error;
        }

        // methods

        // generate a PDB with the minimum cost to reach the goal defined in
        // this instance from every abstract state as given in the ppatern used
        // to create this instance. The resulting PDB is internallly stored.
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
            pdboff_t pspace = pdb_t<node_t<T>>::address_space (pdb<node_t<T>>::_p_pattern);
            auto _pdb_raw = ::operator new (sizeof (pdb_t<node_t<T>>(pspace)));
            pdb<node_t<T>>::_pdb = new (_pdb_raw) pdb_t<node_t<T>> (pspace);
            pdb<node_t<T>>::_pdb->init (pdb<node_t<T>>::_goal, pdb<node_t<T>>::_p_pattern);

            pdboff_t cspace = pdb_t<node_t<T>>::address_space (_c_pattern);
            pdb_t<node_t<T>> cpdb (cspace);
            cpdb.init (pdb<node_t<T>>::_goal, _c_pattern);

            // next, abstract the goal state. The _c_pattern is used here, since
            // this is the pattern used during the search
            std::vector<int> agoal = cpdb.mask (pdb<node_t<T>>::_goal);

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

                // check whether this abstract state has been expanded before or
                // not
                if (cpdb.find (node) != std::string::npos) {

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
                std::vector<int> pperm = pdb<node_t<T>>::_pdb->mask (node.get_state ().get_perm ());
                node_t<T> pnode = node_t<T>(T (pperm), node.get_g ());
                if (pdb<node_t<T>>::_pdb->find (pnode) == std::string::npos) {

                    // if it is not found, then annotate the g-value of this
                    // node (which was incremented in one unit) in the PDB
                    pdb<node_t<T>>::_pdb->insert (pnode);
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

        // verify that data has been seemingly well written. Seemingly, because
        // there is no formal way to verify the contents of a PDB. It just
        // performs the folllowing operations:
        //
        // 1. It checks there is only one entry with the value 1 (which because
        //    they are incremented, should correspond to the abstract goal
        //    state, and there should be only one) (error nb_ones)
        //
        // 2. Verify there is no entry with the value pdbzero (error zero)
        //
        // 3. It also verifies that the number of nodes being expanded is equal
        //    to the size of the abstract state space (error address_space)
        //
        // In case an error is diagnosed, _error is updated and
        // get_error_message can be used to get a string explaining the error
        // found. It reports only one error, if any, even if the PDB is
        // inconsistent in more than one regard.
        bool doctor () {

            // count the number of locations with a value equal to 1
            int nbones = 0;

            // verify the number of expansions is equal to the size of the
            // abstract state space
            pdboff_t pspace = pdb_t<node_t<T>>::address_space (pdb<node_t<T>>::_p_pattern);
            if (_nbexpansions != pspace) {
                _error = error_message::address_space;
                return false;
            }

            // traverse the whole state space of the PDB generated
            for (auto address = 0 ; address < pspace ; address++) {

                // check this position has a value other than pdbzero
                if (pdb<node_t<T>>::_pdb->at (address) == pdbzero) {
                    _error = error_message::zero;
                    return false;
                }

                // check whether this entry  has a value equal to 1
                if (pdb<node_t<T>>::_pdb->at(address) == pdbval_t (1)) {
                    nbones++;
                }
            }

            // Before leaving, ensure there is only one location with a value
            // equal to 1
            if (nbones != 1) {
                _error = error_message::nb_ones;
                return false;
            }

            // At this point, the PDB is deemed as being correctly generated,
            // but cross your fingers!!
            return true;
        }

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

    }; // class outpdb<node_t<T>>

} // namespace pdb

#endif // _PDBOUTPDB_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
