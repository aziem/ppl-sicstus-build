/* PIP_Tree related class implementation: non-inline functions.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>

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

#include <ppl-config.h>
#include "PIP_Tree.defs.hh"
#include "PIP_Problem.defs.hh"

#include <algorithm>

namespace Parma_Polyhedra_Library {

namespace {

// Calculate positive modulo of x % y
void
mod_assign(Coefficient& z,
           Coefficient_traits::const_reference x,
           Coefficient_traits::const_reference y) {
  z = x % y;
  if (z < 0)
    z += y;
}

// Compute x += c * y
void
add_assign(Row& x, const Row& y, Coefficient_traits::const_reference c) {
  PPL_ASSERT(x.size() == y.size());
  for (dimension_type i = x.size(); i-- > 0; )
    add_mul_assign(x[i], c, y[i]);
}

// Compute x -= y
void
sub_assign(Row& x, const Row& y) {
  PPL_ASSERT(x.size() == y.size());
  for (dimension_type i = x.size(); i-- > 0; )
    x[i] -= y[i];
}

// Merge constraint system to a Matrix-form context such as x = x U y
void
merge_assign(Matrix& x,
             const Constraint_System& y,
             const Variables_Set& parameters) {
  PPL_ASSERT(parameters.size() == x.num_columns() - 1);
  const dimension_type new_rows = std::distance(y.begin(), y.end());
  if (new_rows == 0)
    return;
  const dimension_type old_num_rows = x.num_rows();
  x.add_zero_rows(new_rows, Row::Flags());
  // Compute once for all.
  const Variables_Set::const_iterator param_begin = parameters.begin();
  const Variables_Set::const_iterator param_end = parameters.end();

  dimension_type i = old_num_rows;
  for (Constraint_System::const_iterator y_i = y.begin(),
         y_end = y.end(); y_i != y_end; ++y_i, ++i) {
    PPL_ASSERT(y_i->is_nonstrict_inequality());
    Row& x_i = x[i];
    x_i[0] = y_i->inhomogeneous_term();
    Variables_Set::const_iterator pj;
    dimension_type j = 1;
    for (pj = param_begin; pj != param_end; ++pj, ++j)
      x_i[j] = y_i->coefficient(Variable(*pj));
  }
}

// Tranform expression "expr" into "-expr-1", using scaling
void
negate_assign(Row& x, const Row& y, Coefficient_traits::const_reference sc) {
  PPL_ASSERT(x.size() == y.size());
  for (dimension_type i = x.size(); i-- > 0; )
    x[i] = -y[i];
  if (sc != 0) {
    PPL_DIRTY_TEMP_COEFFICIENT(mod);
    mod_assign(mod, x[0], sc);
    x[0] -= ((mod == 0) ? sc : mod);
  }
}

// Update given context matrix using local artificials
dimension_type
update_context(Matrix& context,
               const PIP_Tree_Node::Artificial_Parameter_Sequence& ap) {
  const dimension_type ap_size = ap.size();
  if (ap_size > 0)
    context.add_zero_columns(ap_size);
  return ap_size;
}

// Update given context matrix and parameter set using local artificials
void
update_context(Variables_Set& params, Matrix& context,
               const PIP_Tree_Node::Artificial_Parameter_Sequence& ap,
               dimension_type& space_dimension) {
  const dimension_type ap_size = update_context(context, ap);
  // Update parameters.
  for (dimension_type i = 0; i < ap_size; ++i)
    params.insert(space_dimension + i);
  // Update space dimension.
  space_dimension += ap_size;
}

/* Compares two columns lexicographically in revised simplex tableau
  - Returns true if (column ja)*(-cst_a)/pivot_a[ja]
                    << (column jb)*(-cst_b)/pivot_b[jb]
  - Returns false otherwise
*/
bool
column_lower(const Matrix& tableau,
             const std::vector<dimension_type>& mapping,
             const std::vector<bool>& basis,
             const Row& pivot_a,
             const dimension_type ja,
             const Row& pivot_b,
             const dimension_type jb,
             Coefficient_traits::const_reference cst_a = -1,
             Coefficient_traits::const_reference cst_b = -1) {
  const Coefficient& sij_a = pivot_a[ja];
  const Coefficient& sij_b = pivot_b[jb];
  PPL_ASSERT(sij_a > 0);
  PPL_ASSERT(sij_b > 0);

  PPL_DIRTY_TEMP_COEFFICIENT(lhs_coeff);
  PPL_DIRTY_TEMP_COEFFICIENT(rhs_coeff);
  lhs_coeff = cst_a * sij_b;
  rhs_coeff = cst_b * sij_a;

  if (ja == jb) {
    // Same column: just compare the ratios.
    // This works since all columns are lexico-positive.
    // return cst_a * sij_b > cst_b * sij_a;
    return lhs_coeff > rhs_coeff;
  }

  PPL_DIRTY_TEMP_COEFFICIENT(lhs);
  PPL_DIRTY_TEMP_COEFFICIENT(rhs);
  const dimension_type num_vars = mapping.size();
  dimension_type k = 0;
  // While loop guard is: (k < num_rows && lhs == rhs).
  // Return value is false, if k >= num_rows; lhs < rhs, otherwise.
  // Try to optimize the computation of lhs and rhs.
  while (true) {
    const dimension_type mk = mapping[k];
    const bool in_base = basis[k];
    if (++k >= num_vars)
      return false;
    if (in_base) {
      // Reconstitute the identity submatrix part of tableau.
      if (mk == ja) {
        // Optimizing for: lhs == lhs_coeff && rhs == 0;
        if (lhs_coeff == 0)
          continue;
        else
          return lhs_coeff > 0;
      }
      if (mk == jb) {
        // Optimizing for: lhs == 0 && rhs == rhs_coeff;
        if (rhs_coeff == 0)
          continue;
        else
          return 0 > rhs_coeff;
      }
      // Optimizing for: lhs == 0 && rhs == 0;
      continue;
    } else {
      // Not in base.
      const Row& t_mk = tableau[mk];
      lhs = lhs_coeff * t_mk[ja];
      rhs = rhs_coeff * t_mk[jb];
      if (lhs == rhs)
        continue;
      else
        return lhs > rhs;
    }
  }
  // This point should be unreachable.
  throw std::runtime_error("PPL internal error");
}

/* Find the column j in revised simplex tableau such as
  - pivot_row[j] is positive
  - (column j) / pivot_row[j] is lexico-minimal
*/
bool
find_lexico_minimum_column(const Matrix& tableau,
                           const std::vector<dimension_type>& mapping,
                           const std::vector<bool>& basis,
                           const Row& pivot_row,
                           const dimension_type start_j,
                           dimension_type& j_out) {
  const dimension_type num_cols = tableau.num_columns();
  bool has_positive_coefficient = false;

  j_out = num_cols;
  for (dimension_type j = start_j; j < num_cols; ++j) {
    const Coefficient& c = pivot_row[j];
    if (c <= 0)
      continue;
    has_positive_coefficient = true;
    if (j_out == num_cols
        || column_lower(tableau, mapping, basis,
                        pivot_row, j, pivot_row, j_out))
      j_out = j;
  }
  return has_positive_coefficient;
}

// Divide all coefficients in row x and denominator y by their GCD.
void
row_normalize(Row& x, Coefficient& den) {
  if (den == 1)
    return;
  const dimension_type x_size = x.size();
  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  gcd = den;
  for (dimension_type i = x_size; i-- > 0; ) {
    const Coefficient& x_i = x[i];
    if (x_i != 0) {
      gcd_assign(gcd, x_i, gcd);
      if (gcd == 1)
        return;
    }
  }
  // Divide the coefficients by the GCD.
  for (dimension_type i = x_size; i-- > 0; ) {
    Coefficient& x_i = x[i];
    exact_div_assign(x_i, x_i, gcd);
  }
  // Divide the denominator by the GCD.
  exact_div_assign(den, den, gcd);
}

} // namespace

namespace IO_Operators {

std::ostream&
operator<<(std::ostream& os, const PIP_Tree_Node::Artificial_Parameter& x) {
  const Linear_Expression& expr = static_cast<const Linear_Expression&>(x);
  os << "(" << expr << ") div " << x.get_denominator();
  return os;
}

} // namespace IO_Operators

PIP_Tree_Node::PIP_Tree_Node()
  : parent_(0),
    constraints_(),
    artificial_parameters() {
}

PIP_Tree_Node::PIP_Tree_Node(const PIP_Tree_Node& y)
  : parent_(0), // Parent is not copied.
    constraints_(y.constraints_),
    artificial_parameters(y.artificial_parameters) {
}

