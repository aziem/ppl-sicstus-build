/* Unlimited_Sparse_Row class implementation: inline
   functions.
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

#ifndef PPL_Unlimited_Sparse_Row_inlines_hh
#define PPL_Unlimited_Sparse_Row_inlines_hh 1

// TODO: Remove this.
// Added to please KDevelop4.
#include "Unlimited_Sparse_Row.defs.hh"

namespace Parma_Polyhedra_Library {

inline
Unlimited_Sparse_Row::Unlimited_Sparse_Row()
  : tree() {
}

inline
Unlimited_Sparse_Row
::Unlimited_Sparse_Row(const This& x)
  : tree(x.tree) {
}

inline
Unlimited_Sparse_Row
::Unlimited_Sparse_Row(const std::vector<Coefficient> &v)
  : tree(v) {
}

inline Unlimited_Sparse_Row&
Unlimited_Sparse_Row::operator=(const This& x) {
  tree = x.tree;
  return *this;
}

inline void
Unlimited_Sparse_Row::swap(This& x) {
  tree.swap(x.tree);
}

inline void
Unlimited_Sparse_Row::swap(dimension_type i, dimension_type j) {
  if (tree.empty())
    return;
  CO_Tree::inorder_iterator itr_i;
  CO_Tree::inorder_iterator itr_j;
  tree.go_down_searching_key(itr_i, i);
  tree.go_down_searching_key(itr_j, j);
  if (itr_i->first == i)
    if (itr_j->first == j)
      // Both elements are in the tree
      std::swap(itr_i->second, itr_j->second);
    else {
      // i is in the tree, j isn't
      Coefficient tmp;
      std::swap(itr_i->second, tmp);
      tree.erase(itr_i);
      tree.insert(j, Coefficient_zero(), itr_j);
      std::swap(itr_j->second, tmp);
    }
  else
    if (itr_j->first == j) {
      // j is in the tree, i isn't
      Coefficient tmp;
      std::swap(itr_j->second, tmp);
      tree.erase(itr_j);
      tree.insert(i, Coefficient_zero(), itr_i);
      std::swap(itr_i->second, tmp);
    } else {
      // Do nothing, elements are both unstored zeroes.
    }
}

inline void
Unlimited_Sparse_Row::swap(iterator i, iterator j) {
  PPL_ASSERT(!i.itr.is_before_begin());
  PPL_ASSERT(!i.itr.is_at_end());
  PPL_ASSERT(!j.itr.is_before_begin());
  PPL_ASSERT(!j.itr.is_at_end());
  std::swap(i->second, j->second);
}

inline memory_size_type
Unlimited_Sparse_Row::external_memory_in_bytes() const {
  return tree.external_memory_in_bytes();
}

inline bool
Unlimited_Sparse_Row::OK() const {
  return tree.OK();
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::begin() {
  iterator itr(tree.before_begin());
  ++itr;
  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::end() {
  return iterator(tree.end());
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::begin() const {
  const_iterator itr(tree.before_begin());
  ++itr;
  return itr;
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::end() const {
  return const_iterator(tree.end());
}

inline Unlimited_Sparse_Row::unordered_iterator
Unlimited_Sparse_Row::unordered_begin() {
  return tree.unordered_begin();
}

inline Unlimited_Sparse_Row::unordered_iterator
Unlimited_Sparse_Row::unordered_end() {
  return tree.unordered_end();
}

inline Unlimited_Sparse_Row::unordered_const_iterator
Unlimited_Sparse_Row::unordered_begin() const {
  return tree.unordered_begin();
}

inline Unlimited_Sparse_Row::unordered_const_iterator
Unlimited_Sparse_Row::unordered_end() const {
  return tree.unordered_end();
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find(const dimension_type c) {
  iterator itr;
  find_assign(c, itr);
  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::lower_bound(const dimension_type c) {
  iterator itr;
  lower_bound_assign(c, itr);
  return itr;
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::find(const dimension_type c) const {
  const_iterator itr;
  find_assign(c, itr);
  return itr;
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row
::lower_bound(const dimension_type c) const {
  const_iterator itr;
  lower_bound_assign(c, itr);
  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find(const dimension_type c,
                                        iterator itr) {
  find_hint_assign(c, itr);
  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::lower_bound(const dimension_type c,
                                               iterator itr) {
  lower_bound_hint_assign(c, itr);
  return itr;
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::find(const dimension_type c,
                                        const_iterator itr) const {
  find_hint_assign(c, itr);
  return itr;
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::lower_bound(const dimension_type c,
                                               const_iterator itr) const {
  lower_bound_hint_assign(c, itr);
  return itr;
}

inline void
Unlimited_Sparse_Row
::find_create_assign(dimension_type i, const Coefficient& x,
                     CO_Tree::inorder_iterator& itr) {
  tree.insert(i, x, itr);
}

inline void
Unlimited_Sparse_Row
::find_create_assign(dimension_type i, CO_Tree::inorder_iterator& itr) {
  tree.insert(i, itr);
}

inline void
Unlimited_Sparse_Row
::find_create_assign(const dimension_type i, const Coefficient& x,
                     iterator& itr) {
  tree.insert(i, x, itr.itr);
}

inline void
Unlimited_Sparse_Row
::find_create_assign(const dimension_type i, iterator& itr) {
  tree.insert(i, itr.itr);
}

inline void
Unlimited_Sparse_Row
::find_create_hint_assign(const dimension_type i, const Coefficient& x,
                          iterator& itr) {
  PPL_ASSERT(!itr.itr.is_at_end());
  tree.insert_hint(i, x, itr.itr);
}

inline void
Unlimited_Sparse_Row
::find_create_hint_assign(const dimension_type i, iterator& itr) {
  tree.insert_hint(i, itr.itr);
}

inline void
Unlimited_Sparse_Row
::find_assign(const dimension_type c, iterator& itr) {
  if (tree.empty()) {
    itr = end();
    return;
  }
  itr = iterator(&tree);
  tree.go_down_searching_key(itr.itr, c);
  if ((itr.itr)->first != c)
    itr = end();
}

inline void
Unlimited_Sparse_Row
::find_assign(const dimension_type c, const_iterator& itr) const {
  if (tree.empty()) {
    itr = end();
    return;
  }
  itr = const_iterator(&tree);
  tree.go_down_searching_key(itr.itr, c);
  if ((itr.itr)->first != c)
    itr = end();
}

inline void
Unlimited_Sparse_Row
::lower_bound_assign(const dimension_type c, iterator& itr) {
  if (tree.empty()) {
    itr = end();
    return;
  }
  itr = iterator(&tree);
  tree.go_down_searching_key(itr.itr, c);
  if ((itr.itr)->first < c)
    ++itr;
  PPL_ASSERT(itr.itr.is_at_end() || itr->first >= c);
}

inline void
Unlimited_Sparse_Row
::lower_bound_assign(const dimension_type c, const_iterator& itr) const {
  if (tree.empty()) {
    itr = end();
    return;
  }
  itr = const_iterator(&tree);
  tree.go_down_searching_key(itr.itr, c);
  if ((itr.itr)->first < c)
    ++itr;
  PPL_ASSERT(itr.itr.is_at_end() || itr->first >= c);
}

inline void
Unlimited_Sparse_Row
::find_hint_assign(const dimension_type c, iterator& itr) {
  PPL_ASSERT(!itr.itr.is_at_end());
  find_hint_assign(c, itr.itr);
}

inline void
Unlimited_Sparse_Row
::find_hint_assign(const dimension_type c, const_iterator& itr) const {
  PPL_ASSERT(!itr.itr.is_at_end());
  find_hint_assign(c, itr.itr);
}

inline void
Unlimited_Sparse_Row
::lower_bound_hint_assign(const dimension_type c, iterator& itr) {
  PPL_ASSERT(!itr.itr.is_at_end());
  lower_bound_hint_assign(c, itr.itr);
}

inline void
Unlimited_Sparse_Row
::lower_bound_hint_assign(const dimension_type c, const_iterator& itr) const {
  PPL_ASSERT(!itr.itr.is_at_end());
  lower_bound_hint_assign(c, itr.itr);
}

inline void
Unlimited_Sparse_Row
::lower_bound_hint_assign(dimension_type i,
                          CO_Tree::inorder_iterator& itr) {
  tree.lower_bound(itr, i);
}

inline void
Unlimited_Sparse_Row
::lower_bound_hint_assign(dimension_type i,
                          CO_Tree::inorder_const_iterator& itr) const {
  tree.lower_bound(itr, i);
}

inline void
Unlimited_Sparse_Row
::find_hint_assign(dimension_type i, CO_Tree::inorder_iterator& itr) {
  PPL_ASSERT(!tree.empty());
  if (itr->first <= i)
    while (itr.has_parent() && itr->first < i)
      itr.get_parent();
  else
    while (itr.has_parent() && itr->first > i)
      itr.get_parent();

  tree.go_down_searching_key(itr, i);

#ifndef NDEBUG
  CO_Tree::inorder_iterator itr2(&tree);
  tree.go_down_searching_key(itr2, i);
  PPL_ASSERT(itr == itr2);
#endif

  if (itr->first != i)
    itr = tree.end();
}

inline void
Unlimited_Sparse_Row
::find_hint_assign(dimension_type i,
                   CO_Tree::inorder_const_iterator& itr) const {
  PPL_ASSERT(!tree.empty());
  if (itr->first <= i)
    while (itr.has_parent() && itr->first < i)
      itr.get_parent();
  else
    while (itr.has_parent() && itr->first > i)
      itr.get_parent();

  tree.go_down_searching_key(itr, i);

#ifndef NDEBUG
  CO_Tree::inorder_const_iterator itr2(&tree);
  tree.go_down_searching_key(itr2, i);
  PPL_ASSERT(itr == itr2);
#endif

  if (itr->first != i)
    itr = tree.end();
}

inline void
Unlimited_Sparse_Row::find2(const dimension_type c1,
                                            const dimension_type c2,
                                            iterator& itr1, iterator& itr2) {
  if (tree.empty()) {
    itr1 = end();
    itr2 = itr1;
    return;
  }

  itr1.itr = CO_Tree::inorder_iterator(&tree);
  while (1) {
    if (itr1.itr->first < c1) {
      if (itr1.itr->first < c2) {
        if (!itr1.itr.get_right_child_value()) {
          ++(itr1.itr);
          itr2.itr = itr1.itr;
          break;
        }
      } else {
        itr2.itr = itr1.itr;
        tree.go_down_searching_key(itr1.itr, c1);
        tree.go_down_searching_key(itr2.itr, c2);
        break;
      }
    } else {
      if (itr1.itr->first == c1 || itr1.itr->first <= c2) {
        itr2.itr = itr1.itr;
        tree.go_down_searching_key(itr1.itr, c1);
        tree.go_down_searching_key(itr2.itr, c2);
        break;
      } else {
        if (!itr1.itr.get_left_child_value()) {
          itr2.itr = itr1.itr;
          break;
        }
      }
    }
  }
  if (itr1.itr->first != c1)
    itr1 = end();
  if (itr2.itr->first != c2)
    itr2 = end();
}

inline void
Unlimited_Sparse_Row::find2(const dimension_type c1,
                                         const dimension_type c2,
                                         const_iterator& itr1,
                                         const_iterator& itr2) const {
  if (tree.empty()) {
    itr1 = end();
    itr2 = itr1;
    return;
  }

  itr1.itr = CO_Tree::inorder_const_iterator(&tree);
  while (1) {
    if (itr1.itr->first < c1) {
      if (itr1.itr->first < c2) {
        if (!itr1.itr.get_right_child_value()) {
          ++(itr1.itr);
          itr2.itr = itr1.itr;
          break;
        }
      } else {
        itr2.itr = itr1.itr;
        tree.go_down_searching_key(itr1.itr, c1);
        tree.go_down_searching_key(itr2.itr, c2);
        break;
      }
    } else {
      if (itr1.itr->first == c1 || itr1.itr->first <= c2) {
        itr2.itr = itr1.itr;
        tree.go_down_searching_key(itr1.itr, c1);
        tree.go_down_searching_key(itr2.itr, c2);
        break;
      } else {
        if (!itr1.itr.get_left_child_value()) {
          itr2.itr = itr1.itr;
          break;
        }
      }
    }
  }
  if (itr1.itr->first != c1)
    itr1 = end();
  if (itr2.itr->first != c2)
    itr2 = end();
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::reset(iterator pos) {
  const dimension_type i = pos->first;
  tree.erase(pos.itr);
  return lower_bound(i);
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::reset(iterator first,
                                         iterator last) {
  if (first == last)
    return first;
  --last;
  const dimension_type i = first->first;
  const dimension_type j = last->first;
  PPL_ASSERT(i <= j);
  while (!first.itr.is_at_end() && first->first <= j) {
    tree.erase(first.itr);
    first = lower_bound(i);
  }
  return first;
}

inline void
Unlimited_Sparse_Row::reset(dimension_type i) {
  tree.erase(i);
}

inline void
Unlimited_Sparse_Row::reset(dimension_type i, dimension_type j) {
  iterator itr = lower_bound(i);

  while (!itr.itr.is_at_end() && itr->first < j) {
    reset(itr);
    itr = lower_bound(i);
  }
}

inline void
Unlimited_Sparse_Row::reset_after(dimension_type i) {
  iterator itr = lower_bound(i);

  while (!itr.itr.is_at_end()) {
    reset(itr);
    itr = lower_bound(i);
  }
}

inline void
Unlimited_Sparse_Row
::delete_element_and_shift(dimension_type i) {
  reset(i);
  for (iterator itr = lower_bound(i); !itr.itr.is_at_end(); ++itr)
    --(itr.itr->first);
  PPL_ASSERT(OK());
}

inline void
Unlimited_Sparse_Row::add_zeroes_and_shift(dimension_type n,
                                                        dimension_type i) {
  CO_Tree::inorder_iterator itr = tree.end();
  itr.get_previous_value();
  for ( ; !itr.is_before_begin() && itr->first >= i; itr.get_previous_value())
    itr->first += n;
  PPL_ASSERT(OK());
}

inline void
Unlimited_Sparse_Row::assign(dimension_type i,
                                          const Coefficient& x) {
  if (tree.empty())
    assign_if_nonzero(i, x);
  else {
    CO_Tree::inorder_iterator itr(&tree);
    tree.go_down_searching_key(itr, i);
    if (itr->first == i)
      itr->second = x;
    else
      if (x != 0)
        tree.insert(i, x, itr);
  }
}

inline void
Unlimited_Sparse_Row::assign_if_nonzero(dimension_type i,
                                                     const Coefficient& x) {
  if (x != 0)
    find_create(i, x);
}

inline Coefficient&
Unlimited_Sparse_Row::operator[](dimension_type i) {
  iterator itr;
  find_create_assign(i, itr);
  return itr->second;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find_create(const dimension_type i,
                                               const Coefficient& x) {
  iterator itr;
  find_create_assign(i, x, itr);
  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find_create(const dimension_type i) {
  iterator itr;
  find_create_assign(i, itr);
  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find_create(const dimension_type i,
                                               const Coefficient& x,
                                               iterator itr) {
  find_create_hint_assign(i, x, itr);
  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find_create(const dimension_type i,
                                               iterator itr) {
  find_create_hint_assign(i, itr);
  return itr;
}

inline void
Unlimited_Sparse_Row
::find_create_hint_assign(const dimension_type i, const Coefficient& x,
                          CO_Tree::inorder_iterator& itr) {
  PPL_ASSERT(!tree.empty());
  PPL_ASSERT(!itr.is_at_end());

  if (itr->first <= i)
    while (itr.has_parent() && itr->first < i)
      itr.get_parent();
  else
    while (itr.has_parent() && itr->first > i)
      itr.get_parent();

  tree.go_down_searching_key(itr, i);

#ifndef NDEBUG
  CO_Tree::inorder_iterator itr2(&tree);
  tree.go_down_searching_key(itr2, i);
  PPL_ASSERT(itr == itr2);
#endif

  if (itr->first == i)
    itr->second = x;
  else
    tree.insert_precise(i, x, itr);

  PPL_ASSERT(!itr.is_at_end());
  PPL_ASSERT(itr->first == i);
  PPL_ASSERT(itr->second == x);
}

inline void
Unlimited_Sparse_Row
::find_create_hint_assign(const dimension_type i,
                          CO_Tree::inorder_iterator& itr) {
  PPL_ASSERT(!tree.empty());
  PPL_ASSERT(!itr.is_at_end());

  if (itr->first <= i)
    while (itr.has_parent() && itr->first < i)
      itr.get_parent();
  else
    while (itr.has_parent() && itr->first > i)
      itr.get_parent();

  tree.go_down_searching_key(itr, i);

#ifndef NDEBUG
  CO_Tree::inorder_iterator itr2(&tree);
  tree.go_down_searching_key(itr2, i);
  PPL_ASSERT(itr == itr2);
#endif

  if (itr->first != i)
    tree.insert_precise(i, Coefficient_zero(), itr);

  PPL_ASSERT(!itr.is_at_end());
  PPL_ASSERT(itr->first == i);
}

inline const Coefficient&
Unlimited_Sparse_Row::operator[](const dimension_type i) const {
  return get(i);
}

inline const Coefficient&
Unlimited_Sparse_Row::get(dimension_type i) const {
  if (tree.empty())
    return Coefficient_zero();
  CO_Tree::inorder_const_iterator itr(&tree);
  tree.go_down_searching_key(itr, i);
  if (itr->first == i)
    return itr->second;
  else
    return Coefficient_zero();
}

inline void
Unlimited_Sparse_Row::get2(const dimension_type c1,
                                        const dimension_type c2,
                                        const Coefficient*& p1,
                                        const Coefficient*& p2) const {
  const_iterator itr1 = begin();
  const_iterator itr2 = begin();

  find2(c1, c2, itr1, itr2);

  if (itr1.itr.is_at_end())
    p1 = &Coefficient_zero();
  else
    p1 = &(itr1->second);

  if (itr2.itr.is_at_end())
    p2 = &Coefficient_zero();
  else
    p2 = &(itr2->second);
}


inline
Unlimited_Sparse_Row::iterator::iterator(CO_Tree* x)
  : itr(x) {
}

inline
Unlimited_Sparse_Row::iterator
::iterator(const CO_Tree::inorder_iterator& x)
  : itr(x) {
}

inline
Unlimited_Sparse_Row::iterator::iterator(const iterator& x)
  : itr(x.itr) {
}

inline
Unlimited_Sparse_Row::iterator&
Unlimited_Sparse_Row::iterator
::operator=(const iterator& x) {
  itr = x.itr;
  return *this;
}

inline bool
Unlimited_Sparse_Row::iterator
::operator==(const iterator& x) const {

  return itr == x.itr;
}

inline bool
Unlimited_Sparse_Row::iterator
::operator!=(const iterator& x) const {
  return !(*this == x);
}

inline Unlimited_Sparse_Row::iterator&
Unlimited_Sparse_Row::iterator::operator++() {

  itr.get_next_value();
  return *this;
}

inline Unlimited_Sparse_Row::iterator&
Unlimited_Sparse_Row::iterator::operator--() {

  itr.get_previous_value();
  return *this;
}

inline std::pair<dimension_type, Coefficient&>
Unlimited_Sparse_Row::iterator::operator*() {

  return std::pair<dimension_type, Coefficient&>(itr->first, itr->second);
}

inline Unlimited_Sparse_Row::iterator::Member_Access_Helper
Unlimited_Sparse_Row::iterator::operator->() {

  return Member_Access_Helper(itr->first, itr->second);
}

inline std::pair<dimension_type, const Coefficient&>
Unlimited_Sparse_Row::iterator::operator*() const {

  return *itr;
}

inline CO_Tree::inorder_iterator::Const_Member_Access_Helper
Unlimited_Sparse_Row::iterator::operator->() const {

  return itr.operator->();
}


inline
Unlimited_Sparse_Row::iterator::Member_Access_Helper
::Member_Access_Helper(dimension_type key, CO_Tree::data_type& data)
  : my_pair(key, data) {
}

inline
std::pair<dimension_type, Coefficient&>*
Unlimited_Sparse_Row::iterator::Member_Access_Helper
::operator->() {
  return &my_pair;
}

inline Unlimited_Sparse_Row::iterator
::operator const_iterator() const {
  return const_iterator(itr);
}


inline
Unlimited_Sparse_Row::const_iterator
::const_iterator(const CO_Tree* x)
  : itr(x) {
}

inline
Unlimited_Sparse_Row::const_iterator
::const_iterator(const CO_Tree::inorder_iterator& x)
  : itr(x) {
}

inline
Unlimited_Sparse_Row::const_iterator
::const_iterator(const CO_Tree::inorder_const_iterator& x)
  : itr(x) {
}

inline
Unlimited_Sparse_Row::const_iterator
::const_iterator(const const_iterator& x)
  : itr(x.itr) {
}

inline
Unlimited_Sparse_Row::const_iterator&
Unlimited_Sparse_Row::const_iterator
::operator=(const const_iterator& x) {
  itr = x.itr;
  return *this;
}

inline bool
Unlimited_Sparse_Row::const_iterator
::operator==(const const_iterator& x) const {

  return itr == x.itr;
}

inline bool
Unlimited_Sparse_Row::const_iterator
::operator!=(const const_iterator& x) const {
  return !(*this == x);
}

inline Unlimited_Sparse_Row::const_iterator&
Unlimited_Sparse_Row::const_iterator::operator++() {

  itr.get_next_value();
  return *this;
}

inline Unlimited_Sparse_Row::const_iterator&
Unlimited_Sparse_Row::const_iterator::operator--() {

  itr.get_previous_value();
  return *this;
}

inline std::pair<dimension_type, const Coefficient&>
Unlimited_Sparse_Row::const_iterator::operator*() const {

  return *itr;
}

inline CO_Tree::inorder_const_iterator::Const_Member_Access_Helper
Unlimited_Sparse_Row::const_iterator::operator->() const {

  return itr.operator->();
}


inline
Unlimited_Sparse_Row::unordered_iterator
::unordered_iterator(const CO_Tree::unordered_iterator& itr)
  : CO_Tree::unordered_iterator(itr) {
}

inline Unlimited_Sparse_Row::unordered_iterator
::Member_Access_Helper
Unlimited_Sparse_Row::unordered_iterator
::operator->() {
  return Member_Access_Helper(Base::operator->()->first,
                              Base::operator->()->second);
}

inline Unlimited_Sparse_Row::unordered_iterator
::Const_Member_Access_Helper
Unlimited_Sparse_Row::unordered_iterator
::operator->() const {
  return Const_Member_Access_Helper(Base::operator->()->first,
                                    Base::operator->()->second);
}

inline std::pair<dimension_type, Coefficient&>
Unlimited_Sparse_Row::unordered_iterator::operator*() {
  return std::pair<dimension_type, Coefficient&>(Base::operator->()->first,
                                                 Base::operator->()->second);
}

inline std::pair<dimension_type, const Coefficient&>
Unlimited_Sparse_Row::unordered_iterator::operator*() const {
  return std::pair<dimension_type, const Coefficient&>(
    Base::operator->()->first, Base::operator->()->second);
}

inline
Unlimited_Sparse_Row::unordered_iterator
::operator unordered_const_iterator() const {
  return unordered_const_iterator(*static_cast<const Base*>(this));
}


inline
Unlimited_Sparse_Row::unordered_iterator::Member_Access_Helper
::Member_Access_Helper(dimension_type key, Coefficient& data)
  : my_pair(key, data) {
}

inline std::pair<dimension_type, Coefficient&>*
Unlimited_Sparse_Row::unordered_iterator::Member_Access_Helper
::operator->() {
  return &my_pair;
}


inline
Unlimited_Sparse_Row::unordered_iterator
::Const_Member_Access_Helper
::Const_Member_Access_Helper(dimension_type key, const Coefficient& data)
  : my_pair(key, data) {
}

inline const std::pair<dimension_type, const Coefficient&>*
Unlimited_Sparse_Row::unordered_iterator
::Const_Member_Access_Helper::operator->() const {
  return &my_pair;
}


inline
Unlimited_Sparse_Row::unordered_const_iterator
::unordered_const_iterator(const CO_Tree::unordered_const_iterator& itr)
  : CO_Tree::unordered_const_iterator(itr) {
}

inline std::pair<dimension_type, const Coefficient&>
Unlimited_Sparse_Row::unordered_const_iterator
::operator*() const {
  return std::pair<dimension_type, const Coefficient&>(
    Base::operator->()->first,Base::operator->()->second);
}

inline Unlimited_Sparse_Row::unordered_const_iterator
::Const_Member_Access_Helper
Unlimited_Sparse_Row::unordered_const_iterator
::operator->() const {
  return Const_Member_Access_Helper(
    CO_Tree::unordered_const_iterator::operator->()->first,
    CO_Tree::unordered_const_iterator::operator->()->second);
}


inline
Unlimited_Sparse_Row::unordered_const_iterator
::Const_Member_Access_Helper
::Const_Member_Access_Helper(dimension_type key, const Coefficient& data)
  : my_pair(key, data) {
}

inline
const std::pair<dimension_type, const CO_Tree::data_type&>*
Unlimited_Sparse_Row::unordered_const_iterator
::Const_Member_Access_Helper::operator->() const {
  return &my_pair;
}


} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Unlimited_Sparse_Row_inlines_hh)
