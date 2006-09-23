/* YAP Prolog interface: system-dependent part.
   Copyright (C) 2001-2006 Roberto Bagnara <bagnara@cs.unipr.it>

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
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include "ppl.hh"
#include "pwl.hh"
#include <Yap/YapInterface.h>
#include <cassert>
#include <climits>

typedef YAP_Term Prolog_term_ref;
typedef YAP_Atom Prolog_atom;
typedef YAP_Bool Prolog_foreign_return_type;

namespace {

const Prolog_foreign_return_type PROLOG_SUCCESS = TRUE;
const Prolog_foreign_return_type PROLOG_FAILURE = FALSE;

} // namespace

#include "../exceptions.hh"

namespace PPL = Parma_Polyhedra_Library;

namespace {

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
  Temporary used to communicate big integers between C++ and Prolog.
*/
mpz_class tmp_mpz_class;

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

/*!
  Return a new term reference.
*/
inline Prolog_term_ref
Prolog_new_term_ref() {
  return 0;
}

/*!
  Make \p t be a reference to the same term referenced by \p u,
  i.e., assign \p u to \p t.
*/
inline int
Prolog_put_term(Prolog_term_ref& t, Prolog_term_ref u) {
  t = u;
  return 1;
}

/*!
  Assign to \p t a Prolog integer with value \p l.
*/
inline int
Prolog_put_long(Prolog_term_ref& t, long l) {
  t = YAP_MkIntTerm(l);
  return 1;
}

/*!
  Assign to \p t a Prolog integer with value \p ul.
*/
inline int
Prolog_put_ulong(Prolog_term_ref& t, unsigned long ul) {
  if (ul <= LONG_MAX)
    t = YAP_MkIntTerm(ul);
  else {
    tmp_mpz_class = ul;
    t = YAP_MkBigNumTerm(tmp_mpz_class.get_mpz_t());
  }
  return 1;
}

/*!
  Assign to \p t an atom whose name is given
  by the null-terminated string \p s.
*/
inline int
Prolog_put_atom_chars(Prolog_term_ref& t, const char* s) {
  t = YAP_MkAtomTerm(YAP_FullLookupAtom(s));
  return 1;
}

/*!
  Assign to \p t the Prolog atom \p a.
*/
inline int
Prolog_put_atom(Prolog_term_ref& t, Prolog_atom a) {
  t = YAP_MkAtomTerm(a);
  return 1;
}

/*!
  Assign to \p t a term representing the address contained in \p p.
*/
inline int
Prolog_put_address(Prolog_term_ref& t, void* p) {
  t = YAP_MkIntTerm(reinterpret_cast<long>(p));
  return 1;
}

/*!
  Return an atom whose name is given by the null-terminated string \p s.
*/
Prolog_atom
Prolog_atom_from_string(const char* s) {
  return YAP_FullLookupAtom(s);
}

Prolog_term_ref args[4];

/*!
  Assign to \p t a compound term whose principal functor is \p f
  of arity 1 with argument \p a1.
*/
inline int
Prolog_construct_compound(Prolog_term_ref& t, Prolog_atom f,
			  Prolog_term_ref a1) {
  args[0] = a1;
  t = YAP_MkApplTerm(YAP_MkFunctor(f, 1), 1, args);
  return 1;
}

/*!
  Assign to \p t a compound term whose principal functor is \p f
  of arity 2 with arguments \p a1 and \p a2.
*/
inline int
Prolog_construct_compound(Prolog_term_ref& t, Prolog_atom f,
			  Prolog_term_ref a1, Prolog_term_ref a2) {
  args[0] = a1;
  args[1] = a2;
  t = YAP_MkApplTerm(YAP_MkFunctor(f, 2), 2, args);
  return 1;
}

/*!
  Assign to \p t a compound term whose principal functor is \p f
  of arity 3 with arguments \p a1, \p a2 and \p a3.
*/
inline int
Prolog_construct_compound(Prolog_term_ref& t, Prolog_atom f,
			  Prolog_term_ref a1, Prolog_term_ref a2,
			  Prolog_term_ref a3) {
  args[0] = a1;
  args[1] = a2;
  args[2] = a3;
  t = YAP_MkApplTerm(YAP_MkFunctor(f, 3), 3, args);
  return 1;
}

