// -*- coding: utf-8 -*-
// PDBpdb_t.h
// -----------------------------------------------------------------------------
//
// Started on <dom 04-05-2025 00:32:07.795224855 (1746311527)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Definition of a PDB
//

#ifndef _PDBPDB_T_H_
#define _PDBPDB_T_H_

#include<algorithm>
#include<iostream>
#include<iterator>
#include<memory>
#include<string>
#include<utility>
#include<vector>

#include "../PDBdefs.h"
#include "PDBnode_t.h"

namespace pdb {

    // Forward declaration
    template<typename PDBNodeT>
    class pdb_t;

    // Class definition
    //
    // PDBs use nodes of any type provided that they satisfy the type constraint
    // pdb_type, e.g., npancakes
    template<typename T>
    requires pdb_type<T>
    class pdb_t<node_t<T>> {

    private:

        // INVARIANT: Because the number of items to store is known in advance
        // and a perfect hashing function is used, PDBs are implemented as a
        // vector of pdbval_t so that:
        //
        //    1. Non-empty positions contain their g*-value
        //    2. Empty entries are represented with pdbzero
        //    3. Non-existing positions are associated with the index
        //       string::npos
        std::vector<pdbval_t> _address;

        // The container also counts the number of items stored and the length
        // of the permutations considered
        int _n;
        size_t _size;

        // PDBs are created using a specific pattern. The relevant information
        // of a pattern is:
        //
        //    1. Goal state (_goal): consists of a vector of integers with all
        //       symbols in the goal state (which is the one used, after being
        //       masked, to generate the PDB). The goal has to be explicitly
        //       given, i.e., it can never be abstracted
        //
        //    2. Pattern (_pattern): consists of a string of symbols '-' and
        //       '*', so that the i-th symbol in _goal is preserved in the
        //       abstract state space if and only if pattern[i]='-' and
        //       abstracted away otherwise
        std::vector<int> _goal;
        std::string_view _pattern;

        // From the information of a pattern, the following information is
        // automatically derived:
        //
        //    1. Number of preserved symbols (_nbsymbols): This is computed
        //       automatically from the information given next
        //
        //    2. Operator masking (_omask): stores for every symbol preserved
        //       its location in the partial permutation that is used to
        //       automatically compute a rank
        int _nbsymbols;
        std::vector<int> _omask;

    public:

        // Default constructors are forbidden
        pdb_t () = delete;

        // Explicit constructor ---it is mandatory to provide the number of
        // items that will be stored in the PDB
        explicit pdb_t (const pdboff_t size) :
            _address { std::vector<pdbval_t>(size, pdbzero) },
            _size { 0 }
            {}

        // Copy and assignment constructors are explicitly forbidden
        pdb_t (const pdb_t&) = delete;
        pdb_t (pdb_t&&) = delete;

        // Assignment and move operators are disallowed as well
        pdb_t& operator=(const pdb_t&) = delete;
        pdb_t& operator=(pdb_t&&) = delete;

        // getters
        int get_n () const {
            return _n;
        }
        const std::vector<pdbval_t>& get_address () const {
            return _address;
        }

        // operator overloading

        // given a stable index, return the value stored at that location. In
        // case the index is incorrect, the behaviour is undefined
        const pdbval_t& operator[] (const pdboff_t index) const {
            return _address[index];
        }

        // given a stable index, return a reference to its location, so that it
        // can be re-written. In case the index is incorrect, an exception is
        // raised
        pdbval_t& operator[] (const pdboff_t index) {
            return _address.at (index);
        }

        // methods

        // return the size of the address space required to store all
        // permutations that result from abstracting n symbols with the given
        // pattern, where n is the length of the pattern, and the number of
        // symbols being preserved is given in pattern with the character '-'
        // ---and '*' representing that a particular symbol is abstracted away.
        static pdb::pdboff_t address_space (const std::string_view pattern) {

            // first, compute the number of non-abstracted symbols
            int nbsymbols = 0;
            for (auto i = 0 ; i < int (pattern.size ()) ; i++) {

                // If this symbol is preserved, update the number of symbols
                if (pattern[i] == '-') {
                    nbsymbols++;
                }
            }

            // Given n different symbols, k being preserved, the size of the address
            // space is n!/(n-k)!
            int n = pattern.size ();
            pdb::pdboff_t card = 1L;
            for (auto i = n ; i > n - nbsymbols ; i--) {
                card *= i;
            }
            return card;
        }

