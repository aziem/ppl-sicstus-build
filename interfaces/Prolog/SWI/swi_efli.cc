/* SWI-Prolog extended foreign language interface: definitions.
   Copyright (C) 2001-2008 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include "swi_efli.hh"

namespace Parma_Polyhedra_Library {

namespace Prolog_Interfaces {

/*!
  True if and only if the Prolog engine supports unbounded integers.
*/
bool Prolog_has_unbounded_integers;

/*!
  If \p Prolog_has_unbounded_integers is false, holds the minimum
  integer value representable by a Prolog integer.
  Holds zero otherwise.
*/
long Prolog_min_integer;

/*!
  If \p Prolog_has_unbounded_integers is false, holds the maximum
  integer value representable by a Prolog integer.
  Holds zero otherwise.
*/
long Prolog_max_integer;

/*!
  Performs system-dependent initialization.
*/
void
ppl_Prolog_sysdep_init() {
  Prolog_has_unbounded_integers = true;
  Prolog_min_integer = 0;
  Prolog_max_integer = 0;
}

/*!
  Perform system-dependent de-itialization.
*/
void
ppl_Prolog_sysdep_deinit() {
}

int
Prolog_get_Coefficient(Prolog_term_ref t, Coefficient& n) {
  assert(Prolog_is_integer(t));
  DIRTY_TEMP0(mpz_class, tmp);
  PL_get_mpz(t, tmp.get_mpz_t());
  n = tmp;
  return 1;
}

int
Prolog_unify_Coefficient(Prolog_term_ref t, const Coefficient& n) {
  DIRTY_TEMP0(mpz_class, tmp);
  assign_r(tmp, n, ROUND_NOT_NEEDED);
  return PL_unify_mpz(t, tmp.get_mpz_t());
}

int
Prolog_put_Coefficient(Prolog_term_ref t, const Coefficient& n) {
  PL_put_variable(t);
  return Prolog_unify_Coefficient(t, n);
}

} // namespace Prolog_Interfaces

} // namespace Parma_Polyhedra_Library