/*!
  Assign to \p t a compound term whose principal functor is \p f
  of arity 4 with arguments \p a1, \p a2, \p a3 and \p a4.
*/
inline int
Prolog_construct_compound(Prolog_term_ref& t, Prolog_atom f,
			  Prolog_term_ref a1, Prolog_term_ref a2,
			  Prolog_term_ref a3, Prolog_term_ref a4) {
  args[0] = a1;
  args[1] = a2;
  args[2] = a3;
  args[3] = a4;
  t = YAP_MkApplTerm(YAP_MkFunctor(f, 4), 4, args);
  return 1;
}

/*!
  Assign to \p c a Prolog list whose head is \p h and tail is \p t.
*/
inline int
Prolog_construct_cons(Prolog_term_ref& c,
		      Prolog_term_ref h, Prolog_term_ref t) {
  c = YAP_MkPairTerm(h, t);
  return 1;
}

/*!
  Raise a Prolog exception with \p t as the exception term.
*/
inline void
Prolog_raise_exception(Prolog_term_ref t) {
  YAP_Throw(t);
}

/*!
  Return true if \p t is a Prolog variable, false otherwise.
*/
inline int
Prolog_is_variable(Prolog_term_ref t) {
  return YAP_IsVarTerm(t) != FALSE;
}

/*!
  Return true if \p t is a Prolog atom, false otherwise.
*/
inline int
Prolog_is_atom(Prolog_term_ref t) {
  return YAP_IsAtomTerm(t) != FALSE;
}

/*!
  Return true if \p t is a Prolog integer, false otherwise.
*/
inline int
Prolog_is_integer(Prolog_term_ref t) {
  return YAP_IsIntTerm(t) != FALSE || YAP_IsBigNumTerm(t) != FALSE;
}

/*!
  Return true if \p t is the representation of an address, false otherwise.
*/
inline int
Prolog_is_address(Prolog_term_ref t) {
  return YAP_IsIntTerm(t) != FALSE;
}

/*!
  Return true if \p t is a Prolog compound term, false otherwise.
*/
inline int
Prolog_is_compound(Prolog_term_ref t) {
  return YAP_IsApplTerm(t) != FALSE;
}

/*!
  Return true if \p t is a Prolog list, false otherwise.
*/
inline int
Prolog_is_cons(Prolog_term_ref t) {
  return YAP_IsPairTerm(t) != FALSE;
}

/*!
  Assuming \p t is a Prolog integer, return true if its value fits
  in a long, in which case the value is assigned to \p v,
  return false otherwise.  The behavior is undefined if \p t is
  not a Prolog integer.
*/
inline int
Prolog_get_long(Prolog_term_ref t, long* lp) {
  assert(Prolog_is_integer(t));
  if (YAP_IsBigNumTerm(t) != FALSE) {
    YAP_BigNumOfTerm(t, tmp_mpz_class.get_mpz_t());
    if (tmp_mpz_class >= LONG_MIN && tmp_mpz_class <= LONG_MAX)
      PPL::assign_r(*lp, tmp_mpz_class, PPL::ROUND_NOT_NEEDED);
    else
      return 0;
  }
  else
    *lp = YAP_IntOfTerm(t);
  return 1;
}

/*!
  If \p t is the Prolog representation for a memory address, return
  true and store that address into \p v; return false otherwise.
  The behavior is undefined if \p t is not an address.
*/
inline int
Prolog_get_address(Prolog_term_ref t, void** vpp) {
  assert(Prolog_is_address(t));
  *vpp = reinterpret_cast<void*>(YAP_IntOfTerm(t));
  return 1;
}

/*!
  If \p t is a Prolog atom, return true and store its name into \p name.
  The behavior is undefined if \p t is not a Prolog atom.
*/
inline int
Prolog_get_atom_name(Prolog_term_ref t, Prolog_atom* ap) {
  assert(Prolog_is_atom(t));
  *ap = YAP_AtomOfTerm(t);
  return 1;
}