        // It is mandatory to init a pdb before doing any other operation with
        // it ---in particular, ranking operations. PDBs are initialized with
        // the symbols in a goal and a pattern which specifies which ones are
        // preserved, and which ones are abstracted away.
        //
        // It initializes:
        //
        //    1. _n: the length of the permutations to consider,
        //    2. _nbsymbols: the number of symbols used in the abstract state
        //                   space, and also
        //    3. _omask: the mapping between symbols and their location in the
        //               partial permutation used to rank abstract states
        //
        // It also makes a copy of its arguments:
        //
        //    1. _goal: keeps a copy of a explicit representation of the goal
        //              state, i.e., the goal state must not be given as an
        //              abstract state
        //    2. _pattern: keeps a copy of the given pattern which is used for
        //                 masking permutations
        //
        //  Mind the difference between "masking" permutations and omasking
        //  perms. The former refers to a simple substitution which preserves
        //  those symbols not being abstracted away and writes NONPAT for those
        //  which are not preserved; the latter however, refers to the creation
        //  of a (full/partial) permutation which represents the contents of a
        //  state in a way that can be ranked. "Masking" is implemented in
        //  pdb_t::mask, whereas "omasking" is used in pdb_t::rank
        void init (const std::vector<int> goal, const std::string_view pattern) {

            // first, verify that both the goal and the pattern contain the same
            // number of items. Otherwise, raise a fatal exception immediately
            if (goal.size () != pattern.size ()) {
                std::cerr << " Size of the goal: " << goal.size () << std::endl;
                std::cerr << "\t";
                for (auto i = 0 ; i < int (goal.size ()); i++) {
                    std::cerr << goal[i] << " ";
                }
                std::cerr << std::endl;
                std::cerr << " Size of the pattern: " << pattern.size () << std::endl;
                std::cerr << "\t";
                for (auto i = 0 ; i < int (pattern.size ()); i++) {
                    std::cerr << pattern[i] << " ";
                }
                std::cerr << std::endl;
                throw std::invalid_argument (" [init] The goal and the pattern have different size!");
            }

            // initialize the length of the permutations to consider, and also
            // make a copy of the goal and the pattern
            _n = goal.size ();
            _goal = goal;
            _pattern = pattern;

            // compute the number of non-abstracted symbols
            _nbsymbols = 0;
            for (auto i = 0 ; i < int (pattern.size ()) ; i++) {

                // If this symbol is preserved, update the number of symbols
                if (pattern[i] == '-') {
                    _nbsymbols++;
                }
            }

            // compute also the mask that maps preserved symbols to locations in
            // the partial permutation used to rank abstract states. Symbols are
            // given in the goal definition, so that omask has to contain as
            // many entries as the largest symbol in goal
            auto max_symb = std::max_element (goal.begin (), goal.end ());

            // Because all symbols in goal are assumed to be positive numbers,
            // the size of the mask is 1 + the maximum symbol in the goal. The
            // contents are initialized to -1, i.e., not being preserved so that
            // they point nowhere
            _omask = std::vector<int> (1 + *max_symb, -1);

            // Finally, compute the map from symbols to locations
            int j = 0;
            for (auto i = 0 ; i < int (pattern.size ()) ; i++) {
                if (pattern[i] == '-') {

                    // the pattern is defined wrt the goal definition. Note that
                    // preserved symbols are pushed to the back of the partial
                    // permutation. j counts how many symbols have been already
                    // pushed to the end and it is used to compute the location
                    // of the next symbol in the permutations to rank.
                    _omask[goal[i]] = int (pattern.size ()) - _nbsymbols + j;
                    j++;
                } else if (pattern[i] == '*') {

                    // then this symbol is ignored in the abstract state space
                    _omask[goal[i]] = -1;
                } else {
                    throw std::invalid_argument (" [init] Patterns can be defined only with '-' and '*'");
                }
            }
        }

        // insert an item given within a node into the PDB and return a stable
        // index to it. Inserting an item means just writing down its g*-value
        pdboff_t insert (const node_t<T>& item) {

            // Compute the position where this item should be stored. Note that
            // the state underlying the definition of the node must provide
            // "get_perm" which returns a vector of integers used to rank the
            // permutation as required by the type constraint pdb_type
            pdboff_t index = rank (item.get_state ().get_perm ());

            // Insert this item at the right location (just simply by storing
            // its g-value, which is a service provided by node_t) and
            // increment the count of the number of alive items in the PDB
            _address[index] = item.get_g ();
            _size++;

            // and return the location of this item into the vector
            return index;
        }

        // given a stable index, return the value stored at that location. In
        // case the index is incorrect, an exception is raised
        pdbval_t at (const pdboff_t index) {

            // Bound check
            if (index >= _address.size ()) {
                throw std::runtime_error (" [pdb_t] Access out of bounds");
            }
            return _address[index];
        }

