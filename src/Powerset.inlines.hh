/* Powerset class implementation: inline functions.
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

#ifndef PPL_Powerset_inlines_hh
#define PPL_Powerset_inlines_hh 1

#include <algorithm>
#include <cassert>

namespace Parma_Polyhedra_Library {

template <typename D>
inline
Powerset<D>::omega_iterator::omega_iterator()
  : base() {
}

template <typename D>
inline
Powerset<D>::omega_iterator::omega_iterator(const omega_iterator& y)
  : base(y.base) {
}

template <typename D>
inline
Powerset<D>::omega_iterator::omega_iterator(const Base& b)
  : base(b) {
}

template <typename D>
inline typename Powerset<D>::omega_iterator::reference
Powerset<D>::omega_iterator::operator*() const {
  return *base;
}

template <typename D>
inline typename Powerset<D>::omega_iterator::pointer
Powerset<D>::omega_iterator::operator->() const {
  return &*base;
}

template <typename D>
inline typename Powerset<D>::omega_iterator&
Powerset<D>::omega_iterator::operator++() {
  ++base;
  return *this;
}

template <typename D>
inline typename Powerset<D>::omega_iterator
Powerset<D>::omega_iterator::operator++(int) {
  omega_iterator tmp = *this;
  operator++();
  return tmp;
}

template <typename D>
inline typename Powerset<D>::omega_iterator&
Powerset<D>::omega_iterator::operator--() {
  --base;
  return *this;
}

template <typename D>
inline typename Powerset<D>::omega_iterator
Powerset<D>::omega_iterator::operator--(int) {
  omega_iterator tmp = *this;
  operator--();
  return tmp;
}

template <typename D>
inline bool
Powerset<D>::omega_iterator::operator==(const omega_iterator& y) const {
  return base == y.base;
}

template <typename D>
inline bool
Powerset<D>::omega_iterator::operator!=(const omega_iterator& y) const {
  return !operator==(y);
}

template <typename D>
inline
Powerset<D>::omega_const_iterator::omega_const_iterator()
  : base() {
}

template <typename D>
inline
Powerset<D>
::omega_const_iterator::omega_const_iterator(const omega_const_iterator& y)
  : base(y.base) {
}

template <typename D>
inline
Powerset<D>::omega_const_iterator::omega_const_iterator(const Base& b)
  : base(b) {
}

template <typename D>
inline typename Powerset<D>::omega_const_iterator::reference
Powerset<D>::omega_const_iterator::operator*() const {
  return *base;
}

template <typename D>
inline typename Powerset<D>::omega_const_iterator::pointer
Powerset<D>::omega_const_iterator::operator->() const {
  return &*base;
}

template <typename D>
inline typename Powerset<D>::omega_const_iterator&
Powerset<D>::omega_const_iterator::operator++() {
  ++base;
  return *this;
}

template <typename D>
inline typename Powerset<D>::omega_const_iterator
Powerset<D>::omega_const_iterator::operator++(int) {
  omega_const_iterator tmp = *this;
  operator++();
  return tmp;
}

template <typename D>
inline typename Powerset<D>::omega_const_iterator&
Powerset<D>::omega_const_iterator::operator--() {
  --base;
  return *this;
}

template <typename D>
inline typename Powerset<D>::omega_const_iterator
Powerset<D>::omega_const_iterator::operator--(int) {
  omega_const_iterator tmp = *this;
  operator--();
  return tmp;
}

template <typename D>
inline bool
Powerset<D>
::omega_const_iterator::operator==(const omega_const_iterator& y) const {
  return base == y.base;
}

template <typename D>
inline bool
Powerset<D>
::omega_const_iterator::operator!=(const omega_const_iterator& y) const {
  return !operator==(y);
}

template <typename D>
inline
Powerset<D>
::omega_const_iterator::omega_const_iterator(const omega_iterator& y)
  : base(y.base) {
}

/*! \relates Powerset::omega_const_iterator */
template <typename D>
inline bool
operator==(const typename Powerset<D>::omega_iterator& x,
	   const typename Powerset<D>::omega_const_iterator& y) {
  return Powerset<D>::omega_const_iterator(x).operator==(y);
}

/*! \relates Powerset::omega_const_iterator */
template <typename D>
inline bool
operator!=(const typename Powerset<D>::omega_iterator& x,
	   const typename Powerset<D>::omega_const_iterator& y) {
  return !(x == y);
}