bool
operator==(const PIP_Tree_Node::Artificial_Parameter& x,
           const PIP_Tree_Node::Artificial_Parameter& y) {
  if (x.space_dimension() != y.space_dimension())
    return false;
  if (x.denominator != y.denominator)
    return false;
  if (x.inhomogeneous_term() != y.inhomogeneous_term())
    return false;
  for (dimension_type i = x.space_dimension(); i-- > 0; )
    if (x.coefficient(Variable(i)) != y.coefficient(Variable(i)))
      return false;
  return true;
}

void
PIP_Tree_Node::Artificial_Parameter::ascii_dump(std::ostream& s) const {
  s << "\ndenominator " << denominator << "\n";
  Linear_Expression::ascii_dump(s);
}

bool
PIP_Tree_Node::Artificial_Parameter::ascii_load(std::istream& s) {
  std::string str;
  if (!(s >> str) || str != "denominator")
    return false;
  if (!(s >> denominator))
    return false;
  if (!Linear_Expression::ascii_load(s))
    return false;
  PPL_ASSERT(OK());
  return true;
}

PIP_Decision_Node::~PIP_Decision_Node() {
  delete true_child;
  delete false_child;
}

PIP_Solution_Node::~PIP_Solution_Node() {
}

PIP_Solution_Node::PIP_Solution_Node()
  : PIP_Tree_Node(),
    tableau(),
    basis(),
    mapping(),
    var_row(),
    var_column(),
    special_equality_row(0),
    big_dimension(not_a_dimension()),
    sign(),
    solution(),
    solution_valid(false) {
}

PIP_Solution_Node::PIP_Solution_Node(const PIP_Solution_Node& y)
  : PIP_Tree_Node(y),
    tableau(y.tableau),
    basis(y.basis),
    mapping(y.mapping),
    var_row(y.var_row),
    var_column(y.var_column),
    special_equality_row(y.special_equality_row),
    big_dimension(y.big_dimension),
    sign(y.sign),
    solution(y.solution),
    solution_valid(y.solution_valid) {
}

PIP_Solution_Node::PIP_Solution_Node(const PIP_Solution_Node& y,
                                     bool empty_constraints)
  : PIP_Tree_Node(),
    tableau(y.tableau),
    basis(y.basis),
    mapping(y.mapping),
    var_row(y.var_row),
    var_column(y.var_column),
    special_equality_row(y.special_equality_row),
    big_dimension(y.big_dimension),
    sign(y.sign),
    solution(y.solution),
    solution_valid(y.solution_valid) {
  if (!empty_constraints) {
    constraints_ = y.constraints_;
    artificial_parameters = y.artificial_parameters;
  }
}

PIP_Decision_Node::PIP_Decision_Node(PIP_Tree_Node* fcp,
                                     PIP_Tree_Node* tcp)
  : PIP_Tree_Node(),
    true_child(tcp),
    false_child(fcp) {
  if (fcp != 0)
    fcp->set_parent(this);
  if (tcp != 0)
    tcp->set_parent(this);
}

PIP_Decision_Node ::PIP_Decision_Node(const PIP_Decision_Node& y)
  : PIP_Tree_Node(y),
    true_child(0),
    false_child(0) {
  if (y.true_child != 0) {
    true_child = y.true_child->clone();
    true_child->set_parent(this);
  }
  if (y.false_child != 0) {
    false_child = y.false_child->clone();
    false_child->set_parent(this);
  }
}

const PIP_Solution_Node*
PIP_Tree_Node::as_solution() const {
  return 0;
}

PIP_Solution_Node*
PIP_Tree_Node::as_solution() {
  return 0;
}

const PIP_Decision_Node*
PIP_Tree_Node::as_decision() const {
  return 0;
}

PIP_Decision_Node*
PIP_Tree_Node::as_decision() {
  return 0;
}

const PIP_Solution_Node*
PIP_Solution_Node::as_solution() const {
  return this;
}

PIP_Solution_Node*
PIP_Solution_Node::as_solution() {
  return this;
}

const PIP_Decision_Node*
PIP_Decision_Node::as_decision() const {
  return this;
}

PIP_Decision_Node*
PIP_Decision_Node::as_decision() {
  return this;
}

dimension_type
PIP_Tree_Node::insert_artificials(Variables_Set& params,
                                  const dimension_type space_dimension) const {
  const dimension_type ap_size = artificial_parameters.size();
  PPL_ASSERT(space_dimension >= ap_size);
  dimension_type sd = space_dimension - ap_size;
  const dimension_type parent_size
    = (parent_ == 0) ? 0 : parent_->insert_artificials(params, sd);
  if (ap_size > 0) {
    for (dimension_type i = 0; i < ap_size; ++i)
      params.insert(sd++);
  }
  return parent_size + ap_size;
}

bool
PIP_Solution_Node::Tableau::OK() const {
  if (s.num_rows() != t.num_rows()) {
#ifndef NDEBUG
    std::cerr << "PIP_Solution_Node::Tableau matrices "
              << "have a different number of rows.\n";
#endif
    return false;
  }

  if (!s.OK() || !t.OK()) {
#ifndef NDEBUG
    std::cerr << "A PIP_Solution_Node::Tableau matrix is broken.\n";
#endif
    return false;
  }

  if (denominator <= 0) {
#ifndef NDEBUG
    std::cerr << "PIP_Solution_Node::Tableau with non-positive denominator.\n";
#endif
    return false;
  }

  // All tests passed.
  return true;
}

bool
PIP_Tree_Node::OK() const {
#ifndef NDEBUG
  using std::endl;
  using std::cerr;
#endif
  const Constraint_System::const_iterator begin = constraints_.begin();
  const Constraint_System::const_iterator end = constraints_.end();

  // Parameter constraint system should contain no strict inequalities.
  for (Constraint_System::const_iterator ci = begin; ci != end; ++ci)
    if (ci->is_strict_inequality()) {
#ifndef NDEBUG
      cerr << "The feasible region of the PIP_Problem parameter context"
           << "is defined by a constraint system containing strict "
           << "inequalities."
	   << endl;
      ascii_dump(cerr);
#endif
      return false;
    }
  return true;
}

void
PIP_Tree_Node
::add_constraint(const Row& row, const Variables_Set& parameters) {
  const Variables_Set::const_iterator p_begin = parameters.begin();
  const Variables_Set::const_iterator p_end = parameters.end();
  PPL_ASSERT(static_cast<dimension_type>(std::distance(p_begin, p_end)) + 1
             == row.size());
  // FIXME: optimize the computation of expr.
  Linear_Expression expr = Linear_Expression(row[0]);
  dimension_type j = 1;
  for (Variables_Set::const_iterator pj = p_begin; pj != p_end; ++pj, ++j)
    expr += row[j] * Variable(*pj);
  constraints_.insert(expr >= 0);
}

bool
PIP_Solution_Node::OK() const {
#ifndef NDEBUG
  using std::cerr;
#endif
  if (!PIP_Tree_Node::OK())
    return false;

  // Check that every member used is OK.

  if (!tableau.OK())
    return false;

  // Check coherency of basis, mapping, var_row and var_column
  if (basis.size() != mapping.size()) {
#ifndef NDEBUG
    cerr << "The PIP_Solution_Node::basis and PIP_Solution_Node::mapping "
         << "vectors do not have the same number of elements.\n";
#endif
    return false;
  }
  if (basis.size() != var_row.size() + var_column.size()) {
#ifndef NDEBUG
    cerr << "The sum of number of elements in the PIP_Solution_Node::var_row "
         << "and PIP_Solution_Node::var_column vectors is different from the "
         << "number of elements in the PIP_Solution_Node::basis vector.\n";
#endif
    return false;
  }
  if (var_column.size() != tableau.s.num_columns()) {
#ifndef NDEBUG
    cerr << "The number of elements in the PIP_Solution_Node::var_column "
         << "vector is different from the number of columns in the "
         << "PIP_Solution_Node::tableau.s Matrix.\n";
#endif
    return false;
  }
  if (var_row.size() != tableau.s.num_rows()) {
#ifndef NDEBUG
    cerr << "The number of elements in the PIP_Solution_Node::var_row "
         << "vector is different from the number of rows in the "
         << "PIP_Solution_Node::tableau.s Matrix.\n";
#endif
    return false;
  }
  for (dimension_type i = mapping.size(); i-- > 0; ) {
    const dimension_type rowcol = mapping[i];
    if (basis[i] && var_column[rowcol] != i) {
#ifndef NDEBUG
      cerr << "Variable " << i << " is basic and corresponds to column "
           << rowcol << " but PIP_Solution_Node::var_column[" << rowcol
           << "] does not correspond to variable " << i << ".\n";
#endif
      return false;
    }
    if (!basis[i] && var_row[rowcol] != i) {
#ifndef NDEBUG
      cerr << "Variable " << i << " is nonbasic and corresponds to row "
           << rowcol << " but PIP_Solution_Node::var_row[" << rowcol
           << "] does not correspond to variable " << i << ".\n";
#endif
      return false;
    }
  }
  // All checks passed.
  return true;
}

