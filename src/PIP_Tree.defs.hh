/* PIP_Tree class declaration.
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

#ifndef PPL_PIP_Tree_defs_hh
#define PPL_PIP_Tree_defs_hh 1

#include "PIP_Tree.types.hh"
#include "Constraint_System.types.hh"
#include "PIP_Problem.types.hh"

namespace Parma_Polyhedra_Library {

/*! \brief
  The base class for the nodes of the trees representing the solutions
  of PIP problems.
*/
class PIP_Tree_Node {
public:
  //! Returns \p this if \p *this is a solution node, 0 otherwise.
  virtual const PIP_Solution_Node* as_solution() const;

  //! Returns \p this if \p *this is a solution node, 0 otherwise.
  virtual PIP_Solution_Node* as_solution();

  //! Returns \p this if \p *this is a decision node, 0 otherwise.
  virtual const PIP_Decision_Node* as_decision() const;

  //! Returns \p this if \p *this is a decision node, 0 otherwise.
  virtual PIP_Decision_Node* as_decision();

  //! Destructor.
  virtual ~PIP_Tree_Node();
};

//! A tree node representing part of the space of solutions.
class PIP_Solution_Node : public PIP_Tree_Node {
public:
  //! Destructor.
  ~PIP_Solution_Node();

  //! Returns \p this.
  const PIP_Solution_Node* as_solution() const;

  //! Returns \p this.
  PIP_Solution_Node* as_solution();

  // get_bindings();
};

//! A tree node representing a decision in the space of solutions.
class PIP_Decision_Node : public PIP_Tree_Node {
public:
  //! Destructor.
  ~PIP_Decision_Node();

  //! Returns \p this.
  const PIP_Decision_Node* as_decision() const;

  //! Returns \p this.
  PIP_Decision_Node* as_decision();

  //! Returns a const pointer to the \v (true or false) branch of \p *this.
  const PIP_Tree_Node* child_node(bool v) const;

  //! Returns a pointer to the \v (true or false) branch of \p *this.
  PIP_Tree_Node* child_node(bool v);

  //! Returns the system of constraints controlling \p *this.
  const Constraint_System& constraints();

private:
  //! Pointer to the "true" child of \p *this.
  PIP_Tree_Node* true_child;

  //! Pointer to the "false" child of \p *this.
  PIP_Tree_Node* false_child;

  // Only PIP_Problem is allowed to use the constructor.
  friend class PIP_Problem;

  /*! \brief
    Constructs if \p cs then \p tcp \p else \p fcp.

    Constructs a decision node controlled by \p cs (which is copied),
    with "true" child \p tcp and "false" child \p fcp.

    \param cs
    The system of constraints controlling the node.

    \exception std::invalid_argument
    Thrown if \p cs contains strict inequalities.
  */
  PIP_Decision_Node(const Constraint_System& cs,
                    PIP_Tree_Node* fcp, PIP_Tree_Node* tcp);

  /*! \brief
    Constructs if \p cs then \p tcp \p else \p fcp.

    Constructs a decision node controlled by \p cs (which is recycled),
    with "true" child \p tcp and "false" child \p fcp.

    \param cs
    The system of constraints controlling the node.  It is not
    declared <CODE>const</CODE> because its data-structures may be
    recycled to build the polyhedron.

    \param dummy
    A dummy tag to syntactically differentiate this one
    from the other constructors.

    \exception std::invalid_argument
    Thrown if \p cs contains strict inequalities.
  */
  PIP_Decision_Node(const Constraint_System& cs, Recycle_Input dummy,
                    PIP_Tree_Node* fcp, PIP_Tree_Node* tcp);
};

typedef const PIP_Tree_Node* PIP_Tree;

} // namespace Parma_Polyhedra_Library

#include "PIP_Tree.inlines.hh"

#endif // !defined(PPL_PIP_Tree_defs_hh)