template <typename D>
inline typename Powerset<D>::iterator
Powerset<D>::begin() {
  return sequence.begin();
}

template <typename D>
inline typename Powerset<D>::iterator
Powerset<D>::end() {
  return sequence.end();
}

template <typename D>
inline typename Powerset<D>::const_iterator
Powerset<D>::begin() const {
  return sequence.begin();
}

template <typename D>
inline typename Powerset<D>::const_iterator
Powerset<D>::end() const {
  return sequence.end();
}

template <typename D>
inline typename Powerset<D>::reverse_iterator
Powerset<D>::rbegin() {
  return reverse_iterator(end());
}

template <typename D>
inline typename Powerset<D>::reverse_iterator
Powerset<D>::rend() {
  return reverse_iterator(begin());
}

template <typename D>
inline typename Powerset<D>::const_reverse_iterator
Powerset<D>::rbegin() const {
  return const_reverse_iterator(end());
}

template <typename D>
inline typename Powerset<D>::const_reverse_iterator
Powerset<D>::rend() const {
  return const_reverse_iterator(begin());
}

template <typename D>
inline typename Powerset<D>::size_type
Powerset<D>::size() const {
  return sequence.size();
}

template <typename D>
inline bool
Powerset<D>::empty() const {
  return sequence.empty();
}

template <typename D>
inline typename Powerset<D>::iterator
Powerset<D>::drop_disjunct(iterator position) {
  return sequence.erase(position.base);
}

template <typename D>
inline void
Powerset<D>::drop_disjuncts(iterator first, iterator last) {
  sequence.erase(first.base, last.base);
}

template <typename D>
inline void
Powerset<D>::clear() {
  sequence.clear();
}

template <typename D>
inline
Powerset<D>::Powerset(const Powerset& y)
  : sequence(y.sequence), reduced(y.reduced) {
}

template <typename D>
inline Powerset<D>&
Powerset<D>::operator=(const Powerset& y) {
  sequence = y.sequence;
  reduced = y.reduced;
  return *this;
}

template <typename D>
inline void
Powerset<D>::swap(Powerset& y) {
  std::swap(sequence, y.sequence);
  std::swap(reduced, y.reduced);
}

template <typename D>
inline
Powerset<D>::Powerset()
  : sequence(), reduced(true) {
}

template <typename D>
inline
Powerset<D>::Powerset(const D& d)
  : sequence(), reduced(true) {
  if (!d.is_bottom())
    sequence.push_back(d);
  assert(OK());
}

template <typename D>
inline
Powerset<D>::~Powerset() {
}

template <typename D>
inline void
Powerset<D>::add_non_bottom_disjunct(const D& d) {
  assert(!d.is_bottom());
  add_non_bottom_disjunct(d, begin(), end());
}

template <typename D>
inline void
Powerset<D>::add_disjunct(const D& d) {
  if (!d.is_bottom())
    add_non_bottom_disjunct(d);
}

/*! \relates Powerset */
template <typename D>
inline
bool operator!=(const Powerset<D>& x, const Powerset<D>& y) {
  return !(x == y);
}

template <typename D>
inline bool
Powerset<D>::is_top() const {
  // Must perform omega-reduction for correctness.
  omega_reduce();
  const_iterator xi = begin();
  const_iterator x_end = end();
  return xi != x_end && xi->is_top() && ++xi == x_end;
}

template <typename D>
inline bool
Powerset<D>::is_bottom() const {
  // Must perform omega-reduction for correctness.
  omega_reduce();
  return empty();
}

template <typename D>
inline void
Powerset<D>::collapse() {
  if (!empty())
    collapse(sequence.begin());
}

template <typename D>
inline void
Powerset<D>::meet_assign(const Powerset& y) {
  pairwise_apply_assign(y, std::mem_fun_ref(&D::meet_assign));
}

template <typename D>
inline void
Powerset<D>::upper_bound_assign(const Powerset& y) {
  least_upper_bound_assign(y);
}

template <typename D>
inline memory_size_type
Powerset<D>::total_memory_in_bytes() const {
  return sizeof(*this) + external_memory_in_bytes();
}

} // namespace Parma_Polyhedra_Library


namespace std {

/*! \relates Parma_Polyhedra_Library::Powerset */
template <typename D>
inline void
swap(Parma_Polyhedra_Library::Powerset<D>& x,
     Parma_Polyhedra_Library::Powerset<D>& y) {
  x.swap(y);
}

} // namespace std

#endif // !defined(PPL_Powerset_inlines_hh)
