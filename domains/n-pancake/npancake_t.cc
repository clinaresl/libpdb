// -*- coding: utf-8 -*-
// npancake_t.cc
// -----------------------------------------------------------------------------
//
// Started on <sáb 26-02-2022 18:10:41.081648137 (1645895441)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>   Ian Herman <iankherman@gmail.com>

//
// Definition of a state of the N-pancake
//

#include "npancake_t.h"

using namespace std;

// Static vars
int npancake_t::_n = 0;
pdb::pdbval_t npancake_t::_default_cost = 1;
npancake_variant npancake_t::_variant = npancake_variant::unit;


// Local Variables:
// mode:cpp
// fill-column:80
// End:
