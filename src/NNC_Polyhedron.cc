/* NNC_Polyhedron class implementation (non-inline functions).
   Copyright (C) 2001, 2002 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include <config.h>

#include "NNC_Polyhedron.defs.hh"
#include "Polyhedron.defs.hh"

namespace PPL = Parma_Polyhedra_Library;

PPL::NNC_Polyhedron::NNC_Polyhedron(const Polyhedron& y)
  : PolyBase(NOT_NECESSARILY_CLOSED, y.space_dimension(), UNIVERSE) {
  ConSys cs = y.constraints();
  add_constraints(cs);
  assert(OK(false));
}

void
PPL::NNC_Polyhedron::limited_widening_assign(const NNC_Polyhedron& y,
					     ConSys& cs) {
  assert(OK(false));
  NNC_Polyhedron& x = *this;
  // KLUDGE.
  // Converting x and y to Polyhedron. 
  Polyhedron nc_x = Polyhedron(x);
  Polyhedron nc_y = Polyhedron(y);
  nc_x.limited_widening_assign(nc_y, cs);
  // Converting back and assigning to x.
  x = NNC_Polyhedron(nc_x);
}

bool
PPL::NNC_Polyhedron::is_topologically_closed() const {

  // Any empty or zero-dim polyhedron is closed.
  if (is_empty() || space_dimension() == 0)
    return true;

  if (constraints_are_minimized())
    // A polyhedron is closed iff
    // it has no (non-redundant) strict inequalities.
    return !con_sys.has_strict_inequalities();

  if (generators_are_minimized()) {
    // A polyhedron is closed iff all of its (non-redundant)
    // closure points are matched by a corresponding point.
    obtain_sorted_generators();
    size_t n_rows = gen_sys.num_rows();
    size_t eps_index = gen_sys.num_columns() - 1;
    for (size_t i = n_rows; i-- > 0; ) {
      const Generator& gi = gen_sys[i];
      if (gi.is_closure_point()) {
	bool gi_has_a_matching_point = false;
	// Since `gen_sys' is sorted, matching point must have
	// an index `j' greater than `i'.
	for (size_t j = i + 1; j < n_rows; ++j) {
	  const Generator& gj = gen_sys[j];
	  if (gj[eps_index] > 0) {
	    bool gj_matches_gi = true;
	    for (size_t k = eps_index; k-- > 0; )
	      if (gj[k] != gi[k]) {
		gj_matches_gi = false;
		break;
	      }
	    if (gj_matches_gi) {
	      gi_has_a_matching_point = true;
	      break;
	    }
	  }
	}
	if (!gi_has_a_matching_point)
	  return false;
      }
    }
    // All closure points are matched.
    return true;
  }

  // A polyhedron is closed iff
  // it has no (non-redundant) strict inequalities.
  minimize();
  return is_empty() || !con_sys.has_strict_inequalities();
}
