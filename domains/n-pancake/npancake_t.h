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

    // Consider an instance of the heavy-cost variant of the N-Pancake where the
    // cost of a reversal is equal to the radius of the first disc below the
    // spatula. If a reversal is practiced in an abstract state where such disc
    // has been abstracted away it would not be possible to know the cost of the
    // operator.
    //
    // In order to be able to generate meaningful information both in unit and
    // arbitrary-cost domains, the default cost is defined as the cost of an
    // operator involving an abstracted symbol, by default 1. It can be given in
    // the init procedure
    static pdb::pdbval_t _default_cost;

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
    static int get_default_cost () {
        return _default_cost;
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

    // get the contents of the i-th location. In case i is out of bounds an
    // exception is raised
    int operator[](int i) const {
        return _perm.at (i);
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
    // compute the descendants of any state and, in case the heavy-variant has
    // been selected, it is mandatory to provide the default cost
    static void init (const npancake_variant variant = npancake_variant::unit,
                      const pdb::pdbval_t default_cost=1) {

        // copy the domain variant
        npancake_t::_variant = variant;

        // and set the default cost to be used in the heavy-variant
        _default_cost = default_cost;
    }

    // return the children of this state as a vector of tuples with two
    // elements: first, the g-value of each node, and then the node itself.
    // Because this implementation honors both real and abstract states, the
    // cost of an operator is defined as follows:
    //
    //    1. Unit variant: it is always equal to 1, either with real or abstract
    //                     states
    //    2. Heavy-cost variant:
    //          a. Real states:    it is equal to the radius of the first disc
    //                             immediately below the spatula, ie., the
    //                             radius of the first disc not being transposed
    //          b. Abstract state: if the first disc below the spatula has not
    //                             been abstracted away, then it is its radius;
    //                             otherwise, the default cost is used
    void children (std::vector<std::tuple<pdb::pdbval_t, npancake_t>>& successors) {

        // for all locations
        for (auto i=1; i < _n; i++) {

            // compute the cost of this operator
            pdb::pdbval_t g = 1;
            if (_variant == npancake_variant::heavy_cost) {
                if (i==_n-1) {

                    // The table is never abstracted!
                    g = 1+_n;
                } else if (_perm[1+i]!=pdb::NONPAT) {

                    // use the radius of the first disc immediately below the
                    // spatula
                    g = _perm[1+i];
                } else {

                    // the disc immediately below the spatula is unknown, use
                    // the default cost
                    g = _default_cost;
                }
            }

            // Add this successor to the vector of successors along with its
            // cost
            successors.push_back (std::tuple<pdb::pdbval_t, npancake_t>{
                    g,
                    npancake_t (_flip (i))});
        }
    }

}; // class npancake_t

#endif // _NPANCAKE_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
