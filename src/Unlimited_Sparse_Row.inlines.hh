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
Unlimited_Sparse_Row::Unlimited_Sparse_Row(const Unlimited_Sparse_Row& x)
  : tree(x.tree) {
}

inline Unlimited_Sparse_Row&
Unlimited_Sparse_Row::operator=(const Unlimited_Sparse_Row& x) {
  tree = x.tree;
  return *this;
}

inline void
Unlimited_Sparse_Row::clear() {
  tree.clear();
}

inline void
Unlimited_Sparse_Row::swap(Unlimited_Sparse_Row& x) {
  tree.swap(x.tree);
}

inline void
Unlimited_Sparse_Row::swap(dimension_type i, dimension_type j) {
  if (tree.empty())
    return;

  iterator itr_i = tree.bisect(i);
  iterator itr_j = tree.bisect(j);
  if (itr_i->first == i)
    if (itr_j->first == j)
      // Both elements are in the tree
      std::swap(itr_i->second, itr_j->second);
    else {
      // i is in the tree, j isn't
      PPL_DIRTY_TEMP_COEFFICIENT(tmp);
      std::swap(itr_i->second, tmp);
      tree.erase(itr_i);
      // Now both iterators have been invalidated.
      itr_j = tree.insert(j);
      std::swap(itr_j->second, tmp);
    }
  else
    if (itr_j->first == j) {
      // j is in the tree, i isn't
      PPL_DIRTY_TEMP_COEFFICIENT(tmp);
      std::swap(itr_j->second, tmp);
      // Now both iterators have been invalidated.
      tree.erase(itr_j);
      itr_i = tree.insert(i);
      std::swap(itr_i->second, tmp);
    } else {
      // Do nothing, elements are both unstored zeroes.
    }
}

inline void
Unlimited_Sparse_Row::swap(iterator i, iterator j) {
  PPL_ASSERT(i != end());
  PPL_ASSERT(j != end());
  std::swap(i->second, j->second);
}

inline memory_size_type
Unlimited_Sparse_Row::external_memory_in_bytes() const {
  return tree.external_memory_in_bytes();
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::begin() {
  return tree.begin();
}

inline const Unlimited_Sparse_Row::iterator&
Unlimited_Sparse_Row::end() {
  return tree.end();
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::begin() const {
  return tree.cbegin();
}

inline const Unlimited_Sparse_Row::const_iterator&
Unlimited_Sparse_Row::end() const {
  return tree.cend();
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::cbegin() const {
  return tree.cbegin();
}

inline const Unlimited_Sparse_Row::const_iterator&
Unlimited_Sparse_Row::cend() const {
  return tree.cend();
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find_create(dimension_type i,
                                  const Coefficient& x) {
  return tree.insert(i, x);
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find_create(dimension_type i) {
  return tree.insert(i);
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find_create(iterator itr, dimension_type i,
                                  const Coefficient& x) {
  return tree.insert(itr, i, x);
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find_create(iterator itr, dimension_type i) {
  return tree.insert(itr, i);
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find(dimension_type i) {

  iterator itr = tree.bisect(i);

  if (itr != end() && itr->first == i)
    return itr;

  return end();
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::find(iterator hint, dimension_type i) {

  iterator itr = tree.bisect_near(hint, i);

  if (itr != end() && itr->first == i)
    return itr;

  return end();
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::find(dimension_type i) const {

  const_iterator itr = tree.bisect(i);

  if (itr != end() && itr->first == i)
    return itr;

  return end();
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::find(const_iterator hint, dimension_type i) const {

  const_iterator itr = tree.bisect_near(hint, i);

  if (itr != end() && itr->first == i)
    return itr;

  return end();
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::lower_bound(dimension_type i) {

  iterator itr = tree.bisect(i);

  if (itr == end())
    return end();

  if (itr->first < i)
    ++itr;

  PPL_ASSERT(itr == end() || itr->first >= i);

  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::lower_bound(iterator hint, dimension_type i) {

  iterator itr = tree.bisect_near(hint, i);

  if (itr == end())
    return end();

  if (itr->first < i)
    ++itr;

  PPL_ASSERT(itr == end() || itr->first >= i);

  return itr;
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::lower_bound(dimension_type i) const {

  const_iterator itr = tree.bisect(i);

  if (itr == end())
    return end();

  if (itr->first < i)
    ++itr;

  PPL_ASSERT(itr == end() || itr->first >= i);

  return itr;
}

inline Unlimited_Sparse_Row::const_iterator
Unlimited_Sparse_Row::lower_bound(const_iterator hint, dimension_type i) const {

  const_iterator itr = tree.bisect_near(hint, i);

  if (itr == end())
    return end();

  if (itr->first < i)
    ++itr;

  PPL_ASSERT(itr == end() || itr->first >= i);

  return itr;
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::reset(iterator pos) {
  return tree.erase(pos);
}

inline Unlimited_Sparse_Row::iterator
Unlimited_Sparse_Row::reset(iterator first, iterator last) {
  if (first == last)
    return first;
  PPL_ASSERT(last != end());
  --last;
  const dimension_type j = last->first;
  PPL_ASSERT(first->first <= j);
  // We can't just compare first and last at each iteration, because last will
  // be invalidated by the first erase.
  while (first->first < j)
    first = reset(first);

  first = reset(first);

  return first;
}

inline void
Unlimited_Sparse_Row::reset(dimension_type i) {
  tree.erase(i);
}

inline void
Unlimited_Sparse_Row::reset_after(dimension_type i) {
  iterator itr = lower_bound(i);
  // This is a const reference to an internal iterator, that is kept valid.
  // If we just stored a copy, that would be invalidated by the calls to
  // reset().
  const iterator& itr_end = end();

  while (itr != itr_end)
    itr = reset(itr);
}

inline void
Unlimited_Sparse_Row::delete_element_and_shift(dimension_type i) {
  tree.erase_element_and_shift_left(i);
}

inline void
Unlimited_Sparse_Row::add_zeroes_and_shift(dimension_type n,
                                           dimension_type i) {
  tree.increase_keys_after(i, n);
}

inline Coefficient&
Unlimited_Sparse_Row::operator[](dimension_type i) {
  iterator itr = find_create(i);
  return itr->second;
}

inline const Coefficient&
Unlimited_Sparse_Row::operator[](dimension_type i) const {
  return get(i);
}

inline const Coefficient&
Unlimited_Sparse_Row::get(dimension_type i) const {
  if (tree.empty())
    return Coefficient_zero();
  const_iterator itr = find(i);
  if (itr != end())
    return itr->second;
  else
    return Coefficient_zero();
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Unlimited_Sparse_Row_inlines_hh)