/*!
  If \p t is a Prolog compound term, return true and store its name
  and arity into \p name and \p arity, respectively.
  The behavior is undefined if \p t is not a Prolog compound term.
*/
inline int
Prolog_get_compound_name_arity(Prolog_term_ref t, Prolog_atom* ap, int* ip) {
  assert(Prolog_is_compound(t));
  YAP_Functor f = YAP_FunctorOfTerm(t);
  *ap = YAP_NameOfFunctor(f);
  *ip = YAP_ArityOfFunctor(f);
  return 1;
}

/*!
  If \p t is a Prolog compound term and \p i is a positive integer
  less than or equal to its arity, return true and assign to \p a the
  i-th (principal) argument of \p t.
  The behavior is undefined if \p t is not a Prolog compound term.
*/
inline int
Prolog_get_arg(int i, Prolog_term_ref t, Prolog_term_ref& a) {
  assert(Prolog_is_compound(t));
  a = YAP_ArgOfTerm(i, t);
  return 1;
}

/*!
  If \p c is a Prolog cons (list constructor), assign its head and
  tail to \p h and \p t, respectively.
  The behavior is undefined if \p c is not a Prolog cons.
*/
inline int
Prolog_get_cons(Prolog_term_ref c, Prolog_term_ref& h, Prolog_term_ref& t) {
  assert(Prolog_is_cons(c));
  h = YAP_HeadOfTerm(c);
  t = YAP_TailOfTerm(c);
  return 1;
}

/*!
  Unify the terms referenced by \p t and \p u and return true
  if the unification is successful; return false otherwise.
*/
inline int
Prolog_unify(Prolog_term_ref t, Prolog_term_ref u) {
  return YAP_Unify(t, u) != FALSE;
}

PPL::Coefficient
integer_term_to_Coefficient(Prolog_term_ref t) {
  PPL::Coefficient n;
  if (YAP_IsBigNumTerm(t) != FALSE) {
    YAP_BigNumOfTerm(t, tmp_mpz_class.get_mpz_t());
    n = tmp_mpz_class;
  }
  else
    n = YAP_IntOfTerm(t);
  return n;
}

Prolog_term_ref
Coefficient_to_integer_term(const PPL::Coefficient& n) {
  if (n >= LONG_MIN && n <= LONG_MAX) {
    long l = 0;
    PPL::assign_r(l, n, PPL::ROUND_NOT_NEEDED);
    return YAP_MkIntTerm(l);
  }
  else {
    PPL::assign_r(tmp_mpz_class, n, PPL::ROUND_NOT_NEEDED);
    return YAP_MkBigNumTerm(tmp_mpz_class.get_mpz_t());
  }
}

} // namespace

#include "../ppl_prolog.icc"

#define YAP_STUB_0(name) \
extern "C" Prolog_foreign_return_type \
yap_stub_##name() { \
  return name(); \
}

#define YAP_STUB_1(name) \
extern "C" Prolog_foreign_return_type \
yap_stub_##name() { \
  Prolog_term_ref arg1 = YAP_ARG1; \
  return name(arg1); \
}

#define YAP_STUB_2(name) \
extern "C" Prolog_foreign_return_type \
yap_stub_##name() { \
  Prolog_term_ref arg1 = YAP_ARG1; \
  Prolog_term_ref arg2 = YAP_ARG2; \
  return name(arg1, arg2); \
}

#define YAP_STUB_3(name) \
extern "C" Prolog_foreign_return_type \
yap_stub_##name() { \
  Prolog_term_ref arg1 = YAP_ARG1; \
  Prolog_term_ref arg2 = YAP_ARG2; \
  Prolog_term_ref arg3 = YAP_ARG3; \
  return name(arg1, arg2, arg3); \
}

#define YAP_STUB_4(name) \
extern "C" Prolog_foreign_return_type \
yap_stub_##name() { \
  Prolog_term_ref arg1 = YAP_ARG1; \
  Prolog_term_ref arg2 = YAP_ARG2; \
  Prolog_term_ref arg3 = YAP_ARG3; \
  Prolog_term_ref arg4 = YAP_ARG4; \
  return name(arg1, arg2, arg3, arg4); \
}