bool
PIP_Decision_Node::OK() const {
  /* FIXME: finish me! */

  // Perform base class well-formedness check on this node.
  if (!PIP_Tree_Node::OK())
    return false;

  // Recursively check if child nodes are well-formed.
  if (true_child && !true_child->OK())
    return false;
  if (false_child && !false_child->OK())
    return false;

  // Decision nodes with a false child must have exactly one constraint.
  if (false_child) {
    dimension_type
      dist = std::distance(constraints_.begin(), constraints_.end());
    if (dist != 1) {
#ifndef NDEBUG
      std::cerr << "PIP_Decision_Node with a 'false' child has "
                << dist << " parametric constraints (should be 1).\n";
#endif
      return false;
    }
  }

  // All checks passed.
  return true;
}

void
PIP_Decision_Node::update_tableau(const PIP_Problem& problem,
                                  const dimension_type external_space_dim,
                                  const dimension_type first_pending_constraint,
                                  const Constraint_Sequence& input_cs,
                                  const Variables_Set& parameters) {
  true_child->update_tableau(problem,
                             external_space_dim,
                             first_pending_constraint,
                             input_cs,
                             parameters);
  if (false_child)
    false_child->update_tableau(problem,
                                external_space_dim,
                                first_pending_constraint,
                                input_cs,
                                parameters);
  PPL_ASSERT(OK());
}

PIP_Problem_Status
PIP_Decision_Node::solve(PIP_Tree_Node*& parent_ref,
                         const PIP_Problem& problem,
                         const Matrix& context,
                         const Variables_Set& params,
                         dimension_type space_dimension) {
  PPL_ASSERT(true_child != 0);
  Matrix context_true(context);
  Variables_Set parameters(params);
  update_context(parameters, context_true, artificial_parameters,
                 space_dimension);
  merge_assign(context_true, constraints_, parameters);
  PIP_Problem_Status stt = true_child->solve(true_child, problem,
                                             context_true, parameters,
                                             space_dimension);

  PIP_Problem_Status stf = UNFEASIBLE_PIP_PROBLEM;
  if (false_child) {
    // Decision nodes with false child must have exactly one constraint
    PPL_ASSERT(1 == std::distance(constraints_.begin(), constraints_.end()));
    Matrix context_false(context);
    update_context(context_false, artificial_parameters);
    merge_assign(context_false, constraints_, parameters);
    Row& last = context_false[context_false.num_rows()-1];
    negate_assign(last, last, 1);
    stf = false_child->solve(false_child, problem, context_false, parameters,
                             space_dimension);
  }

  if (stt == UNFEASIBLE_PIP_PROBLEM && stf == UNFEASIBLE_PIP_PROBLEM) {
    parent_ref = 0;
    delete this;
    return UNFEASIBLE_PIP_PROBLEM;
  }
  else
    return OPTIMIZED_PIP_PROBLEM;
}

void
PIP_Solution_Node::Tableau::normalize() {
  if (denominator == 1)
    return;

  const dimension_type num_rows = s.num_rows();
  const dimension_type s_cols = s.num_columns();
  const dimension_type t_cols = t.num_columns();

  // Compute global gcd.
  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  gcd = denominator;
  for (dimension_type i = num_rows; i-- > 0; ) {
    const Row& s_i = s[i];
    for (dimension_type j = s_cols; j-- > 0; ) {
      const Coefficient& s_ij = s_i[j];
      if (s_ij != 0) {
        gcd_assign(gcd, s_ij, gcd);
        if (gcd == 1)
          return;
      }
    }
    const Row& t_i = t[i];
    for (dimension_type j = t_cols; j-- > 0; ) {
      const Coefficient& t_ij = t_i[j];
      if (t_ij != 0) {
        gcd_assign(gcd, t_ij, gcd);
        if (gcd == 1)
          return;
      }
    }
  }

  PPL_ASSERT(gcd > 1);
  // Normalize all coefficients.
  for (dimension_type i = num_rows; i-- > 0; ) {
    Row& s_i = s[i];
    for (dimension_type j = s_cols; j-- > 0; ) {
      Coefficient& s_ij = s_i[j];
      exact_div_assign(s_ij, s_ij, gcd);
    }
    Row& t_i = t[i];
    for (dimension_type j = t_cols; j-- > 0; ) {
      Coefficient& t_ij = t_i[j];
      exact_div_assign(t_ij, t_ij, gcd);
    }
  }
  // Normalize denominator.
  exact_div_assign(denominator, denominator, gcd);
}

void
PIP_Solution_Node::Tableau::scale(Coefficient_traits::const_reference ratio) {
  for (dimension_type i = s.num_rows(); i-- > 0; ) {
    Row& s_i = s[i];
    for (dimension_type j = s.num_columns(); j-- > 0; )
      s_i[j] *= ratio;
    Row& t_i = t[i];
    for (dimension_type j = t.num_columns(); j-- > 0; )
      t_i[j] *= ratio;
  }
  denominator *= ratio;
}

bool
PIP_Solution_Node::Tableau
::is_better_pivot(const std::vector<dimension_type>& mapping,
                  const std::vector<bool>& basis,
                  const dimension_type i,
                  const dimension_type j,
                  const dimension_type i_,
                  const dimension_type j_) const {
  dimension_type k;
  dimension_type num_params = t.num_columns();
  dimension_type num_rows = s.num_rows();
  const Row& s_i = s[i];
  const Row& s_i_ = s[i_];
  const Row& t_i = t[i];
  const Row& t_i_ = t[i_];
  bool columns_are_different = false;
  for (k=0; k<num_params; ++k) {
    PPL_DIRTY_TEMP_COEFFICIENT(t_ikXs_i_j_);
    PPL_DIRTY_TEMP_COEFFICIENT(t_i_kXs_ij);
    t_ikXs_i_j_ = t_i[k] * s_i_[j_];
    t_i_kXs_ij = t_i_[k] * s_i[j];
    for (dimension_type x=0; x<num_rows; ++x) {
      const Row& s_x = s[x];
      if (s_x[j] * t_ikXs_i_j_ != s_x[j_] * t_i_kXs_ij) {
        columns_are_different = true;
        goto endloop;
      }
    }
  }
endloop:
  return columns_are_different
         && column_lower(s, mapping, basis, s_i, j, s_i_, j_, t_i[k], t_i_[k]);
}

void
PIP_Tree_Node::ascii_dump(std::ostream& s) const {
  s << "\nconstraints_\n";
  constraints_.ascii_dump(s);
  dimension_type artificial_parameters_size = artificial_parameters.size();
  s << "\nartificial_parameters( " << artificial_parameters_size << " )\n";
  for (dimension_type i=0; i<artificial_parameters_size; ++i)
    artificial_parameters[i].ascii_dump(s);
}

bool
PIP_Tree_Node::ascii_load(std::istream& s) {
  std::string str;
  if (!(s >> str) || str != "constraints_")
    return false;
  constraints_.ascii_load(s);

  if (!(s >> str) || str != "artificial_parameters(")
    return false;
  dimension_type artificial_parameters_size;
  if (!(s >> artificial_parameters_size))
    return false;
  Artificial_Parameter ap;
  for (dimension_type i=0; i<artificial_parameters_size; ++i) {
    if (!ap.ascii_load(s))
      return false;
    artificial_parameters.push_back(ap);
  }

  PPL_ASSERT(OK());
  return true;
}

PIP_Tree_Node*
PIP_Solution_Node::clone() const {
  return new PIP_Solution_Node(*this);
}

PIP_Tree_Node*
PIP_Decision_Node::clone() const {
  return new PIP_Decision_Node(*this);
}

void
PIP_Solution_Node::Tableau::ascii_dump(std::ostream& st) const {
  st << "denominator " << denominator << "\n";
  st << "variables ";
  s.ascii_dump(st);
  st << "parameters ";
  t.ascii_dump(st);
}

bool
PIP_Solution_Node::Tableau::ascii_load(std::istream& st) {
  std::string str;
  if (!(st >> str) || str != "denominator")
    return false;
  Coefficient den;
  if (!(st >> den))
    return false;
  denominator = den;
  if (!(st >> str) || str != "variables")
    return false;
  if (!s.ascii_load(st))
    return false;
  if (!(st >> str) || str != "parameters")
    return false;
  if (!t.ascii_load(st))
    return false;
  return true;
}

