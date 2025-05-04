// -*- coding: utf-8 -*-
// npancake_t.h
// -----------------------------------------------------------------------------
//
// Started on <sáb 26-02-2022 18:07:39.045877735 (1645895259)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>   Ian Herman <iankherman@gmail.com>

//
// Definition of a state of the N-pancake
//

#ifndef _NPANCAKE_T_H_
#define _NPANCAKE_T_H_

#include<algorithm>
#include <cstdint>
#include<iostream>
#include<iterator>
#include<string>
#include<vector>

#include "../../src/PDBdefs.h"

// Definition of variants
enum class npancake_variant { unit, heavy_cost };

// Class definition
//
// Defintion of a state of the N-pancake
class npancake_t {

private:

    // INVARIANT: an abstract state in the N pancake is characterized by its
    // length n and a permutation of ints in the range [1, n] along with some
    // abstracted symbols represented with pdb::NONPAT
    static int _n;                                 // length of the permutation
    static int _nbsymbols;                   // number of non-abstracted symbols
    std::vector<int> _perm;                                      // permutation

    // to create the partial permutations representing a specific real state it
    // is necessary to map the contents of non-abstracted symbols into its
    // locations in the partial permutation
    static std::vector<int> _omask;

    // this implementation acknowledges different variants:
    //
    //    + unit: the cost of every operator is the same and equal to one
    //
    //    + heavy-cost: the cost of every operator is the disc id of the disc
    //    *below* the spatula, i.e., the first disc not being transposed. This
    //    is intentionally done to ensure that both the direct and inverse
    //    application of the same operator has the same cost, and also because
    //    it is possible to use a consistent heuristic for this optimization
    //    task. The usage of this variant is inspired from Hatem, M.; and Ruml,
    //    W. 2014. Bounded suboptimal search in linear space: New results. In
    //    Proceedings of SoCS-14.
    //
    static npancake_variant _variant;

    // methods

    // flip the first k positions of this permutation
    std::vector<int> _flip (int k) {

        std::vector<int> perm = _perm;
        for (auto i=0 ; i<=k/2 ; i++) {
            std::swap (perm [i], perm [k-i]);
        }
        return perm;
    }

public:

    // Default constructors are forbidden by default
    npancake_t () = delete;

    // A permutation can be constructed from a vector of integers. This
    // constructor assumes that all integers are distinct and belong to the
    // range [1, n] and a number of abstracted symbols represented with
    // pdb::NONPAT. It also takes care to update the assignment of locations of
    // non-abstracted symbols to locations in the partial permutation, _omask
    npancake_t (const std::vector<int> perm) :
        _perm { perm }
        {
            // verify the size of the permutation is equal to the length of the
            // pattern
            if (_n != int (perm.size ())) {
                throw std::invalid_argument ("The length of the permutation and the pattern are different!");
            }
        }

    // And also with an initializer list. It automatically computes the number
    // of non-abstracted symbols. It also takes care to update the assignment of
    // locations of non-abstracted symbols to locations in the partial
    // permutation, _omask
    npancake_t (std::initializer_list<int> perm) :
        _perm { perm }
        {
            // verify the size of the permutation is equal to the length of the
            // pattern
            if (_n != int (perm.size ())) {
                throw std::invalid_argument ("The length of the permutation and the pattern are different!");
            }
        }

    // getters
    static int get_n () {
        return _n;
    }
    static int get_nbsymbols () {
        return _nbsymbols;
    }
    const std::vector<int>& get_perm () const {
        return _perm;
    }
    static const npancake_variant get_variant () {
        return _variant;
    }

    // operator overloading

    // this instance is less than another if its permutation precedes it
    bool operator<(const npancake_t& right) const {
        return _perm < right.get_perm ();
    }

    // two instances are the same if they have the same permutation
    bool operator==(const npancake_t& right) const {
        return _perm == right.get_perm ();
    }

    // two instancse are not the same if they have different permutations
    bool operator!=(const npancake_t& right) const {
        return _perm != right.get_perm ();
    }

    // get the contents of the i-th location. In case i is out of bounds the
    // results are undefined
    int operator[](int i) const {
        return _perm[i];
    }

