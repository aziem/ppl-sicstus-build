/* Checked extended arithmetic functions.
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

namespace Parma_Polyhedra_Library {

namespace Checked {

#define handle_ext(Type) (Float<Type>::fpu_related)

template <typename To_Policy, typename From_Policy,
	  typename To, typename From>
inline Result
assign_ext(To& to, const From& from, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From))
    return assign<To_Policy>(to, from, mode);
  Result r = classify<From_Policy>(from, true, true, false);
  if (r == VC_NORMAL)
    return assign<To_Policy>(to, from, mode);
  return set_special<To_Policy>(to, r);
}

template <typename To_Policy, typename From_Policy,
	  typename To, typename From>
inline Result
neg_ext(To& to, const From& x, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From))
    return neg<To_Policy>(to, x, mode);
  Result r = classify<From_Policy>(x, true, true, false);
  if (r == VC_NORMAL)
    return neg<To_Policy>(to, x, mode);
  else if (r == VC_MINUS_INFINITY)
    r = VC_PLUS_INFINITY;
  else if (r == VC_PLUS_INFINITY)
    r = VC_MINUS_INFINITY;
  return set_special<To_Policy>(to, r);
}

template <typename To_Policy, typename From_Policy,
	  typename To, typename From>
inline Result
abs_ext(To& to, const From& x, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From))
    return abs<To_Policy>(to, x, mode);
  Result r = classify<From_Policy>(x, true, true, false);
  if (r == VC_NORMAL)
    return abs<To_Policy>(to, x, mode);
  else if (r == VC_MINUS_INFINITY)
    r = VC_PLUS_INFINITY;
  return set_special<To_Policy>(to, r);
}

template <typename To_Policy, typename From1_Policy, typename From2_Policy,
	  typename To, typename From1, typename From2>
inline Result
add_ext(To& to, const From1& x, const From2& y, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From1) && handle_ext(From2))
    return add<To_Policy>(to, x, y, mode);
  Result rx;
  Result ry;
  Result r;
  if ((rx = classify<From1_Policy>(x, true, true, false)) == VC_NAN
      || (ry = classify<From2_Policy>(y, true, true, false)) == VC_NAN)
    r = VC_NAN;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return add<To_Policy>(to, x, y, mode);
  else if (rx == VC_NORMAL)
    r = ry;
  else if (ry == VC_NORMAL || rx == ry)
    r = rx;
  else
    r = V_INF_ADD_INF;
  return set_special<To_Policy>(to, r);
}
    
template <typename To_Policy, typename From1_Policy, typename From2_Policy,
	  typename To, typename From1, typename From2>
inline Result
sub_ext(To& to, const From1& x, const From2& y, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From1) && handle_ext(From2))
    return sub<To_Policy>(to, x, y, mode);
  Result rx;
  Result ry;
  Result r;
  if ((rx = classify<From1_Policy>(x, true, true, false)) == VC_NAN
      || (ry = classify<From2_Policy>(y, true, true, false)) == VC_NAN)
    r = VC_NAN;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return sub<To_Policy>(to, x, y, mode);
  else if (rx == VC_NORMAL)
    r = ry == VC_PLUS_INFINITY ? VC_MINUS_INFINITY : VC_PLUS_INFINITY;
  else if (ry == VC_NORMAL || rx != ry)
    r = rx;
  else
    r = V_INF_SUB_INF;
  return set_special<To_Policy>(to, r);
}
    
template <typename To_Policy, typename From1_Policy, typename From2_Policy,
	  typename To, typename From1, typename From2>
inline Result
mul_ext(To& to, const From1& x, const From2& y, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From1) && handle_ext(From2))
    return mul<To_Policy>(to, x, y, mode);
  Result rx;
  Result ry;
  Result r;
  if ((rx = classify<From1_Policy>(x, true, true, true)) == VC_NAN
      || (ry = classify<From2_Policy>(y, true, true, true)) == VC_NAN)
    r = VC_NAN;
  else if (!(is_special(rx) || is_special(ry)))
    return mul<To_Policy>(to, x, y, mode);
  else {
    rx = sign(rx);
    ry = sign(ry);
    if (rx == V_EQ || ry == V_EQ)
      r = V_INF_MUL_ZERO;
    else if (rx == ry)
      r = VC_PLUS_INFINITY;
    else
      r = VC_MINUS_INFINITY;
  }
  return set_special<To_Policy>(to, r);
}
    
	
template <typename To_Policy, typename From1_Policy, typename From2_Policy,
	  typename To, typename From1, typename From2>
inline Result
div_ext(To& to, const From1& x, const From2& y, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From1) && handle_ext(From2))
    return div<To_Policy>(to, x, y, mode);
  Result rx;
  Result ry;
  Result r;
  if ((rx = classify<From1_Policy>(x, true, true, true)) == VC_NAN
      || (ry = classify<From2_Policy>(y, true, true, true)) == VC_NAN)
    r = VC_NAN;
  else if (!is_special(rx)) {
    if (!is_special(ry))
      return div<To_Policy>(to, x, y, mode);
    else {
      to = 0;
      r = V_EQ;
    }
  }
  else if (!is_special(ry)) {
    if (ry == V_EQ)
      r = V_DIV_ZERO;
    else {
      rx = sign(rx);
      if (rx == ry)
	r = VC_PLUS_INFINITY;
      else
	r = VC_MINUS_INFINITY;
    }
  }
  else
    r = V_INF_DIV_INF;
  return set_special<To_Policy>(to, r);
}
    
	
template <typename To_Policy, typename From1_Policy, typename From2_Policy,
	  typename To, typename From1, typename From2>
inline Result
rem_ext(To& to, const From1& x, const From2& y, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From1) && handle_ext(From2))
    return rem<To_Policy>(to, x, y, mode);
  Result rx;
  Result ry;
  Result r;
  if ((rx = classify<From1_Policy>(x, true, true, false)) == VC_NAN
      || (ry = classify<From2_Policy>(y, true, true, false)) == VC_NAN)
    r = VC_NAN;
  else if (rx == VC_NORMAL) {
    if (ry == VC_NORMAL)
      return rem<To_Policy>(to, x, y, mode);
    else {
      to = x;
      r = V_EQ;
    }
  }
  else
    r = V_INF_MOD;
  return set_special<To_Policy>(to, r);
}
    
template <typename To_Policy, typename From_Policy,
	  typename To, typename From>
inline Result
sqrt_ext(To& to, const From& x, const Rounding& mode) {
  if (handle_ext(To) && handle_ext(From))
    return sqrt<To_Policy>(to, x, mode);
  Result r = classify<From_Policy>(x, true, true, false);
  if (r == VC_NORMAL)
    return sqrt<To_Policy>(to, x, mode);
  else if (r == VC_NAN)
    r = VC_NAN;
  else if (r == VC_MINUS_INFINITY)
    r = V_SQRT_NEG;
  else
    r = VC_PLUS_INFINITY;
  return set_special<To_Policy>(to, r);
}

template <typename To_Policy, typename From1_Policy, typename From2_Policy,
	  typename To, typename From1, typename From2>
inline Result
gcd_ext(To& to, const From1& x, const From2& y, const Rounding& mode) {
  Result rx;
  Result ry;
  Result r;
  if ((rx = classify<From1_Policy>(x, true, true, false)) == VC_NAN
      || (ry = classify<From2_Policy>(y, true, true, false)) == VC_NAN)
    r = VC_NAN;
  else if (rx == VC_NORMAL) {
    if (ry == VC_NORMAL)
      return gcd<To_Policy>(to, x, y, mode);
    else {
      to = x;
      r = V_EQ;
    }
  }
  else if (ry == VC_NORMAL) {
    to = y;
    r = V_EQ;
  }
  else
    r = VC_PLUS_INFINITY;
  return set_special<To_Policy>(to, r);
}
    
template <typename To_Policy, typename From1_Policy, typename From2_Policy,
	  typename To, typename From1, typename From2>
inline Result
lcm_ext(To& to, const From1& x, const From2& y, const Rounding& mode) {
  Result rx;
  Result ry;
  Result r;
  if ((rx = classify<From1_Policy>(x, true, true, false)) == VC_NAN
      || (ry = classify<From2_Policy>(y, true, true, false)) == VC_NAN)
    r = VC_NAN;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return lcm<To_Policy>(to, x, y, mode);
  else
    r = VC_PLUS_INFINITY;
  return set_special<To_Policy>(to, r);
}

template <typename Policy1, typename Policy2,
	  typename Type1, typename Type2>
inline Result
cmp_ext(const Type1& x, const Type2& y) {
  if (handle_ext(Type1) && handle_ext(Type2))
    return cmp<Policy1>(x, y);
  Result rx;
  Result ry;
  if ((rx = classify<Policy1>(x, true, true, false)) == VC_NAN
      || (ry = classify<Policy2>(y, true, true, false)) == VC_NAN)
    return V_UNORD_COMP;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return cmp<Policy1>(x, y);
  else if (rx == ry)
    return V_EQ;
  else if (rx == VC_MINUS_INFINITY || ry == VC_PLUS_INFINITY)
    return V_LT;
  else
    return V_GT;
}

template <typename Policy1, typename Policy2,
	  typename Type1, typename Type2>
inline bool
lt_ext(const Type1& x, const Type2& y) {
  if (handle_ext(Type1) && handle_ext(Type2))
    return x < y;
  Result rx;
  Result ry;
  if ((rx = classify<Policy1>(x, true, true, false)) == VC_NAN
      || (ry = classify<Policy2>(y, true, true, false)) == VC_NAN)
    return false;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return x < y;
  else if (rx == ry)
    return false;
  return (rx == VC_MINUS_INFINITY || ry == VC_PLUS_INFINITY);
}

template <typename Policy1, typename Policy2,
	  typename Type1, typename Type2>
inline bool
gt_ext(const Type1& x, const Type2& y) {
  if (handle_ext(Type1) && handle_ext(Type2))
    return x > y;
  Result rx;
  Result ry;
  if ((rx = classify<Policy1>(x, true, true, false)) == VC_NAN
      || (ry = classify<Policy2>(y, true, true, false)) == VC_NAN)
    return false;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return x > y;
  else if (rx == ry)
    return false;
  return (rx == VC_PLUS_INFINITY || ry == VC_MINUS_INFINITY);
}

template <typename Policy1, typename Policy2,
	  typename Type1, typename Type2>
inline bool
le_ext(const Type1& x, const Type2& y) {
  if (handle_ext(Type1) && handle_ext(Type2))
    return x <= y;
  Result rx;
  Result ry;
  if ((rx = classify<Policy1>(x, true, true, false)) == VC_NAN
      || (ry = classify<Policy2>(y, true, true, false)) == VC_NAN)
    return false;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return x <= y;
  return (rx == VC_MINUS_INFINITY || ry == VC_PLUS_INFINITY);
}

template <typename Policy1, typename Policy2,
	  typename Type1, typename Type2>
inline bool
ge_ext(const Type1& x, const Type2& y) {
  if (handle_ext(Type1) && handle_ext(Type2))
    return x >= y;
  Result rx;
  Result ry;
  if ((rx = classify<Policy1>(x, true, true, false)) == VC_NAN
      || (ry = classify<Policy2>(y, true, true, false)) == VC_NAN)
    return false;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return x >= y;
  return (rx == VC_PLUS_INFINITY || ry == VC_MINUS_INFINITY);
}

template <typename Policy1, typename Policy2,
	  typename Type1, typename Type2>
inline bool
eq_ext(const Type1& x, const Type2& y) {
  if (handle_ext(Type1) && handle_ext(Type2))
    return x == y;
  Result rx;
  Result ry;
  if ((rx = classify<Policy1>(x, true, true, false)) == VC_NAN
      || (ry = classify<Policy2>(y, true, true, false)) == VC_NAN)
    return false;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return x == y;
  return rx == ry;
}

template <typename Policy1, typename Policy2,
	  typename Type1, typename Type2>
inline bool
ne_ext(const Type1& x, const Type2& y) {
  if (handle_ext(Type1) && handle_ext(Type2))
    return x != y;
  Result rx;
  Result ry;
  if ((rx = classify<Policy1>(x, true, true, false)) == VC_NAN
      || (ry = classify<Policy2>(y, true, true, false)) == VC_NAN)
    return true;
  else if (rx == VC_NORMAL && ry == VC_NORMAL)
    return x != y;
  return rx != ry;
}

template <typename Policy, typename Type>
inline Result
print_ext(std::ostream& os, const Type& x, const Numeric_Format& format, const Rounding& mode) {
  if (handle_ext(Type))
    return print<Policy>(os, x, format, mode);
  Result rx = classify<Policy>(x, true, true, false);
  switch (rx) {
  case VC_NORMAL:
    return print<Policy>(os, x, format, mode);
    break;
  case VC_MINUS_INFINITY:
    os << "-inf";
    break;
  case VC_PLUS_INFINITY:
    os << "inf";
    break;
  default:
    os << "nan";
    break;
  }
  return rx;
}

} // namespace Checked

} // namespace Parma_Polyhedra_Library
