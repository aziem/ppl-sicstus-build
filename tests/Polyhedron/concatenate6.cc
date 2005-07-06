/* Test Polyhedron::concatenate_assign(): in this case the two
   polyhedra can have something pending.
   Copyright (C) 2001-2005 Roberto Bagnara <bagnara@cs.unipr.it>

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

#include "ppl_test.hh"

using namespace std;
using namespace Parma_Polyhedra_Library;

#ifndef NOISY
#define NOISY 0
#endif

namespace {

void
test1() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph1(1);
  ph1.generators();
  ph1.add_constraint(A >= 0);

  C_Polyhedron ph2(1);
  ph2.generators();
  ph2.add_constraint(A == 2);

#if NOISY
  print_constraints(ph1, "*** ph1 ***");
  print_constraints(ph2, "*** ph2 ***");
#endif

  ph1.concatenate_assign(ph2);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(B == 2);

  bool ok = (ph1 == known_result);

#if NOISY
  print_constraints(ph1, "*** After ph1.concatenate_assign(ph2) ***");
#endif

  if (!ok)
    exit(1);
}

void
test2() {
  Variable A(0);
  Variable B(1);

  C_Polyhedron ph1(1, EMPTY);
  ph1.add_generator(point());
  ph1.constraints();
  ph1.add_generator(ray(A));

  C_Polyhedron ph2(1, EMPTY);
  ph2.add_generator(point(2*A));
  ph2.constraints();

#if NOISY
  print_generators(ph1, "*** ph1 ***");
  print_generators(ph2, "*** ph2 ***");
#endif

  ph1.concatenate_assign(ph2);

  C_Polyhedron known_result(2);
  known_result.add_constraint(A >= 0);
  known_result.add_constraint(B == 2);

  bool ok = (ph1 == known_result);

#if NOISY
  print_generators(ph1, "*** After ph1.concatenate_assign(ph2) ***");
#endif

  if (!ok)
    exit(1);
}

} // namespace

int
main() TRY {
  set_handlers();

  test1();
  test2();

  return 0;
}
CATCH
