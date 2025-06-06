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

#include<cstdint>
#include<filesystem>
#include<fstream>
#include<iomanip>
#include<limits>

#include "PDBpdb.h"
#include "../gui/PDBprogress_t.h"

namespace pdb {

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
        // in the abstract state space abstracted with a given _c_pattern. To do
        // this, they use a closed list which is the PDB itself. As a result,
        // the minimum cost of every abstract state, according to a second
        // pattern, _p_pattern, is stored in a final PDB.
        //
        // In case _p and _c_pattern are different, then every symbol masked (or
        // abstracted) in _c_pattern must be abstracted as well in _p_pattern.
        // In other words, the abstract space induced by _c_pattern must be a
        // superset or equal to the abstract state space induced by _p_pattern.
        //
        // Data members are available in the base class to register the explicit
        // definition of the goal, and both patterns. It also records the final
        // PDB generated.

        // outPDBs also gather some statistics
        std::chrono::duration<double, std::milli> _elapsed_time;
        size_t _nbexpansions;

    private:

        // write the binary data given as a vector of uint8_t at the end of the
        // ofstream out and return true if the operation was feasible and false
        // otherwise
        bool _write (std::ofstream& out, std::vector<uint8_t> data) {

            out.write(reinterpret_cast<const char*>(data.data ()),
                      static_cast<std::streamsize>(data.size()));
            if (!out) {
                return false;
            }
            return true;
        }

        // return a vector of binary data with the contents of another vector
        // which consists of ints. In case any of the ints exceed the maximum
        // value for uint8_t, an exception is raised
        void _int_to_binary (const std::vector<int> data,
                             std::vector<uint8_t>& result) {

            for (const auto item: data) {
                if (item > std::numeric_limits<uint8_t>::max()) {
                    throw std::runtime_error ("[_int_to_binary] an item was found whose vallue exceeds the range of uint8_t");
                }
                result.push_back (uint8_t (item));
            }
        }

        // return a vector of binary data with the contents of a string. In case
        // any of the characters exceed the maximum value for uint8_t, an
        // exception is raised
        void _sv_to_binary (const std::string data,
                            std::vector<uint8_t>& result) {

            for (const auto item: data) {
                result.push_back (uint8_t (item));
            }
        }

    public:

        // Default constructors are forbidden
        outpdb () = delete;

        // Explicit constructor ---it is mandatory to provide the pdb mode, the
        // goal and both patterns, those used for generating the pattern
        // (p_pattern) and also the one used to search (c_pattern)
        outpdb (pdb_mode mode,
                const std::vector<int>& goal,
                const std::string cpattern,
                const std::string ppattern) :
            pdb<node_t<T>>(mode, goal, cpattern, ppattern),
            _nbexpansions              {                       0 }
            { }

        // getters
        const std::chrono::duration<double, std::milli> get_elapsed_time () const {
            return _elapsed_time;
        }
        const size_t get_nbexpansions () const {
            return _nbexpansions;
        }

        // methods