        // return a stable index to the item given (as a node_t) in case it is
        // found in the PDB. Otherwise, return string::npos
        pdboff_t find (const node_t<T>& item) const {

            // get the location where the item should be found
            pdboff_t index = rank (item.get_state ().get_perm ());

            // in case this location has not been written yet, return
            // string::npos
            if (_address[index] == pdbzero) {
                return std::string::npos;
            }

            // Otherwise, return a stable index to its location
            return index;
        }

        // masking simply substitutes abstracted away symbols by NONPAT
        // according to the pattern stored in this instance while preserving the
        // rest. The following procedure masks both full and partial
        // permutations, i.e., it accepts NONPAT in perm. In case any symbol in
        // perm is NONPAT it is copied to the output as NONPAT as well in spite
        // of the pattern
        std::vector<int> mask (const std::vector<int>& perm) {

            // first of all, verify the given permutation has the same size used
            // to initialize this pdb
            if (_n != perm.size ()) {
                throw std::invalid_argument (" [mask] The permutation has not the length used in the initialization of this PDB");
            }

            // sort the pattern according to its symbols
            auto max_symb = std::max_element (_goal.begin (), _goal.end ());
            auto smask = std::vector<int> (1 + *max_symb, int (pdbzero));

            for (auto i = 0 ; i < int (_goal.size ()) ; i++) {

                // in case this position is abstracted away, substitute it by
                // NONPAT and preserve it otherwise
                smask[_goal[i]] = (_pattern[i] == '*') ? int (NONPAT) : _goal[i];
            }

            // from this mask return the corresponding masked permutation
            std::vector<int> result;
            for (auto i = 0 ; i < int (perm.size ()) ; i++) {
                result.push_back ( (perm [i] == NONPAT) ? int (NONPAT) : smask[perm[i]] );
            }

            return result;
        }

        // use the iterative implementation of Myrvold&Ruskey ranking function
        // to compute the ranking of the given permutation, which can be either
        // a full or partial permutation i.e., either representing a state in
        // the real state space or an abstracted state. In case a partial
        // permutation is given, abstracted away symbols should be represented
        // with the constant NONPAT
        //
        // The value returned is used to index instances of T in the PDB
        pdboff_t rank (const std::vector<int>& perm) const {

            // first of all, verify the given permutation has the same size used
            // to initialize this pdb
            if (_n != perm.size ()) {
                throw std::invalid_argument (" [rank] The permutation has not the length used in the initialization of this PDB");
            }

            // initialize the rank of the permutation to 0 and also the series of
            // factors to use
            pdboff_t r = 0L;
            pdboff_t f = 1L;

            // create the (partial) permutation to rank, and compute also its
            // inverse. Because the pattern is given in a partial permutation, all
            // non-abstracted symbols are pushed to the end of the permutation
            int nbsymbols = 0;
            std::vector<int> p (_n);
            std::vector<int> q (_n);
            for (auto i = 0 ; i < _n ; i++) {

                // add this content to p only if it is not abstracted. If an
                // abstract state has been given this is noted because the i-th
                // symbol might be NONPAT. In case a full permutation is given,
                // this case is detected because _omask is -1.
                //
                // WARNING - delivering a permutation perm which is not
                // compatible with _omask would produced undefined behaviour
                if (perm[i] != pdb::NONPAT) {
                    if (_omask[perm[i]] >= 0) {

                        // push it to the end of the partial permutation and store its
                        // location in the inverse permutation
                        p[_omask[perm[i]]] = i;
                        q[i] = _omask[perm[i]];

                        // and increment the number of symbols being computed
                        nbsymbols++;
                    }
                }
            }

            // ensure that the number of symbols produced and the number of
            // symbols considered in the initialization of this PDB are the same
            if (nbsymbols != _nbsymbols) {
                throw std::runtime_error (" [rank] nbsymbols != _nbsymbols");
            }

            // compute the rank
            int n = _n;
            int s, w;
            while (n > _n - _nbsymbols) {

                // take the last element from the permutation and swap n-1 and
                // q[n-1] in p
                s = p[n-1];
                w = p[n-1]; p[n-1] = p[q[n-1]]; p[q[n-1]] = w;

                // next, swap s and n-1 in q
                w = q[s]; q[s] = q[n-1]; q[n-1]=w;

                // update the ranking
                r += s*f; f *= n;

                // and decrement the count of symbols to compute
                n--;
            }
            return r;
        }

        // return the number of available positions in the PDB
        size_t capacity () const {
            return _address.capacity ();
        }

        // return the number of elements written into the PDB. Note this value
        // refers to the number of times insert was used because the operator[]
        // can also be used to write data but it does not update the data member
        // _size
        size_t size () const
            { return _size; }

    }; // class pdb_t<node_t<T>>

} // namespace pdb

#endif // _PDBPDB_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