void
PIP_Solution_Node::ascii_dump(std::ostream& s) const {
  PIP_Tree_Node::ascii_dump(s);

  s << "\ntableau\n";
  tableau.ascii_dump(s);

  s << "\nbasis ";
  dimension_type basis_size = basis.size();
  s << basis_size;
  for (dimension_type i=0; i<basis_size; ++i)
    s << (basis[i] ? " true" : " false");

  s << "\nmapping ";
  dimension_type mapping_size = mapping.size();
  s << mapping_size;
  for (dimension_type i=0; i<mapping_size; ++i)
    s << " " << mapping[i];

  s << "\nvar_row ";
  dimension_type var_row_size = var_row.size();
  s << var_row_size;
  for (dimension_type i=0; i<var_row_size; ++i)
    s << " " << var_row[i];

  s << "\nvar_column ";
  dimension_type var_column_size = var_column.size();
  s << var_column_size;
  for (dimension_type i=0; i<var_column_size; ++i)
    s << " " << var_column[i];
  s << "\n";

  s << "special_equality_row " << special_equality_row << "\n";
  s << "big_dimension " << big_dimension << "\n";

  s << "sign ";
  dimension_type sign_size = sign.size();
  s << sign_size;
  for (dimension_type i=0; i<sign_size; ++i) {
    s << " ";
    switch (sign[i]) {
    case UNKNOWN:
      s << "UNKNOWN";
      break;
    case ZERO:
      s << "ZERO";
      break;
    case POSITIVE:
      s << "POSITIVE";
      break;
    case NEGATIVE:
      s << "NEGATIVE";
      break;
    case MIXED:
      s << "MIXED";
      break;
    }
  }
  s << "\n";

  dimension_type solution_size = solution.size();
  s << "solution " << solution_size << "\n";
  for (dimension_type i=0; i<solution_size; ++i)
    solution[i].ascii_dump(s);
  s << "\n";

  s << "solution_valid " << (solution_valid ? "true" : "false") << "\n";
}

bool
PIP_Solution_Node::ascii_load(std::istream& s) {
  if (!PIP_Tree_Node::ascii_load(s))
    return false;

  std::string str;
  if (!(s >> str) || str != "tableau")
    return false;
  if (!tableau.ascii_load(s))
    return false;

  if (!(s >> str) || str != "basis")
    return false;
  dimension_type basis_size;
  if (!(s >> basis_size))
    return false;
  basis.clear();
  for (dimension_type i=0; i<basis_size; ++i) {
    if (!(s >> str))
      return false;
    bool val = false;
    if (str == "true")
      val = true;
    else if (str != "false")
      return false;
    basis.push_back(val);
  }

  if (!(s >> str) || str != "mapping")
    return false;
  dimension_type mapping_size;
  if (!(s >> mapping_size))
    return false;
  mapping.clear();
  for (dimension_type i=0; i<mapping_size; ++i) {
    dimension_type val;
    if (!(s >> val))
      return false;
    mapping.push_back(val);
  }

  if (!(s >> str) || str != "var_row")
    return false;
  dimension_type var_row_size;
  if (!(s >> var_row_size))
    return false;
  var_row.clear();
  for (dimension_type i=0; i<var_row_size; ++i) {
    dimension_type val;
    if (!(s >> val))
      return false;
    var_row.push_back(val);
  }

  if (!(s >> str) || str != "var_column")
    return false;
  dimension_type var_column_size;
  if (!(s >> var_column_size))
    return false;
  var_column.clear();
  for (dimension_type i=0; i<var_column_size; ++i) {
    dimension_type val;
    if (!(s >> val))
      return false;
    var_column.push_back(val);
  }

  if (!(s >> str) || str != "special_equality_row")
    return false;
  if (!(s >> special_equality_row))
    return false;

  if (!(s >> str) || str != "big_dimension")
    return false;
  if (!(s >> big_dimension))
    return false;

  if (!(s >> str) || str != "sign")
    return false;
  dimension_type sign_size;
  if (!(s >> sign_size))
    return false;
  sign.clear();
  for (dimension_type i=0; i<sign_size; ++i) {
    if (!(s >> str))
      return false;
    Row_Sign val;
    if (str == "UNKNOWN")
      val = UNKNOWN;
    else if (str == "ZERO")
      val = ZERO;
    else if (str == "POSITIVE")
      val = POSITIVE;
    else if (str == "NEGATIVE")
      val = NEGATIVE;
    else if (str == "MIXED")
      val = MIXED;
    else
      return false;
    sign.push_back(val);
  }

  if (!(s >> str) || str != "solution")
    return false;
  dimension_type solution_size;
  if (!(s >> solution_size))
    return false;
  solution.clear();
  for (dimension_type i=0; i<solution_size; ++i) {
    Linear_Expression val;
    if (!val.ascii_load(s))
      return false;
    solution.push_back(val);
  }

  if (!(s >> str) || str != "solution_valid")
    return false;
  if (!(s >> str))
    return false;
  if (str == "true")
    solution_valid = true;
  else if (str == "false")
    solution_valid = false;
  else
    return false;

  PPL_ASSERT(OK());
  return true;
}

const Linear_Expression&
PIP_Solution_Node
::parametric_values(Variable v,
                    const Variables_Set& parameters) const {
  Variables_Set all_parameters(parameters);
  // Complete the parameter set with artificials.
  insert_artificials(all_parameters,
                     tableau.s.num_columns() + tableau.t.num_columns() - 1);

  const_cast<PIP_Solution_Node&>(*this).update_solution(all_parameters);
  dimension_type id = v.id();
  dimension_type j;
  Variables_Set::iterator location = all_parameters.lower_bound(id);
  if (location == all_parameters.end())
    j = id;
  else {
    if (*location == id) {
#ifndef NDEBUG
      std::cerr << "PIP_Solution_Node::parametric_values(Variable): "
                   "Supplied Variable corresponds to a parameter"
                << std::endl;
#endif
      j = not_a_dimension();
    } else
      j = id - std::distance(all_parameters.begin(),location);
  }

  return solution[j];
}

PIP_Solution_Node::Row_Sign
PIP_Solution_Node::row_sign(const Row& x, dimension_type big_dimension) {
  if (big_dimension != not_a_dimension()) {
    /* If a big parameter has been set and its coefficient is not zero,
      just return the sign of the coefficient */
    const Coefficient& c = x[big_dimension];
    if (c > 0)
      return POSITIVE;
    if (c < 0)
      return NEGATIVE;
    // otherwise c == 0, then no big parameter involved
  }
  PIP_Solution_Node::Row_Sign sign = ZERO;
  for (int i = x.size(); i-- > 0; ) {
    const Coefficient& c = x[i];
    switch (sign) {
      case UNKNOWN:
        // cannot happen
        break;
      case ZERO:
        if (c < 0)
          sign = NEGATIVE;
        else if (c > 0)
          sign = POSITIVE;
        break;
      case NEGATIVE:
        if (c > 0)
          return MIXED;
        break;
      case POSITIVE:
        if (c < 0)
          return MIXED;
        break;
      case MIXED:
        // cannot happen
        break;
    }
  }
  return sign;
}

