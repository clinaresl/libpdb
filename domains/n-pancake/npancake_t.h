// -*- coding: utf-8 -*-
// npancake_t.h
// -----------------------------------------------------------------------------
//
// Started on <sáb 26-02-2022 18:07:39.045877735 (1645895259)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>   Ian Herman <iankherman@gmail.com>

//
// Definition of a state (either real or abstracted) of the N-pancake
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
    // length n and a permutation of ints. In case the permutation represents a
    // state in the real state space, only symbols in the range [1, n] are used;
    // in case an abstracted state is specified in the permutation, then those
    // contents which are abstracted should be given with the constant
    // pdb::NONPAT
    static int _n;                                 // length of the permutation
    std::vector<int> _perm;                                      // permutation

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
    // pdb::NONPAT
    npancake_t (const std::vector<int>& perm) :
        _perm { perm }
        {
            _n = perm.size ();
        }

    // And also with an initializer list
    npancake_t (std::initializer_list<int> perm) :
        _perm { perm }
        {
            _n = perm.size ();
        }

    // getters
    static int get_n () {
        return _n;
    }
    const std::vector<int>& get_perm () const {
        return _perm;
    }
    static npancake_variant get_variant () {
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

    // Invoke this service before using any other services of the npancake_t. It
    // sets the desired variant (unit by default) which is required to properly
    // compute the descendants of any state
    static void init (const npancake_variant variant = npancake_variant::unit) {

        // copy the domain variant
        npancake_t::_variant = variant;
    }

    // return the children of this state as a vector of tuples with two
    // elements: first, the g-value of each node, and then the node itself
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

}; // class npancake_t

#endif // _NPANCAKE_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
