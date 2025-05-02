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
#include<iostream>
#include<iterator>
#include<string>
#include<vector>

// Class definition
//
// Defintion of a state of the N-pancake
class npancake_t {

private:

    // INVARIANT: a state in the N pancake is characterized by its length n and
    // a permutation of ints in the range [1, n]
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
    static std::string _variant;

    // In case an inconsistent heuristic is used, it is the max between the gap
    // heuristic of the permutation and the gap heuristic of the dual
    // permutation. When computing the heuristic value all that is known is the
    // heuristic estimate of the parent, and that is not sufficient to
    // incrementally compute the gap heuristic of the permutation ---which is
    // the only one that can be computed incrementally because the gap heuristic
    // of the dual has to be computed from scratch for every successor. Hence,
    // and additional data field is used to store separately the gap heuristic
    // of the permutation. Because it is updated in the h function and
    // libksearch requires h to be marked as constant, it is declared mutable
    std::vector<int> _invperm;                               // inv permutation

    // methods

    // flip the first k positions of this permutation. It does not affect the
    // inverse permutation
    std::vector<int> _flip (int k) {

        std::vector<int> perm = _perm;
        for (auto i=0 ; i<=k/2 ; i++) {
            std::swap (perm [i], perm [k-i]);
        }
        return perm;
    }

    // return the inverse of this permutation
    const std::vector<int> _inverse (std::vector<int>& perm) const {
        std::vector<int> invperm = std::vector<int>(perm.size (), 0);

        // note that all symbols in the inverse permutation should be also in
        // the range [1, _n]
        for (auto i = 0 ; i < _n ; i++) {
            invperm[perm[i]-1] = 1+i;
        }
        return invperm;
    }

public:

    // Default constructors are forbidden by default
    npancake_t () = delete;

    // A permutation can be constructed from a vector of integers. This
    // constructor assumes that all integers are distinct and belong to the
    // range [1, n]. It automatically computes the inverse of the given
    // permutation
    npancake_t (std::vector<int> perm) :
        _perm       { perm },
        _invperm    { std::vector<int> (perm.size (), 0) }
        {
            // store the size of the permuation
            _n = perm.size ();

            // and compute the inverse of this permutation
            _invperm = _inverse (perm);
        }

    // And also with an initializer list. It automatically computes the inverse
    // of the given permutation
    npancake_t (std::initializer_list<int> perm) :
        _perm       { perm },
        _invperm    { std::vector<int> (perm.size (), 0) }
        {
            // store the size of the permuation
            _n = perm.size ();

            // and compute the inverse of this permutation
            _invperm = _inverse (_perm);
        }

    // getters
    static int get_n () {
        return _n;
    }
    const std::vector<int>& get_invperm () const {
        return _invperm;
    }
    const std::vector<int>& get_perm () const {
        return _perm;
    }
    static const std::string& get_variant () {
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
    const int operator[](int i) const {
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

    // Invoke this service before using any other services of the npancake_t
    static void init (const std::string& variant = "unit") {

        // copy the domain variant
        npancake_t::_variant = variant;
    }

    // return the children of this state
    void children (std::vector<npancake_t>& successors) {

        // for all locations
        for (auto i=1; i < _n; i++) {

            // add the successor that results after flipping the first (i+1)
            // discs
            successors.push_back (npancake_t (_flip (i)));
        }
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