bool
PIP_Solution_Node::compatibility_check(const Matrix& ctx, const Row& cnst) {
  Matrix s(ctx);
  s.add_row(cnst);
  dimension_type i, i_, j, k, j_, j__, var_i, var_j;
  dimension_type num_rows = s.num_rows();
  dimension_type num_cols = s.num_columns();
  dimension_type num_vars = num_cols-1;
  std::vector<Coefficient> scaling(num_rows, 1);
  PPL_DIRTY_TEMP_COEFFICIENT(sij);
  PPL_DIRTY_TEMP_COEFFICIENT(mult);
  PPL_DIRTY_TEMP_COEFFICIENT(gcd);
  PPL_DIRTY_TEMP_COEFFICIENT(scale_factor);
  PPL_DIRTY_TEMP_COEFFICIENT(scaling_i);
  std::vector<dimension_type> mapping;
  std::vector<bool> basis;
  std::vector<dimension_type> var_row;
  std::vector<dimension_type> var_column;
  // Column 0 is the constant term, not a variable
  var_column.push_back(not_a_dimension());
  for (j = 1; j <= num_vars; ++j) {
    basis.push_back(true);
    mapping.push_back(j);
    var_column.push_back(j-1);
  }
  for (i = 0; i < num_rows; ++i) {
    basis.push_back(false);
    mapping.push_back(i);
    var_row.push_back(i+num_vars);
  }
  Row p(num_cols, compute_capacity(num_cols, Matrix::max_num_columns()),
        Row::Flags());

  /* Perform simplex pivots on the context until we find an empty solution
   * or an optimum */
  for (;;) {
    // Look for a negative RHS (=constant term, stored in matrix column 0)
    i = num_rows;
    j = 0;

    // Find Pivot row i and pivot column j, by maximizing the pivot column
    for (i_ = 0; i_ < num_rows; ++i_) {
      if (s[i_][0] >= 0)
        continue;
      if (!find_lexico_minimum_column(s, mapping, basis, s[i_], 1, j_)) {
        // No positive pivot candidate: empty problem
        return false;
      }
      if (j == 0 || column_lower(s, mapping, basis,
                    s[i], j, s[i_], j_, s[i][0], s[i_][0])) {
        i = i_;
        j = j_;
      }
    }

    if (j == 0) {
      // No negative RHS: fractional optimum found. If it is integer, then
      // the test is successful. Otherwise, generate a new cut.
      bool all_integer_vars = true;
      for (i=0; i<num_vars; ++i) {
        if (basis[i])
          // basic variable = 0 -> integer
          continue;
        // nonbasic variable
        i_ = mapping[i];
        if (s[i_][0] % scaling[i_] != 0) {
          // Constant term is not integer.
          all_integer_vars = false;
          break;
        }
      }
      if (all_integer_vars) {
        // Found an integer solution, thus the check is successful
        return true;
      }
      for (i = 0; i < num_vars; ++i) {
        if (basis[i])
          // basic variable = 0 -> integer
          continue;
        i_ = mapping[i];
        const Coefficient& d = scaling[i_];
        if (s[i_][0] % d == 0)
          continue;
        // Constant term is not integer. Generate a new cut.
        var_row.push_back(mapping.size());
        basis.push_back(false);
        mapping.push_back(num_rows);
        s.add_zero_rows(1, Row::Flags());
        const Row& row = s[i_];
        Row& cut = s[num_rows++];
        for (j = 0; j < num_cols; ++j)
          mod_assign(cut[j], row[j], d);
        cut[0] -= d;
        scaling.push_back(d);
      }
      continue;
    }

    // Now we have a positive s[i][j] pivot

    /* Normalize the tableau before pivoting */
    for (i_=0; i_<num_rows; ++i_)
      row_normalize(s[i_], scaling[i_]);

    /* update basis */
    var_j = var_column[j];
    var_i = var_row[i];
    basis[var_j] = false;
    mapping[var_j] = i;
    basis[var_i] = true;
    mapping[var_i] = j;
    var_column[j] = var_i;
    var_row[i] = var_j;

    /* create the identity matrix row corresponding to basic variable j */
    for (j_=0; j_<num_cols; ++j_)
      p[j_]=0;
    p[j] = 1;
    p.swap(s[i]);
    sij = p[j];
    scaling_i = scaling[i];
    scaling[i] = 1;

    // Perform a pivot operation on the matrix
    for (j_ = 0; j_ < num_cols; ++j_) {
      if (j_ == j)
        continue;
      const Coefficient& sij_ = p[j_];
      if (sij_ == 0)
        // if element j of pivot row is zero, nothing to do for this column
        continue;
      for (k = 0; k < num_rows; ++k) {
        Row& row = s[k];
        mult = row[j] * sij_;
        if (mult % sij != 0) {
          // Must scale row to stay in integer case
          gcd_assign(gcd, mult, sij);
          exact_div_assign(scale_factor, sij, gcd);
          for (j__=0; j__<num_cols; ++j__)
            row[j__] *= scale_factor;
          mult *= scale_factor;
          scaling[k] *= scale_factor;
        }
        row[j_] -= mult / sij;
      }
    }
    if (sij != scaling_i) {
      // Update column only if pivot != 1
      for (k=0; k<num_rows; ++k) {
        Row& row = s[k];
        Coefficient& skj = row[j];
        mult = skj*scaling_i;
        if (mult % sij != 0) {
          // as above, we must perform row scaling
          gcd_assign(gcd, mult, sij);
          exact_div_assign(scale_factor, sij, gcd);
          for (j__=0; j__<num_cols; ++j__)
            row[j__] *= scale_factor;
          scaling[k] *= scale_factor;
          mult *= scale_factor;
        }
        exact_div_assign(skj, mult, sij);
      }
    }
  }

  // This point is never reached
  return false;
}

void
PIP_Solution_Node::update_tableau(const PIP_Problem& problem,
                                  dimension_type external_space_dim,
                                  dimension_type first_pending_constraint,
                                  const Constraint_Sequence& input_cs,
                                  const Variables_Set& parameters) {
  dimension_type i;
  dimension_type n_params = parameters.size();
  dimension_type n_vars = external_space_dim - n_params;
  dimension_type internal_space_dim = tableau.s.num_columns()
                                      + tableau.t.num_columns() - 1;
  Constraint_Sequence::const_iterator cst;

  // Create the parameter column, corresponding to the constant term
  if (tableau.t.num_columns() == 0) {
    tableau.t.add_zero_columns(1);
    internal_space_dim = 0;
  }

  // add new columns to the tableau
  /* FIXME: when the node or its parents have artificial parameters, we
    must insert new parameter columns before the columns corresponding to
    the artificial parameters. Meanwhile parameter insertion after a first
    solve (incremental solving) is broken. */
  for (i=internal_space_dim; i<external_space_dim; ++i) {
    if (parameters.count(i) == 1)
      tableau.t.add_zero_columns(1);
    else {
      dimension_type column = tableau.s.num_columns();
      tableau.s.add_zero_columns(1);
      if (tableau.s.num_rows() == 0) {
        // No rows have been added yet
        basis.push_back(true);
        mapping.push_back(column);
      } else {
        /* Need to insert the original variable id before the slack variable
          id's to respect variable ordering */
        dimension_type j;
        basis.insert(basis.begin()+column, true);
        mapping.insert(mapping.begin()+column, column);
        // update variable id's of slack variables
        for (j = var_row.size(); j-- > 0; )
          if (var_row[j] >= column)
            ++var_row[j];
        for (j = var_column.size(); j-- > 0; )
          if (var_column[j] >= column)
            ++var_column[j];
        if (special_equality_row > 0)
          ++special_equality_row;
      }
      var_column.push_back(column);
    }
  }
  internal_space_dim = external_space_dim;
  if (big_dimension == not_a_dimension()
      && problem.big_parameter_dimension != not_a_dimension()) {
    // Compute the column number of big parameter in tableau.t matrix
    Variables_Set::const_iterator begin = parameters.begin();
    Variables_Set::const_iterator pos
      = parameters.find(problem.big_parameter_dimension);
    big_dimension = std::distance(begin, pos) + 1;
  }

  const Coefficient& denom = tableau.get_denominator();

  for (cst = input_cs.begin() + first_pending_constraint;
       cst < input_cs.end(); ++cst) {
    int v = 0;
    int p = 1;
    Row var(n_vars, tableau.s_capacity(), Row::Flags());
    Row param(n_params+1, tableau.t_capacity(), Row::Flags());
    Coefficient cnst_term = cst->inhomogeneous_term();
    dimension_type dim = cst->space_dimension();
    if (cst->is_strict_inequality())
      // convert c > 0  <=>  c-1 >= 0
      cnst_term -= 1;
    param[0] = cnst_term * denom;
    for (i=0; i<dim; i++) {
      if (parameters.count(i) == 1) {
        param[p++] = cst->coefficient(Variable(i)) * denom;
      } else {
        const Coefficient& c = cst->coefficient(Variable(i));
        dimension_type idx = mapping[v];
        if (basis[v])
          // Basic variable : add c * x_i
          var[idx] += c * denom;
        else {
          // Nonbasic variable : add c * row_i
          add_assign(var, tableau.s[idx], c);
          add_assign(param, tableau.t[idx], c);
        }
        ++v;
      }
    }
    if (row_sign(var, not_a_dimension()) != ZERO) {
      /* parametric-only constraints have already been inserted in initial
        context, so no need to insert them in the tableau
      */
      dimension_type var_id = mapping.size();
      dimension_type row_id = tableau.s.num_rows();
      tableau.s.add_row(var);
      tableau.t.add_row(param);
      sign.push_back(row_sign(param, big_dimension));
      basis.push_back(false);
      mapping.push_back(row_id);
      var_row.push_back(var_id);
      if (cst->is_equality()) {
        /* Handle equality constraints. After having added the f_i(x,p) >= 0
          constraint, we must add -f_i(x,p) to the special equality row */
        if (special_equality_row == 0 || basis[special_equality_row]) {
          // The special constraint has not been created yet
          /* FIXME: for now, we don't handle the case where the variable is
            basic, and create a new row. This might be faster however. */
          ++var_id;
          ++row_id;
          negate_assign(var, var, 0);
          negate_assign(param, param, 0);
          tableau.s.add_row(var);
          tableau.t.add_row(param);
          sign.push_back(row_sign(param, big_dimension));
          special_equality_row = mapping.size();
          basis.push_back(false);
          mapping.push_back(row_id);
          var_row.push_back(var_id);
        } else {
          // The special constraint already exists and is nonbasic
          dimension_type row = mapping[special_equality_row];
          sub_assign(tableau.s[row], var);
          sub_assign(tableau.t[row], param);
        }
      }
    }
  }
  PPL_ASSERT(OK());
}