#define YAP_STUB_5(name) \
extern "C" Prolog_foreign_return_type \
yap_stub_##name() { \
  Prolog_term_ref arg1 = YAP_ARG1; \
  Prolog_term_ref arg2 = YAP_ARG2; \
  Prolog_term_ref arg3 = YAP_ARG3; \
  Prolog_term_ref arg4 = YAP_ARG4; \
  Prolog_term_ref arg5 = YAP_ARG5; \
  return name(arg1, arg2, arg3, arg4, arg5); \
}

#define YAP_STUB_6(name) \
extern "C" Prolog_foreign_return_type \
yap_stub_##name() { \
  Prolog_term_ref arg1 = YAP_ARG1; \
  Prolog_term_ref arg2 = YAP_ARG2; \
  Prolog_term_ref arg3 = YAP_ARG3; \
  Prolog_term_ref arg4 = YAP_ARG4; \
  Prolog_term_ref arg5 = YAP_ARG5; \
  Prolog_term_ref arg6 = YAP_ARG6; \
  return name(arg1, arg2, arg3, arg4, arg5, arg6); \
}

YAP_STUB_1(ppl_version_major)
YAP_STUB_1(ppl_version_minor)
YAP_STUB_1(ppl_version_revision)
YAP_STUB_1(ppl_version_beta)
YAP_STUB_1(ppl_version)
YAP_STUB_1(ppl_banner)
YAP_STUB_1(ppl_max_space_dimension)
YAP_STUB_0(ppl_Coefficient_is_bounded)
YAP_STUB_1(ppl_Coefficient_max)
YAP_STUB_1(ppl_Coefficient_min)
YAP_STUB_0(ppl_initialize)
YAP_STUB_0(ppl_finalize)
YAP_STUB_1(ppl_set_timeout_exception_atom)
YAP_STUB_1(ppl_timeout_exception_atom)
YAP_STUB_1(ppl_set_timeout)
YAP_STUB_0(ppl_reset_timeout)
YAP_STUB_3(ppl_new_C_Polyhedron_from_space_dimension)
YAP_STUB_3(ppl_new_NNC_Polyhedron_from_space_dimension)
YAP_STUB_2(ppl_new_C_Polyhedron_from_C_Polyhedron)
YAP_STUB_2(ppl_new_C_Polyhedron_from_NNC_Polyhedron)
YAP_STUB_2(ppl_new_NNC_Polyhedron_from_C_Polyhedron)
YAP_STUB_2(ppl_new_NNC_Polyhedron_from_NNC_Polyhedron)
YAP_STUB_2(ppl_new_C_Polyhedron_from_constraints)
YAP_STUB_2(ppl_new_NNC_Polyhedron_from_constraints)
YAP_STUB_2(ppl_new_C_Polyhedron_from_generators)
YAP_STUB_2(ppl_new_NNC_Polyhedron_from_generators)
YAP_STUB_2(ppl_new_C_Polyhedron_from_bounding_box)
YAP_STUB_2(ppl_new_NNC_Polyhedron_from_bounding_box)
YAP_STUB_2(ppl_Polyhedron_swap)
YAP_STUB_1(ppl_delete_Polyhedron)
YAP_STUB_2(ppl_Polyhedron_space_dimension)
YAP_STUB_2(ppl_Polyhedron_affine_dimension)
YAP_STUB_2(ppl_Polyhedron_get_constraints)
YAP_STUB_2(ppl_Polyhedron_get_minimized_constraints)
YAP_STUB_2(ppl_Polyhedron_get_generators)
YAP_STUB_2(ppl_Polyhedron_get_minimized_generators)
YAP_STUB_3(ppl_Polyhedron_relation_with_constraint)
YAP_STUB_3(ppl_Polyhedron_relation_with_generator)
YAP_STUB_3(ppl_Polyhedron_get_bounding_box)
YAP_STUB_1(ppl_Polyhedron_is_empty)
YAP_STUB_1(ppl_Polyhedron_is_universe)
YAP_STUB_1(ppl_Polyhedron_is_bounded)
YAP_STUB_1(ppl_Polyhedron_contains_integer_point)
YAP_STUB_2(ppl_Polyhedron_bounds_from_above)
YAP_STUB_2(ppl_Polyhedron_bounds_from_below)
YAP_STUB_5(ppl_Polyhedron_maximize)
YAP_STUB_6(ppl_Polyhedron_maximize_with_point)
YAP_STUB_5(ppl_Polyhedron_minimize)
YAP_STUB_6(ppl_Polyhedron_minimize_with_point)
YAP_STUB_1(ppl_Polyhedron_is_topologically_closed)
YAP_STUB_2(ppl_Polyhedron_contains_Polyhedron)
YAP_STUB_2(ppl_Polyhedron_strictly_contains_Polyhedron)
YAP_STUB_2(ppl_Polyhedron_is_disjoint_from_Polyhedron)
YAP_STUB_2(ppl_Polyhedron_equals_Polyhedron)
YAP_STUB_1(ppl_Polyhedron_OK)
YAP_STUB_2(ppl_Polyhedron_add_constraint)
YAP_STUB_2(ppl_Polyhedron_add_constraint_and_minimize)
YAP_STUB_2(ppl_Polyhedron_add_generator)
YAP_STUB_2(ppl_Polyhedron_add_generator_and_minimize)
YAP_STUB_2(ppl_Polyhedron_add_constraints)
YAP_STUB_2(ppl_Polyhedron_add_constraints_and_minimize)
YAP_STUB_2(ppl_Polyhedron_add_generators)
YAP_STUB_2(ppl_Polyhedron_add_generators_and_minimize)
YAP_STUB_2(ppl_Polyhedron_intersection_assign)
YAP_STUB_2(ppl_Polyhedron_intersection_assign_and_minimize)
YAP_STUB_2(ppl_Polyhedron_poly_hull_assign)
YAP_STUB_2(ppl_Polyhedron_poly_hull_assign_and_minimize)
YAP_STUB_2(ppl_Polyhedron_poly_difference_assign)
YAP_STUB_4(ppl_Polyhedron_affine_image)
YAP_STUB_4(ppl_Polyhedron_affine_preimage)
YAP_STUB_5(ppl_Polyhedron_bounded_affine_image)
YAP_STUB_5(ppl_Polyhedron_bounded_affine_preimage)
YAP_STUB_5(ppl_Polyhedron_generalized_affine_image)
YAP_STUB_5(ppl_Polyhedron_generalized_affine_preimage)
YAP_STUB_4(ppl_Polyhedron_generalized_affine_image_lhs_rhs)
YAP_STUB_4(ppl_Polyhedron_generalized_affine_preimage_lhs_rhs)
YAP_STUB_2(ppl_Polyhedron_time_elapse_assign)
YAP_STUB_1(ppl_Polyhedron_topological_closure_assign)
YAP_STUB_4(ppl_Polyhedron_BHRZ03_widening_assign_with_tokens)
YAP_STUB_2(ppl_Polyhedron_BHRZ03_widening_assign)
YAP_STUB_5(ppl_Polyhedron_limited_BHRZ03_extrapolation_assign_with_tokens)
YAP_STUB_3(ppl_Polyhedron_limited_BHRZ03_extrapolation_assign)
YAP_STUB_5(ppl_Polyhedron_bounded_BHRZ03_extrapolation_assign_with_tokens)
YAP_STUB_3(ppl_Polyhedron_bounded_BHRZ03_extrapolation_assign)
YAP_STUB_4(ppl_Polyhedron_H79_widening_assign_with_tokens)
YAP_STUB_2(ppl_Polyhedron_H79_widening_assign)
YAP_STUB_5(ppl_Polyhedron_limited_H79_extrapolation_assign_with_tokens)
YAP_STUB_3(ppl_Polyhedron_limited_H79_extrapolation_assign)
YAP_STUB_5(ppl_Polyhedron_bounded_H79_extrapolation_assign_with_tokens)
YAP_STUB_3(ppl_Polyhedron_bounded_H79_extrapolation_assign)
YAP_STUB_2(ppl_Polyhedron_add_space_dimensions_and_project)
YAP_STUB_2(ppl_Polyhedron_add_space_dimensions_and_embed)
YAP_STUB_2(ppl_Polyhedron_concatenate_assign)
YAP_STUB_2(ppl_Polyhedron_remove_space_dimensions)
YAP_STUB_2(ppl_Polyhedron_remove_higher_space_dimensions)
YAP_STUB_3(ppl_Polyhedron_expand_space_dimension)
YAP_STUB_3(ppl_Polyhedron_fold_space_dimensions)
YAP_STUB_2(ppl_Polyhedron_map_space_dimensions)
YAP_STUB_2(ppl_new_LP_Problem_from_space_dimension)
YAP_STUB_5(ppl_new_LP_Problem)
YAP_STUB_2(ppl_new_LP_Problem_from_LP_Problem)
YAP_STUB_2(ppl_LP_Problem_swap)
YAP_STUB_1(ppl_delete_LP_Problem)
YAP_STUB_2(ppl_LP_Problem_space_dimension)
YAP_STUB_2(ppl_LP_Problem_constraints)
YAP_STUB_2(ppl_LP_Problem_objective_function)
YAP_STUB_2(ppl_LP_Problem_optimization_mode)
YAP_STUB_1(ppl_LP_Problem_clear)
YAP_STUB_2(ppl_LP_Problem_add_space_dimensions_and_embed)
YAP_STUB_2(ppl_LP_Problem_add_constraint)
YAP_STUB_2(ppl_LP_Problem_add_constraints)
YAP_STUB_2(ppl_LP_Problem_set_objective_function)
YAP_STUB_2(ppl_LP_Problem_set_optimization_mode)
YAP_STUB_1(ppl_LP_Problem_is_satisfiable)
YAP_STUB_2(ppl_LP_Problem_solve)
YAP_STUB_2(ppl_LP_Problem_feasible_point)
YAP_STUB_2(ppl_LP_Problem_optimizing_point)
YAP_STUB_3(ppl_LP_Problem_optimal_value)
YAP_STUB_4(ppl_LP_Problem_evaluate_objective_function)
YAP_STUB_1(ppl_LP_Problem_OK)

