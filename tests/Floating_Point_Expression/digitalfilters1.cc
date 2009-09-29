/* Test Rate Limiter on differents abstract domains.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

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

#include "ppl_test.hh"

namespace {

/*
This file tests a rate limiter that, given random input flows of floating
point values X and D, bounded respectively by [-128, 128] and [0, 16],
computes an output flow Y that tries to follow X while having a change rate
limited by D. The pseudo-code of such rate limiter is the following:

input X, D;
output Y;
R = (-inf, +inf);
S = (-inf, +inf);
for (n = 0; n < N; ++n) {
  S = Y;
  R = X - S;
  Y = X;
  if (R <= -D)
    Y = S - D;
  if (R >= D)
    Y = S + D;
}
*/

// tests rate limiter using intervals abstract domain.
bool
test01() {
  Variable X(0); //input
  Variable D(1); //input
  Variable Y(2); //output
  Variable S(3); //last output
  Variable R(4); //actual rate
  FP_Interval_Abstract_Store abstract_store(5);
  FP_Interval tmp(-128);
  tmp.join_assign(128);
  abstract_store.set_interval(X, tmp);
  tmp.lower() = 0;
  tmp.upper() = 16;
  abstract_store.set_interval(D, tmp);

  nout << "*** for each n ***" << endl;

  //if (R <= -D) Y = S - D;
  FP_Interval_Abstract_Store as_then(abstract_store);
  as_then.refine_with_constraint(R <= -D);
  as_then.set_interval(Y, abstract_store.get_interval(S)
                        - abstract_store.get_interval(D));
  abstract_store.refine_with_constraint(R > -D);
  abstract_store.upper_bound_assign(as_then);

  //if (R >= D)  Y = S + D;
  as_then = abstract_store;
  as_then.refine_with_constraint(R >= D);
  as_then.set_interval(Y, abstract_store.get_interval(S)
                        + abstract_store.get_interval(D));
  abstract_store.refine_with_constraint(R > D);
  abstract_store.upper_bound_assign(as_then);

  tmp = abstract_store.get_interval(Y);
  nout << "Y in " << tmp << endl;
  return !tmp.is_bounded();
}

// tests rate limiter using bounded differences abstract domain.
bool
test02() {

  return true;
}

// tests rate limiter using octagons abstract domain.
bool
test03() {
  Variable X(0); //input
  Variable D(1); //input
  Variable Y(2); //output
  Variable S(3); //last output
  Variable R(4); //actual rate
  FP_Interval_Abstract_Store abstract_store(5);
  FP_Interval tmp(-128);
  tmp.join_assign(128);
  abstract_store.set_interval(X, tmp);
  tmp.lower() = 0;
  tmp.upper() = 16;
  abstract_store.set_interval(D, tmp);
  FP_Octagonal_Shape oc(abstract_store);
  FP_Linear_Form ld(D);

  nout << "*** n = 0 ***" << endl;

  //S = Y; R = X - S; Y = X;
  oc.affine_image(S, FP_Linear_Form(Y));
  oc.affine_image(R, FP_Linear_Form(X - S));
  oc.affine_image(Y, FP_Linear_Form(X));


  //if (R <= -D) Y = S - D;
  FP_Linear_Form lr(R);
  FP_Octagonal_Shape oc_then(oc);
  oc_then.refine_with_linear_form_inequality(lr, -ld);
  oc_then.affine_image(Y, FP_Linear_Form(S - D));
  oc.refine_with_linear_form_inequality(-ld, lr);
  oc.upper_bound_assign(oc_then);
  oc.refine_fp_interval_abstract_store(abstract_store);

  //if (R >= D)  Y = S + D;
  oc_then = oc;
  oc_then.refine_with_linear_form_inequality(ld, lr);
  oc_then.affine_image(Y, FP_Linear_Form(S + D));
  oc.refine_with_linear_form_inequality(lr, ld);
  oc.upper_bound_assign(oc_then);
  oc.refine_fp_interval_abstract_store(abstract_store);

  tmp = abstract_store.get_interval(Y);
  nout << "Y in " << tmp << endl;

  return (tmp.lower() == -136);
}