void
PIP_Solution_Node::update_solution(const Variables_Set& parameters) {
  if (solution_valid)
    return;
  dimension_type num_vars = tableau.s.num_columns();
  const Coefficient& d = tableau.get_denominator();
  if (solution.size() != num_vars)
    solution.resize(num_vars);
  for (dimension_type i = num_vars; i-- > 0; ) {
    Linear_Expression& sol = solution[i];
    if (basis[i]) {
      sol = Linear_Expression(0);
    } else {
      Row& row = tableau.t[mapping[i]];
      sol = Linear_Expression(row[0]/d);
      dimension_type k;
      Variables_Set::const_iterator j;
      Variables_Set::const_iterator j_end = parameters.end();
      for (j = parameters.begin(), k = 1; j != j_end; ++j, ++k)
        sol += (row[k]/d) * Variable(*j);
    }
  }
  solution_valid = true;
}

PIP_Problem_Status
PIP_Solution_Node::solve(PIP_Tree_Node*& parent_ref,
                         const PIP_Problem& problem,
                         const Matrix& ctx,
                         const Variables_Set& params,
                         dimension_type space_dimension) {
  Matrix context(ctx);
  Variables_Set parameters(params);
  update_context(parameters, context, artificial_parameters,
                 space_dimension);
  merge_assign(context, constraints_, parameters);
  const dimension_type n_a_d = not_a_dimension();
  Coefficient gcd;

  // Main loop of the simplex algorithm
  for(;;) {
    dimension_type i, j;
    dimension_type i_ = n_a_d;
    dimension_type i__ = n_a_d;
    dimension_type num_rows = tableau.t.num_rows();
    dimension_type num_vars = tableau.s.num_columns();
    dimension_type num_params = tableau.t.num_columns();
    Row_Sign s;
    PPL_ASSERT(OK());

#ifdef NOISY_PIP
    tableau.ascii_dump(std::cout);
    std::cout << "context ";
    context.ascii_dump(std::cout);
#endif

    for (i=0; i<num_rows; ++i) {
      Row_Sign s = sign[i];
      if (s == UNKNOWN || s == MIXED) {
        s = row_sign(tableau.t[i], big_dimension);
        sign[i] = s;
      }
      /* Locate first row with negative parameter row */
      if (s == NEGATIVE && i_ == n_a_d)
        i_ = i;
      /* Locate first row with unknown-signed parameter row */
      if (s == MIXED && i__ == n_a_d)
        i__ = i;
    }

    /* If no negative parameter row found, try to refine the sign of
       undetermined rows using compatibility checks with the current context
    */
    if (i_ == n_a_d && i__ != n_a_d) {
      for (i=i__; i<num_rows; ++i) {
        if (sign[i] != MIXED)
          continue;
        s = ZERO;
        if (compatibility_check(context, tableau.t[i]))
          // constraint t_i(z) >= 0 is compatible with the context
          s = POSITIVE;
        Row c(num_params, Row::Flags());
        negate_assign(c, tableau.t[i], tableau.get_denominator());
        if (compatibility_check(context, c)) {
          // constraint t_i(z) < 0 <=> -t_i(z)-1 >= 0 is compatible
          if (s == POSITIVE)
            s = MIXED;
          else
            s = NEGATIVE;
        }
        if (s == NEGATIVE && i_ == n_a_d)
          // first negative row found
          i_ = i;
        if (s != MIXED) {
          // clear first mixed-sign row index if row is found to be not mixed
          if (i == i__)
            i__ = n_a_d;
        } else if (i__ == n_a_d)
          // first mixed-sign row found
          i__ = i;
        sign[i] = s;
      }
    }

    /* If there remains a row i with undetermined sign and at least one
       positive S_ij coefficient, where constraint t_i(z) > 0 is not
       compatible with the context, the row parameter can be considered
       negative
    */
    if (i_ == n_a_d && i__ != n_a_d) {
      for (i=i__; i<num_rows; ++i) {
        if (sign[i] != MIXED)
          continue;
        bool found = false;
        const Row& p = tableau.s[i];
        for (j=0; j<num_vars; ++j)
          if (p[j] > 0) {
            found = true;
            break;
          }
        if (!found)
          continue;
        Row row(tableau.t[i]);
        const Coefficient& denom = tableau.get_denominator();
        PPL_DIRTY_TEMP_COEFFICIENT(mod);
        mod_assign(mod, row[0], denom);
        row[0] -= ((mod == 0) ? denom : mod);
        if (compatibility_check(context, row)) {
          if (i__ == n_a_d)
            i__ = i;
        } else {
          sign[i] = NEGATIVE;
          if (i_ == n_a_d)
            i_ = i;
          if (i__ == i)
            i__ = n_a_d;
        }
      }
    }

#ifdef NOISY_PIP
    std::cout << "sign =";
    for (i=0; i<sign.size(); ++i)
      std::cout << " " << "?0+-*"[sign[i]];
    std::cout << std::endl;
#endif

    /* If we have found a row i_ with negative parameters :
       Either the problem is empty, or a pivoting step is required
    */
    if (i_ != n_a_d) {
      /* Search for the best pivot row. */
      dimension_type j_ = n_a_d;
      for (i=0; i<num_rows; ++i) {
        if (sign[i] != NEGATIVE)
          continue;

        if (!find_lexico_minimum_column(tableau.s, mapping, basis,
                                        tableau.s[i], 0, j)) {
          /* If no positive S_ij: problem is empty */
#ifdef NOISY_PIP
          std::cout << "No positive pivot found: Solution = _|_"
                    << std::endl;
#endif
          parent_ref = 0;
          delete this;
          return UNFEASIBLE_PIP_PROBLEM;
        }
        if (j_ == n_a_d
            || tableau.is_better_pivot(mapping, basis, i, j, i_, j_)) {
          // First pivot column found
          // OR better pivot row/column pair found -> update pivot
          i_ = i;
          j_ = j;
          if (problem.control_parameters[PIP_Problem::PIVOT_ROW_STRATEGY]
              == PIP_Problem::PIVOT_ROW_STRATEGY_FIRST)
            // stop at first valid row
            break;
        }
      }

#ifdef NOISY_PIP
      std::cout << "Pivot row: " << i_ << " pivot column: " << j_
                << std::endl;
#endif

      /* Normalize the tableau before pivoting */
      tableau.normalize();

      PPL_DIRTY_TEMP_COEFFICIENT(sij);
      sij = tableau.s[i_][j_];

      /* ** Perform pivot operation ** */
      PPL_DIRTY_TEMP_COEFFICIENT(c);

      /* update basis */
      dimension_type var_j = var_column[j_];
      dimension_type var_i = var_row[i_];
      basis[var_j] = false;
      mapping[var_j] = i_;
      var_row[i_] = var_j;
      basis[var_i] = true;
      mapping[var_i] = j_;
      var_column[j_] = var_i;

      /* create the identity matrix row corresponding to basic variable j_ */
      tableau.s.add_zero_rows(1, Row::Flags());
      Row& prs = tableau.s[num_rows];
      tableau.t.add_zero_rows(1, Row::Flags());
      Row& prt = tableau.t[num_rows];
      prs[j_] = tableau.get_denominator();
      /* swap it with pivot row which would become identity after pivoting */
      prs.swap(tableau.s[i_]);
      prt.swap(tableau.t[i_]);
      sign[i_] = ZERO;
      /* save current denominator corresponding to sij */
      PPL_DIRTY_TEMP_COEFFICIENT(sij_denom);
      sij_denom = tableau.get_denominator();
      /* Compute columns s[*][j] : s[k][j] -= s[k][j_] * prs[j] / sij */
      PPL_DIRTY_TEMP_COEFFICIENT(scale_factor);
      dimension_type k;
      for (j=0; j<num_vars; ++j) {
        if (j==j_)
          continue;
        const Coefficient& prsj = prs[j];
        if (prsj == 0)
          // if element j of pivot row is zero, nothing to do for this column
          continue;
        for (k=0; k<num_rows; ++k) {
          PPL_DIRTY_TEMP_COEFFICIENT(mult);
          mult = prsj * tableau.s[k][j_];
          if (mult % sij != 0) {
            // Must scale matrix to stay in integer case
            gcd_assign(gcd, mult, sij);
            scale_factor = sij/gcd;
            tableau.scale(scale_factor);
            mult *= scale_factor;
          }
          tableau.s[k][j] -= mult / sij;
        }
      }

      /* Compute columns t[*][j] : t[k][j] -= t[k][j_] * prt[j] / sij */
      for (j=0; j<num_params; ++j) {
        const Coefficient& prtj = prt[j];
        if (prtj == 0)
          // if element j of pivot row is zero, nothing to do for this column
          continue;
        for (k=0; k<num_rows; ++k) {
          c = prtj * tableau.s[k][j_];
          if (c % sij != 0) {
            // Must scale matrix to stay in integer case
            gcd_assign(gcd, c, sij);
            scale_factor = sij/gcd;
            tableau.scale(scale_factor);
            c *= scale_factor;
          }
          c /= sij;
          tableau.t[k][j] -= c;

          s = sign[k];
          if (s != MIXED) {
            switch (s) {
               case ZERO:
                if (c > 0)
                  sign[k] = NEGATIVE;
                else if (c < 0)
                  sign[k] = POSITIVE;
                break;
              case POSITIVE:
                if (c > 0)
                  sign[k] = MIXED;
                break;
              case NEGATIVE:
                if (c < 0)
                  sign[k] = MIXED;
                break;
              default:
                break;
            }
          }
        }
      }

      // Drop rows previously added at end of tableau.
      tableau.s.erase_to_end(num_rows);
      tableau.t.erase_to_end(num_rows);

      /* compute column s[*][j_] : s[k][j_] /= sij */
      if (sij != sij_denom) {
        // Update column only if pivot != 1
        for (k=0; k<num_rows; ++k) {
          Coefficient& c = tableau.s[k][j_];
          PPL_DIRTY_TEMP_COEFFICIENT(numerator);
          numerator = c * sij_denom;
          if (numerator % sij != 0) {
            PPL_DIRTY_TEMP_COEFFICIENT(gcd);
            gcd_assign(gcd, numerator, sij);
            scale_factor = sij/gcd;
            tableau.scale(scale_factor);
            numerator *= scale_factor;
          }
          c = numerator / sij;
        }
      }
      solution_valid = false;
    }

    /* Otherwise, we have found a row i__ with mixed parameter sign. */
    else if (i__ != n_a_d) {
      dimension_type neg = n_a_d;
      PPL_DIRTY_TEMP_COEFFICIENT(ns);
      PPL_DIRTY_TEMP_COEFFICIENT(score);

      /* Look for a constraint with mixed parameter sign with no positive
       * variable coefficients */
      for (i=i__; i<num_rows; ++i) {
        if (sign[i] != MIXED)
          continue;
        for (j=0; j<num_vars; ++j) {
          if (tableau.s[i][j] > 0)
            break;
        }
        /* Choose row with lowest score, potentially eliminating
         * implicated tautologies if some exist */
        if (j == num_vars) {
          score = 0;
          for (j=0; j<num_params; ++j)
            score += tableau.t[i][j];
          if (neg == n_a_d || score < ns) {
            neg = i;
            ns = score;
          }
        }
      }
      if (neg != n_a_d) {
        i = neg;
#ifdef NOISY_PIP
        std::cout << "Found row with unknown parameter sign and negative "
                     "variable coefficients: " << i
                  << std::endl;
#endif
        Row r(tableau.t[i]);
        r.normalize();
        context.add_row(r);
        add_constraint(r, parameters);
        sign[i] = POSITIVE;
#ifdef NOISY_PIP
        using namespace IO_Operators;
        Constraint_System::const_iterator c = constraints_.begin();
        Constraint_System::const_iterator c_end = constraints_.end();
        Constraint_System::const_iterator c1 = c;
        while (++c1 != constraints_.end())
          c = c1;
        std::cout << "Adding tautology: " << *c << std::endl;
#endif
      } else {
        /* Heuristically choose "best" pivoting row. */
        PPL_DIRTY_TEMP_COEFFICIENT(score);
        PPL_DIRTY_TEMP_COEFFICIENT(best);
        best = 0;
        dimension_type best_i = n_a_d;
        for (i = i__; i < num_rows; ++i) {
          if (sign[i] != MIXED)
            continue;
          const Row& row = tableau.t[i];
          score = 0;
          for (j = 0; j < num_params; ++j)
            score += row[j];
          if (best_i == n_a_d || score < best) {
            best = score;
            best_i = i;
          }
        }
        i__ = best_i;

        Row test(tableau.t[i__]);
        test.normalize();
#ifdef NOISY_PIP
        {
          using namespace IO_Operators;
          Linear_Expression e;
          Variables_Set::const_iterator p;
          dimension_type j;
          for (p = parameters.begin(), j=1; p != parameters.end(); ++p, ++j)
            e += test[j] * Variable(*p);
          e += test[0];
          std::cout << "Found row with mixed parameter sign: " << i__
                    << "\nSolution depends on the sign of parameter " << e
                    << std::endl;
        }
#endif

        /* Create a solution Node to become "true" version of current Node */
        PIP_Tree_Node *tru = new PIP_Solution_Node(*this, true);
        context.add_row(test);
        PIP_Problem_Status status_t = tru->solve(tru, problem, context,
                                                 parameters, space_dimension);

        /* Modify *this to become "false" version */
        Constraint_System cs;
        Artificial_Parameter_Sequence aps;
        cs.swap(constraints_);
        aps.swap(artificial_parameters);
        PIP_Tree_Node *fals = this;
        Row& testf = context[context.num_rows()-1];
        negate_assign(testf, test, 1);
        PIP_Problem_Status status_f = solve(fals, problem, context, parameters,
                                            space_dimension);

        if (status_t == UNFEASIBLE_PIP_PROBLEM
            && status_f == UNFEASIBLE_PIP_PROBLEM) {
          parent_ref = 0;
          return UNFEASIBLE_PIP_PROBLEM;
        } else if (status_t == UNFEASIBLE_PIP_PROBLEM) {
          cs.swap(constraints_);
          aps.swap(artificial_parameters);
          add_constraint(testf, parameters);
          return OPTIMIZED_PIP_PROBLEM;
        } else if (status_f == UNFEASIBLE_PIP_PROBLEM) {
          cs.swap(tru->constraints_);
          aps.swap(tru->artificial_parameters);
          tru->add_constraint(test, parameters);
          parent_ref = tru;
          return OPTIMIZED_PIP_PROBLEM;
        }

        /* Create a decision Node to become parent of current Node */
        PIP_Decision_Node* parent = new PIP_Decision_Node(fals, tru);
        parent->add_constraint(test, parameters);

        if (!cs.empty()) {
          /* If node to be solved had tautologies, store them in a new
             decision node */
          parent = new PIP_Decision_Node(0, parent);
          cs.swap(parent->constraints_);
        }
        aps.swap(parent->artificial_parameters);

        parent_ref = parent;
        return OPTIMIZED_PIP_PROBLEM;
      }
    }

    /* Otherwise, all parameters are positive: we have found a continuous
     * solution. If the solution happens to be integer, then it is the
     * solution of the  integer problem. Otherwise, we may need to generate
     * a new cut to try and get back into the integer case. */
    else {
#ifdef NOISY_PIP
      std::cout << "All parameters are positive."
                << std::endl;
#endif
      tableau.normalize();

      // Look for a row with non integer parameter coefficients (first is okay)
      const Coefficient& d = tableau.get_denominator();
      for (i=0; i<num_vars; ++i) {
        if (basis[i])
          // basic variable = 0 -> integer
          continue;
        const Row& row = tableau.t[mapping[i]];
        for (j=0; j<num_params; ++j) {
          if (row[j] % d != 0)
            goto endsearch;
        }
      }
      endsearch:

      if (i == num_vars) {
        /* The solution is integer */
#ifdef NOISY_PIP
        std::cout << "Solution found for problem in current node."
                  << std::endl;
#endif
        return OPTIMIZED_PIP_PROBLEM;
      }
      /* The solution is non-integer. We have to generate a cut. */
      PPL_DIRTY_TEMP_COEFFICIENT(mod);
      const PIP_Problem::Control_Parameter_Value cutting_strategy
        = problem.control_parameters[PIP_Problem::CUTTING_STRATEGY];
      if (cutting_strategy == PIP_Problem::CUTTING_STRATEGY_FIRST) {
        // Find the first row with simplest parametric part.
        dimension_type best_i = n_a_d;
        dimension_type best_pcount = n_a_d;
        dimension_type pcount;
        for (i_ = 0; i_ < num_vars; ++i_) {
          if (basis[i_])
            continue;
          i = mapping[i_];
          const Row& row_t = tableau.t[i];
          pcount = 0;
          for (j = 0; j < num_params; ++j) {
            mod_assign(mod, row_t[j], d);
            if (mod != 0)
              ++pcount;
          }
          if (pcount != 0 && (best_i == n_a_d || (pcount < best_pcount))) {
            best_pcount = pcount;
            best_i = i;
          }
        }
        generate_cut(best_i, parameters, context, space_dimension);
      }
      else {
        assert(cutting_strategy == PIP_Problem::CUTTING_STRATEGY_DEEPEST
               || cutting_strategy == PIP_Problem::CUTTING_STRATEGY_ALL);
        /* Find the row with simplest parametric part which will generate
          the "deepest" cut */
        PPL_DIRTY_TEMP_COEFFICIENT(score);
        PPL_DIRTY_TEMP_COEFFICIENT(score2);
        PPL_DIRTY_TEMP_COEFFICIENT(best_score);
        best_score = 0;
        dimension_type best_i = n_a_d;
        dimension_type best_pcount = n_a_d;
        dimension_type pcount;
        std::vector<dimension_type> all_best_is;
        for (i_ = 0; i_ < num_vars; ++i_) {
          if (basis[i_])
            continue;
          i = mapping[i_];
          const Row& row_t = tableau.t[i];
          const Row& row_s = tableau.s[i];
          score = 0;
          pcount = 0;
          for (j = 0; j < num_params; ++j) {
            mod_assign(mod, row_t[j], d);
            if (mod != 0) {
              score += d - mod;
              ++pcount;
            }
          }
          score2 = 0;
          for (j = 0; j < num_vars; ++j) {
            mod_assign(mod, row_s[j], d);
            score2 += d - mod;
          }
          score *= score2;
          /* Choose row i if:
            row i is non-integer
            AND (no row has been chosen yet
                 OR row i has number of non-integer parameter
                    coefficients lower than the current best row
                 OR row i has the same number of non-integer parameter
                    coefficients as the current best row, and its score is
                    better)
          */
          if (pcount != 0
              && (best_i == n_a_d
                  || (pcount < best_pcount)
                  || (pcount == best_pcount && score > best_score))) {
            if (pcount < best_pcount)
              all_best_is.clear();
            best_score = score;
            best_pcount = pcount;
            best_i = i;
          }
          if (pcount > 0)
            all_best_is.push_back(i);
        }
        if (cutting_strategy == PIP_Problem::CUTTING_STRATEGY_DEEPEST)
          generate_cut(best_i, parameters, context, space_dimension);
        else /* cutting_strategy == PIP_Problem::CUTTING_STRATEGY_ALL */ {
          for (i = all_best_is.size(); i-- > 0; )
            generate_cut(all_best_is[i], parameters, context, space_dimension);
        }
      }
    } // if (i__ != n_a_d)
  } // Main loop of the simplex algorithm

  return OPTIMIZED_PIP_PROBLEM;
}