    friend std::ostream& operator<<(std::ostream& stream, const npancake_t& right) {

        // capture the permutation
        auto perm = right.get_perm ();

        // Show the permutation indicating the length of the side
        std::ostream_iterator<int> put (stream, " ");
        std::copy (perm.begin(), perm.end(), put);

        return stream;
    }

    // methods

    // return the size of the address space that results of using this pattern.
    // The address space is the interval [0, S) which corresponds to the ranking
    // of all feasible permutations
    static pdb::pdboff_t address_space () {

        // Given n different symbols, k being preserved, the size of the address
        // space is n!/(n-k)!
        pdb::pdboff_t card = 1L;
        for (auto i = _n ; i > _n-_nbsymbols ; i--) {
            card *= i;
        }
        return card;
    }

    // Invoke this service before using any other services of the npancake_t. It
    // sets the desired variant (unit by default) and, in addition it computes
    // the number of symbols (i.e., items not abstracted away) from the given
    // pattern and compute _omask, a vector which stores for every symbol its
    // location in the partial permutation used to compute the rank of any
    // permutation
    static void init (const std::string pattern,
                      const npancake_variant variant = npancake_variant::unit) {

        // copy the domain variant
        npancake_t::_variant = variant;

        // set the size of all permutations to be equal to the length of the
        // pattern
        _n = pattern.size ();

        // and compute the number of non-abstracted symbols
        _nbsymbols = 0;
        for (auto i = 0 ; i < _n ; i++) {

            // If this symbol is preserved, update the number of symbols
            if (pattern[i] == '-') {
                _nbsymbols++;
            }
        }

        // compute also the mask that maps symbols to locations in the final
        // partial permutation
        _omask = std::vector<int>(1+_n, -1);
        for (auto i = 0 ; i < _n ; i++) {
            if (pattern[i] == '-') {

                // the pattern is defined wrt the identity permutation with the
                // symbols [1, _n]. Thus, the i-th location refers to the symbol
                // (1+i)
                _omask[1+i] = _n-_nbsymbols + i;
            }
        }
    }

    // return the children of this state as a vector of tuples with two
    // elements: first, the g-value of each node, and then the node itself. The
    // inverse permutation of the children is automatically computed
    void children (std::vector<std::tuple<uint8_t, npancake_t>>& successors) {

        // for all locations
        for (auto i=1; i < _n; i++) {

            // Add this successor to the vector of successors along with its
            // cost
            successors.push_back (std::tuple<uint8_t, npancake_t>{
                    (_variant == npancake_variant::unit) ? 1 : ((i==_n-1) ? _n+1 : _perm[1+i]),
                    npancake_t (_flip (i))});
        }
    }

    // use the iterative implementation of Myrvold&Ruskey ranking function to
    // compute the ranking of the partial permutation of this instance. The
    // value returned is used to index instances of npancake_t in a Pattern
    // Database
    pdb::pdboff_t rank_pdb () const {

        int s, w;
        int n = _n;

        // initialize the rank of the permutation to 0 and also the series of
        // factors to use
        pdb::pdboff_t r = 0L;
        pdb::pdboff_t f = 1L;

        // create the (partial) permutation to rank, and compute also its
        // inverse. Because the pattern is given in a partial permutation, all
        // non-abstracted symbols are pushed to the end of the permutation
        std::vector<int> p (_n);
        std::vector<int> q (_n);
        for (auto i = 0 ; i < _n ; i++) {

            // in case this content is not abstracted
            if (_perm[i] != pdb::NONPAT) {

                // push it to the end of the partial permutation and store its
                // location in the inverse permutation
                p[_omask[_perm[i]]] = i;
                q[i] = _omask[_perm[i]];
            }
        }

        // compute the rank
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

}; // class npancake_t

namespace std {

    // Definition of a hash function for instances of the n-pancake. The
    // definition is included in the std namespace so that it can be used
    // directly by the functions implemented in that namespace
    template<>
    struct hash<::npancake_t> {

        // hash function
        size_t operator() (const npancake_t& right) const {

            // return the hash value of the vector of integers representing this
            // specific permutation
            // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
            size_t seed = right.get_perm ().size();
            for(auto& i : right.get_perm ()) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }

    }; // struct hash<npancake_t>
}

#endif // _NPANCAKE_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