#define YAP_USER_C_PREDICATE(name, arity) \
 YAP_UserCPredicate(#name, reinterpret_cast<int(*)()>(yap_stub_##name), arity)

extern "C" void
init() {
  ppl_initialize();
  YAP_USER_C_PREDICATE(ppl_version_major, 1);
  YAP_USER_C_PREDICATE(ppl_version_minor, 1);
  YAP_USER_C_PREDICATE(ppl_version_revision, 1);
  YAP_USER_C_PREDICATE(ppl_version_beta, 1);
  YAP_USER_C_PREDICATE(ppl_version, 1);
  YAP_USER_C_PREDICATE(ppl_banner, 1);
  YAP_USER_C_PREDICATE(ppl_max_space_dimension, 1);
  YAP_USER_C_PREDICATE(ppl_Coefficient_is_bounded, 0);
  YAP_USER_C_PREDICATE(ppl_Coefficient_max, 1);
  YAP_USER_C_PREDICATE(ppl_Coefficient_min, 1);
  YAP_USER_C_PREDICATE(ppl_initialize, 0);
  YAP_USER_C_PREDICATE(ppl_finalize, 0);
  YAP_USER_C_PREDICATE(ppl_set_timeout_exception_atom, 1);
  YAP_USER_C_PREDICATE(ppl_timeout_exception_atom, 1);
  YAP_USER_C_PREDICATE(ppl_set_timeout, 1);
  YAP_USER_C_PREDICATE(ppl_reset_timeout, 0);
  YAP_USER_C_PREDICATE(ppl_new_C_Polyhedron_from_space_dimension, 3);
  YAP_USER_C_PREDICATE(ppl_new_NNC_Polyhedron_from_space_dimension, 3);
  YAP_USER_C_PREDICATE(ppl_new_C_Polyhedron_from_C_Polyhedron, 2);
  YAP_USER_C_PREDICATE(ppl_new_C_Polyhedron_from_NNC_Polyhedron, 2);
  YAP_USER_C_PREDICATE(ppl_new_NNC_Polyhedron_from_C_Polyhedron, 2);
  YAP_USER_C_PREDICATE(ppl_new_NNC_Polyhedron_from_NNC_Polyhedron, 2);
  YAP_USER_C_PREDICATE(ppl_new_C_Polyhedron_from_constraints, 2);
  YAP_USER_C_PREDICATE(ppl_new_NNC_Polyhedron_from_constraints, 2);
  YAP_USER_C_PREDICATE(ppl_new_C_Polyhedron_from_generators, 2);
  YAP_USER_C_PREDICATE(ppl_new_NNC_Polyhedron_from_generators, 2);
  YAP_USER_C_PREDICATE(ppl_new_C_Polyhedron_from_bounding_box, 2);
  YAP_USER_C_PREDICATE(ppl_new_NNC_Polyhedron_from_bounding_box, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_swap, 2);
  YAP_USER_C_PREDICATE(ppl_delete_Polyhedron, 1);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_space_dimension, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_affine_dimension, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_get_constraints, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_get_minimized_constraints, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_get_generators, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_get_minimized_generators, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_relation_with_constraint, 3);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_relation_with_generator, 3);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_get_bounding_box, 3);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_is_empty, 1);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_is_universe, 1);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_is_bounded, 1);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_contains_integer_point, 1);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_bounds_from_above, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_bounds_from_below, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_maximize, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_maximize_with_point, 6);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_minimize, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_minimize_with_point, 6);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_is_topologically_closed, 1);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_contains_Polyhedron, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_strictly_contains_Polyhedron, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_is_disjoint_from_Polyhedron, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_equals_Polyhedron, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_OK, 1);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_constraint, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_constraint_and_minimize, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_generator, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_generator_and_minimize, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_constraints, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_constraints_and_minimize, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_generators, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_generators_and_minimize, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_intersection_assign, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_intersection_assign_and_minimize, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_poly_hull_assign, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_poly_hull_assign_and_minimize, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_poly_difference_assign, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_affine_image, 4);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_affine_preimage, 4);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_bounded_affine_image, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_bounded_affine_preimage, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_generalized_affine_image, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_generalized_affine_preimage, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_generalized_affine_image_lhs_rhs, 4);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_generalized_affine_preimage_lhs_rhs, 4);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_time_elapse_assign, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_topological_closure_assign, 1);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_BHRZ03_widening_assign_with_tokens, 4);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_BHRZ03_widening_assign, 2);
  YAP_USER_C_PREDICATE(
          ppl_Polyhedron_limited_BHRZ03_extrapolation_assign_with_tokens, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_limited_BHRZ03_extrapolation_assign, 3);
  YAP_USER_C_PREDICATE(
          ppl_Polyhedron_bounded_BHRZ03_extrapolation_assign_with_tokens, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_bounded_BHRZ03_extrapolation_assign, 3);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_H79_widening_assign_with_tokens, 4);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_H79_widening_assign, 2);
  YAP_USER_C_PREDICATE(
          ppl_Polyhedron_limited_H79_extrapolation_assign_with_tokens, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_limited_H79_extrapolation_assign, 3);
  YAP_USER_C_PREDICATE(
          ppl_Polyhedron_bounded_H79_extrapolation_assign_with_tokens, 5);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_bounded_H79_extrapolation_assign, 3);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_space_dimensions_and_project, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_add_space_dimensions_and_embed, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_concatenate_assign, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_remove_space_dimensions, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_remove_higher_space_dimensions, 2);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_expand_space_dimension, 3);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_fold_space_dimensions, 3);
  YAP_USER_C_PREDICATE(ppl_Polyhedron_map_space_dimensions, 2);
  YAP_USER_C_PREDICATE(ppl_new_LP_Problem_from_space_dimension, 2);
  YAP_USER_C_PREDICATE(ppl_new_LP_Problem, 5);
  YAP_USER_C_PREDICATE(ppl_new_LP_Problem_from_LP_Problem, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_swap, 2);
  YAP_USER_C_PREDICATE(ppl_delete_LP_Problem, 1);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_space_dimension, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_constraints, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_objective_function, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_optimization_mode, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_clear, 1);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_add_space_dimensions_and_embed, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_add_constraint, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_add_constraints, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_set_objective_function, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_set_optimization_mode, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_is_satisfiable, 1);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_solve, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_feasible_point, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_optimizing_point, 2);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_optimal_value, 3);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_evaluate_objective_function, 4);
  YAP_USER_C_PREDICATE(ppl_LP_Problem_OK, 1);
}