void
PIP_Solution_Node::generate_cut(const dimension_type index,
                                Variables_Set& parameters,
                                Matrix& context,
                                dimension_type& space_dimension) {
  const dimension_type num_rows = tableau.t.num_rows();
  PPL_ASSERT(index < num_rows);
  const dimension_type num_vars = tableau.s.num_columns();
  const dimension_type num_params = tableau.t.num_columns();
  PPL_ASSERT(num_params == parameters.size());
  const Coefficient& den = tableau.get_denominator();

  PPL_DIRTY_TEMP_COEFFICIENT(mod);

#ifdef NOISY_PIP
  std::cout << "Row " << index << " contains non-integer coefficients. "
            << "Cut generation required."
            << std::endl;
#endif // #ifdef NOISY_PIP

  // Test if cut to be generated must be parametric or not.
  bool generate_parametric_cut = false;
  {
    // Limiting the scope of reference row_t (may be later invalidated).
    const Row& row_t = tableau.t[index];
    for (dimension_type j = 1; j < num_params; ++j)
      if (row_t[j] % den != 0) {
        generate_parametric_cut = true;
        break;
      }
  }

  // Column index of already existing Artificial_Parameter.
  dimension_type ap_column = not_a_dimension();
  bool reuse_ap = false;

  if (generate_parametric_cut) {
    // Fractional parameter coefficient found: generate parametric cut.

    // Limiting the scope of reference row_t (may be later invalidated).
    const Row& row_t = tableau.t[index];
    mod_assign(mod, row_t[0], den);
    Linear_Expression expr;
    if (mod != 0) {
      expr += den;
      expr -= mod;
    }
    Variables_Set::const_iterator p = parameters.begin();
    for (dimension_type j = 1; j < num_params; ++j, ++p) {
      mod_assign(mod, row_t[j], den);
      // FIXME: find a way to optimize the following.
      if (mod != 0)
        expr += (den - mod) * Variable(*p);
    }
    // Generate new artificial parameter.
    Artificial_Parameter ap(expr, den);

    // Search if the Artificial_Parameter has already been generated.
    ap_column = space_dimension;
    const PIP_Tree_Node* node = this;
    do {
      for (dimension_type j = node->artificial_parameters.size(); j-- > 0; ) {
        --ap_column;
        if (node->artificial_parameters[j] == ap) {
          reuse_ap = true;
          break;
        }
      }
      node = node->parent();
    } while (!reuse_ap && node != 0);

    if (reuse_ap) {
      // We can re-use an existing Artificial_Parameter.
#ifdef NOISY_PIP
      using namespace IO_Operators;
      std::cout << "Re-using parameter " << Variable(ap_column)
                << " = (" << expr << ")/" << den
                << std::endl;
#endif // #ifdef NOISY_PIP
      ap_column = ap_column - num_vars + 1;
    }
    else {
      // Here reuse_ap == false: the Artificial_Parameter does not exist yet.
      // Beware: possible reallocation invalidates row references.
      tableau.t.add_zero_columns(1);
      context.add_zero_columns(1);
      artificial_parameters.push_back(ap);
      parameters.insert(space_dimension);
#ifdef NOISY_PIP
      using namespace IO_Operators;
      std::cout << "Creating new parameter "
                << Variable(space_dimension)
                << " = (" << expr << ")/" << den
                << std::endl;
#endif // #ifdef NOISY_PIP
      ++space_dimension;
      ap_column = num_params;

      // Update current context with constraints on the new parameter.
      const dimension_type ctx_num_rows = context.num_rows();
      context.add_zero_rows(2, Row::Flags());
      Row& ctx1 = context[ctx_num_rows];
      Row& ctx2 = context[ctx_num_rows+1];
      // Recompute row reference after possible reallocation.
      const Row& row_t = tableau.t[index];
      for (dimension_type j = 0; j < num_params; ++j) {
        mod_assign(mod, row_t[j], den);
        if (mod != 0) {
          ctx1[j] = den;
          ctx1[j] -= mod;
          neg_assign(ctx2[j], ctx1[j]);
        }
      }
      neg_assign(ctx1[num_params], den);
      ctx2[num_params] = den;
      // ctx2[0] += den-1;
      ctx2[0] += den;
      --ctx2[0];
#ifdef NOISY_PIP
      {
        using namespace IO_Operators;
        Variables_Set::const_iterator p = parameters.begin();
        Linear_Expression expr1(ctx1[0]);
        Linear_Expression expr2(ctx2[0]);
        for (dimension_type j = 1; j <= num_params; ++j, ++p) {
          expr1 += ctx1[j] * Variable(*p);
          expr2 += ctx2[j] * Variable(*p);
        }
        std::cout << "Inserting into context: "
                  << Constraint(expr1 >= 0) << " ; "
                  << Constraint(expr2 >= 0) << std::endl;
      }
#endif // #ifdef NOISY_PIP
    }
  }

  // Generate new cut.
  tableau.s.add_zero_rows(1, Row::Flags());
  tableau.t.add_zero_rows(1, Row::Flags());
  Row& cut_s = tableau.s[num_rows];
  Row& cut_t = tableau.t[num_rows];
  // Recompute references after possible reallocation.
  const Row& row_s = tableau.s[index];
  const Row& row_t = tableau.t[index];
  for (dimension_type j = 0; j < num_vars; ++j) {
    mod_assign(cut_s[j], row_s[j], den);
  }
  for (dimension_type j = 0; j < num_params; ++j) {
    mod_assign(mod, row_t[j], den);
    if (mod != 0) {
      cut_t[j] = mod;
      cut_t[j] -= den;
    }
  }
  if (ap_column != not_a_dimension())
    // If we re-use an existing Artificial_Parameter
    cut_t[ap_column] = den;

#ifdef NOISY_PIP
  {
    using namespace IO_Operators;
    Linear_Expression expr;
    dimension_type ti = 1;
    dimension_type si = 0;
    for (dimension_type j = 0; j < space_dimension; ++j) {
      if (parameters.count(j) == 1)
        expr += cut_t[ti++] * Variable(j);
      else
        expr += cut_s[si++] * Variable(j);
    }
    std::cout << "Adding cut: "
              << Constraint(expr + cut_t[0] >= 0)
              << std::endl;
  }
#endif
  var_row.push_back(num_rows + num_vars);
  basis.push_back(false);
  mapping.push_back(num_rows);
  sign.push_back(NEGATIVE);
}

} // namespace Parma_Polyhedra_Library