        // generate a PDB with the minimum cost to reach the goal defined in
        // this instance from every abstract state as given in the ppatern used
        // to create this instance. The resulting PDB is internallly stored. To
        // write it down to a file use 'write'
        //
        // If cpattern induces a superset of the abstract state space induced by
        // ppatern, then the resulting PDB contains the minimum cost among all
        // entries in the state space induced by cpattern that are mapped to the
        // same abstract state in the state space induced by ppatern.
        //
        // If the cost of an abstract state exceeds the range of pdbval_t, then
        // a runtime_error is immediately raised
        //
        // In case console takes the value true, a progress bar is shown on the
        // standard output
        void generate (bool console=false) {

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

            pdboff_t cspace = pdb_t<node_t<T>>::address_space (pdb<node_t<T>>::_c_pattern);
            pdb_t<node_t<T>> cpdb (cspace);
            cpdb.init (pdb<node_t<T>>::_goal, pdb<node_t<T>>::_c_pattern);

            // next, abstract the goal state. The _c_pattern is used here, since
            // this is the pattern used during the search
            std::vector<int> agoal = cpdb.mask (pdb<node_t<T>>::_goal);

            // and seed the open list with this abstract state and g=1. The
            // g-value of all annotations in a PDB are incremented in one unit
            // to be distinguished with pdbzero (which is zero)
            open_t<node_t<T>> open;
            open.insert (node_t (T (agoal), 1));

            // Also, create a progress bar to be displayed in case console
            // takees the value true. Note the upper bound is defined over the
            // number of items to be generated by the search algorithm, and not
            // the number of entries in the final PDB, just because the former
            // is assumed to be more accurate.
            progress_t progress_bar (cspace);
            progress_bar.set_prefix (" Generating PDB ");

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

                // the progress bar is relative to the search and not to the
                // entries in the pattern database ---just simply because this
                // seems to be more accurate
                if (console) {
                    progress_bar.set_value (cpdb.size ());
                    progress_bar.show ();
                }

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

                    // before continuing ensure that the g-value of the child
                    // does not exceed the max value of pdbval_t
                    if (std::numeric_limits<pdbval_t>::max() - node.get_g () < g) {
                        throw std::runtime_error (" [outpdb.generate] g(child) out of range");
                    }

                    // otherwise, add it to open
                    open.insert (node_t (ichild, g + node.get_g ()));
                }
            }

            // stop the chrono and register the elapsed time
            auto stop = std::chrono::high_resolution_clock::now();
            _elapsed_time = stop - start;
        }

        // verify that data has been seemingly well created. Seemingly, because
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
        //
        // IMPORTANT: Use 'doctor' before 'write'
        bool doctor () {

            // count the number of locations with a value equal to 1
            int nbones = 0;

            // verify the number of expansions is equal to the size of the
            // abstract state space
            pdboff_t pspace = pdb_t<node_t<T>>::address_space (pdb<node_t<T>>::_p_pattern);
            if (_nbexpansions != pspace) {
                pdb<node_t<T>>::_error = error_message::address_space;
                return false;
            }

            // traverse the whole state space of the PDB generated
            for (auto address = 0 ; address < pspace ; address++) {

                // check this position has a value other than pdbzero
                if (pdb<node_t<T>>::_pdb->at (address) == pdbzero) {
                    pdb<node_t<T>>::_error = error_message::zero;
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
                pdb<node_t<T>>::_error = error_message::nb_ones;
                return false;
            }

            // At this point, the PDB is deemed as being correctly generated,
            // but cross your fingers!!
            return true;
        }

        // return true if it was possiblle to write the contents of the
        // generated pdb into the specified file and false otherwise. The binary
        // file is started with a header that contains the following info:
        //
        // 1. pdb mode: either MAX or ADD
        // 2. The goal given in explicit form
        // 3. The pattern used to generate the abstract state space
        bool write (const std::filesystem::path& path) {

            // Try to open the file and if it is not possible, then return false
            std::ofstream out(path, std::ios::binary | std::ios::trunc);
            if (!out.is_open()) {
                return false;
            }

            // Now write the binary data stored in the pdb in case there is any
            if (pdb<node_t<T>>::_pdb != nullptr) {

                // first, write the header which consists of:
                // 1. The pdb mode (_mode): MAX or ADD
                std::vector<uint8_t> header;
                if (pdb<node_t<T>>::_mode == pdb_mode::max) {
                    header = std::vector<uint8_t> {'M', 'A', 'X'};
                } else {
                    header = std::vector<uint8_t> {'A', 'D', 'D'};
                }

                // 2. The length of the goal (_n): which has to be equal to the
                //    length of both patterns
                header.push_back (uint8_t (pdb<node_t<T>>::_pdb->get_n ()));

                // 3. The goal (_goal): consists of a explicit definition of
                //    the goal state in the true state space
                std::vector<uint8_t> goal;
                _int_to_binary (pdb<node_t<T>>::_goal, goal);
                header.insert (header.end (), goal.begin (), goal.end ());

                // 3. The ppattern (_p_pattern): used to generate this PDB
                std::vector<uint8_t> ppattern;
                _sv_to_binary (pdb<node_t<T>>::_p_pattern, ppattern);
                header.insert (header.end (), ppattern.begin (), ppattern.end ());

                // 4. The cpattern (_c_pattern): used to determine the abstract
                //    space to traverse to generate the PDB
                std::vector<uint8_t> cpattern;
                _sv_to_binary (pdb<node_t<T>>::_c_pattern, cpattern);
                header.insert (header.end (), cpattern.begin (), cpattern.end ());

                // and write the header
                if (!_write (out, header)) {
                    return false;
                }

                // Before writting the values in the PDB to the file, decrement
                // every value in one unit. The reason is that the PDB
                // generation increments the g-value of the start state in one
                // unit to distinguish empty locations from those with a g-value
                // equal to zero (e.g., the abstract goal state), so that they
                // have to be decremented now
                for (auto i = 0 ; i < pdb<node_t<T>>::_pdb->size () ; i++) {
                    (*pdb<node_t<T>>::_pdb) [i] = (*pdb<node_t<T>>::_pdb) [i]-1;
                }

                // Finally, write the PDB binary data into this file
                if (!_write (out, pdb<node_t<T>>::_pdb->get_address ())) {
                    return false;
                }
            }

            return true;
        }

    }; // class outpdb<node_t<T>>

} // namespace pdb

#endif // _PDBOUTPDB_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