// tests rate limiter using polyhedra abstract domain.
bool
test04() {
  Variable X(0); //input
  Variable D(1); //input
  Variable Y(2); //output
  Variable S(3); //last output
  Variable R(4); //actual rate
  FP_Interval_Abstract_Store abstract_store(5);
  FP_Interval tmp(-128);
  tmp.join_assign(128);
  abstract_store.set_interval(X, tmp);
  tmp.lower() = 0;
  tmp.upper() = 16;
  abstract_store.set_interval(D, tmp);
  FP_Linear_Form ld(D);
  //FIXME: Dirty values, but abstract store must be bounded!
  tmp.lower() = -std::numeric_limits<ANALYZER_FP_FORMAT>::max();
  tmp.upper() =  std::numeric_limits<ANALYZER_FP_FORMAT>::max();
  abstract_store.set_interval(Y, tmp);
  abstract_store.set_interval(S, tmp);
  abstract_store.set_interval(R, tmp);
  C_Polyhedron ph(abstract_store);

  nout << "*** n = 0 ***" << endl;

  //S = Y; R = X - S; Y = X;
  ph.affine_image(S, FP_Linear_Form(Y), abstract_store);
  ph.affine_image(R, FP_Linear_Form(X - S), abstract_store);
  ph.affine_image(Y, FP_Linear_Form(X), abstract_store);

  //if (R <= -D) Y = S - D;
  FP_Linear_Form lr(R);
  C_Polyhedron ph_then(ph);
  FP_Interval_Abstract_Store as_then(abstract_store);
  ph_then.refine_with_linear_form_inequality(lr, -ld, as_then);
  ph_then.affine_image(Y, FP_Linear_Form(S - D), as_then);
  ph.generalized_refine_with_linear_form_inequality(
    -ld, lr, LESS_THAN, abstract_store);
  ph.upper_bound_assign(ph_then);
  abstract_store.upper_bound_assign(as_then);
  ph.refine_fp_interval_abstract_store(abstract_store);

  //if (R >= D)  Y = S + D;
  ph_then = ph;
  as_then = abstract_store;
  ph_then.refine_with_linear_form_inequality(ld, lr, as_then);
  ph_then.affine_image(Y, FP_Linear_Form(S + D), as_then);
  ph.generalized_refine_with_linear_form_inequality(
    lr, ld, LESS_THAN, abstract_store);
  ph.upper_bound_assign(ph_then);
  abstract_store.upper_bound_assign(as_then);
  ph.refine_fp_interval_abstract_store(abstract_store);

  tmp = abstract_store.get_interval(Y);
  nout << "Y in " << tmp << endl;
  return (tmp.lower() == -128);
}

// tests rate limiter using octagons abstract domain and
// FP_Linear_Form_Abstract_Store.
bool
test05() {
  Variable X(0); //input
  Variable D(1); //input
  Variable Y(2); //output
  Variable S(3); //last output
  Variable R(4); //actual rate
  Var_FP_Expression x(0);
  Var_FP_Expression d(1);
  Var_FP_Expression y(2);
  Var_FP_Expression s(3);
  Var_FP_Expression r(4);
  FP_Linear_Form lx(X);
  FP_Linear_Form ld(D);
  FP_Linear_Form ly(Y);
  FP_Linear_Form ls;
  FP_Linear_Form lr;
  FP_Interval_Abstract_Store abstract_store(5);
  FP_Linear_Form_Abstract_Store lf_abstract_store;
  FP_Interval tmp(-128);
  tmp.join_assign(128);
  abstract_store.set_interval(X, tmp);
  tmp.lower() = 0;
  tmp.upper() = 16;
  abstract_store.set_interval(D, tmp);
  FP_Octagonal_Shape oc(abstract_store);

  nout << "*** n = 0 ***" << endl;

  //S = Y; R = X - S; Y = X;
  oc.affine_image(S, ly);
  //assign_linear_form
  Var_FP_Expression* px = new Var_FP_Expression(0);
  Var_FP_Expression* ps = new Var_FP_Expression(3);
  Dif_FP_Expression x_dif_s(px, ps);
  x_dif_s.linearize(abstract_store, lf_abstract_store, lr);
  oc.affine_image(R, lr);
  //assign_linear_form
  oc.affine_image(Y, lx);
  //assign_linear_form

  //if (R <= -D) Y = S - D;
  FP_Octagonal_Shape oc_then(oc);
  oc_then.refine_with_linear_form_inequality(lr, -ld);
  Var_FP_Expression* ps1 = new Var_FP_Expression(3);
  Var_FP_Expression* pd  = new Var_FP_Expression(2);
  Dif_FP_Expression s1_dif_d(ps1, pd);
  s1_dif_d.linearize(abstract_store, lf_abstract_store, ly);
  oc_then.affine_image(Y, ly);
  //assign_linear_form
  oc.refine_with_linear_form_inequality(-ld, lr);
  oc.upper_bound_assign(oc_then);
  oc.refine_fp_interval_abstract_store(abstract_store);
  //lub on FP_Interval_abstract_store

  //if (R >= D)  Y = S + D;
  oc_then = oc;
  oc_then.refine_with_linear_form_inequality(ld, lr);
  Var_FP_Expression* ps2 = new Var_FP_Expression(3);
  Var_FP_Expression* pd1  = new Var_FP_Expression(2);
  Sum_FP_Expression s2_sum_d1(ps2, pd1);
  s2_sum_d1.linearize(abstract_store, lf_abstract_store, ly);
  oc_then.affine_image(Y, ly);
  //assign_linear_form
  oc.refine_with_linear_form_inequality(lr, ld);
  oc.upper_bound_assign(oc_then);
  oc.refine_fp_interval_abstract_store(abstract_store);
  //lub on FP_Interval_abstract_store

  tmp = abstract_store.get_interval(Y);
  nout << "Y in " << tmp << endl;
  return tmp.is_bounded();
}

} // namespace

BEGIN_MAIN
  DO_TEST(test01);
  DO_TEST(test02);
  DO_TEST(test03);
  DO_TEST(test04);
  DO_TEST(test05);
END_MAIN
