/* Checked_Number class implementation: inline functions.
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

#ifndef PPL_Checked_Number_inlines_hh
#define PPL_Checked_Number_inlines_hh 1

#include <stdexcept>

namespace Parma_Polyhedra_Library {

inline void
Checked_Number_Default_Policy::handle_result(Result r) {
  if (is_special(r) || (round_inexact && r != V_EQ))
    throw_result_exception(r);
}

inline void
Extended_Number_Policy::handle_result(Result r) {
  switch (r) {
  case V_EQ:
  case V_LT:
  case V_GT:
  case V_GE:
  case V_LE:
  case VC_MINUS_INFINITY:
  case VC_PLUS_INFINITY:
    break;
  default:
    throw_result_exception(r);
    break;
  }
}

template <typename T, typename Policy>
inline
Checked_Number<T, Policy>::Checked_Number()
 : v(0) {
}

#if 0
template <typename To, typename To_Policy>
template <typename From, typename From_Policy>
inline
Checked_Number<To, To_Policy>::Checked_Number(const Checked_Number<From, From_Policy>& y) {
  To_Policy::handle_result(Checked::assign_ext<To_Policy, From_Policy>(v, y.raw_value(), Rounding::CURRENT));
}
#endif

#define DEF_CTOR(type) \
template <typename T, typename Policy> \
inline \
Checked_Number<T, Policy>::Checked_Number(const type y) { \
  Policy::handle_result(Checked::assign<Policy>(v, y, Rounding::CURRENT)); \
}

DEF_CTOR(signed char)
DEF_CTOR(short)
DEF_CTOR(int)
DEF_CTOR(long)
DEF_CTOR(long long)
DEF_CTOR(unsigned char)
DEF_CTOR(unsigned short)
DEF_CTOR(unsigned int)
DEF_CTOR(unsigned long)
DEF_CTOR(unsigned long long)
DEF_CTOR(float)
DEF_CTOR(double)
#if CXX_SUPPORTS_LONG_DOUBLE
DEF_CTOR(long double)
#endif
DEF_CTOR(mpq_class&)
DEF_CTOR(mpz_class&)
DEF_CTOR(c_string)

#undef DEF_CTOR

template <typename T, typename Policy>
inline
Checked_Number<T, Policy>::operator T() const {
  if (Policy::convertible)
    return v;
}

template <typename T, typename Policy>
inline T&
Checked_Number<T, Policy>::raw_value() {
  return v;
}

template <typename T, typename Policy>
inline const T&
Checked_Number<T, Policy>::raw_value() const {
  return v;
}

/*! \relates Checked_Number */
template <typename T, typename Policy>
inline const T&
raw_value(const Checked_Number<T, Policy>& x) {
  return x.raw_value();
}

/*! \relates Checked_Number */
template <typename T, typename Policy>
inline T&
raw_value(Checked_Number<T, Policy>& x) {
  return x.raw_value();
}

template <typename T, typename Policy>
inline bool
Checked_Number<T, Policy>::OK() const {
  return true;
}

template <typename T, typename Policy>
inline Result
Checked_Number<T, Policy>::classify(bool nan, bool inf, bool sign) const {
  return Checked::classify<Policy>(v, nan, inf, sign);
}

/*! \relates Checked_Number */
template <typename T, typename Policy>
size_t
total_memory_in_bytes(const Checked_Number<T, Policy>& x) {
  // FIXME
  return sizeof(x);
}

/*! \relates Checked_Number */
template <typename T, typename Policy>
size_t
external_memory_in_bytes(const Checked_Number<T, Policy>&) {
  return 0;
}

template <typename To, typename To_Policy>
inline Result
Checked_Number<To, To_Policy>::assign(const Minus_Infinity&, const Rounding&) {
  return Checked::set_special<To_Policy>(v, VC_MINUS_INFINITY);
}
template <typename To, typename To_Policy>
inline Result
Checked_Number<To, To_Policy>::assign(const Plus_Infinity&, const Rounding&) {
  return Checked::set_special<To_Policy>(v, VC_PLUS_INFINITY);
}
template <typename To, typename To_Policy>
inline Result
Checked_Number<To, To_Policy>::assign(const Not_A_Number&, const Rounding&) {
  return Checked::set_special<To_Policy>(v, VC_NAN);
}

template <typename To, typename To_Policy>
template <typename From>
inline Result
Checked_Number<To, To_Policy>::assign(const From& x, const Rounding& mode) {
  return Checked::assign<To_Policy>(v, x, mode);
}
template <typename To, typename To_Policy>
template <typename From, typename From_Policy>
inline Result
Checked_Number<To, To_Policy>::assign(const Checked_Number<From, From_Policy>& x, const Rounding& mode) {
  return Checked::assign_ext<To_Policy, From_Policy>(v, x.raw_value(), mode);
}

#define FUNC1(name, func) \
template <typename To, typename To_Policy> \
template <typename From> \
inline Result \
Checked_Number<To, To_Policy>::name(const From& x, const Rounding& mode) { \
  return Checked::func<To_Policy, Checked::Transparent_Policy>(v, x, mode); \
} \
template <typename To, typename To_Policy> \
template <typename From, typename From_Policy> \
inline Result \
Checked_Number<To, To_Policy>::name(const Checked_Number<From, From_Policy>& x, const Rounding& mode) { \
  return Checked::func<To_Policy, From_Policy>(v, x.raw_value(), mode); \
}

FUNC1(assign_neg, neg_ext)
FUNC1(assign_abs, abs_ext)
FUNC1(assign_sqrt, sqrt_ext)

#undef FUNC1

#define FUNC2(name, func) \
template <typename To, typename To_Policy> \
template <typename From1, \
	  typename From2> \
inline Result \
Checked_Number<To, To_Policy>::name(const From1& x, const From2& y, const Rounding& mode) { \
  return Checked::func<To_Policy, Checked::Transparent_Policy, Checked::Transparent_Policy>(v, x, y, mode); \
} \
template <typename To, typename To_Policy> \
template <typename From1, \
	  typename From2, typename Policy2> \
inline Result \
Checked_Number<To, To_Policy>::name(const From1& x, const Checked_Number<From2, Policy2>& y, const Rounding& mode) { \
  return Checked::func<To_Policy, Checked::Transparent_Policy, Policy2>(v, x, y.raw_value(), mode); \
} \
template <typename To, typename To_Policy> \
template <typename From1, typename Policy1, \
	  typename From2> \
inline Result \
Checked_Number<To, To_Policy>::name(const Checked_Number<From1, Policy1>& x, const From2& y, const Rounding& mode) { \
  return Checked::func<To_Policy, Policy1, Checked::Transparent_Policy>(v, x.raw_value(), y, mode); \
} \
template <typename To, typename To_Policy> \
template <typename From1, typename Policy1, \
	  typename From2, typename Policy2> \
inline Result \
Checked_Number<To, To_Policy>::name(const Checked_Number<From1, Policy1>& x, const Checked_Number<From2, Policy2>& y, const Rounding& mode) { \
  return Checked::func<To_Policy, Policy1, Policy2>(v, x.raw_value(), y.raw_value(), mode); \
}

FUNC2(assign_add, add_ext)
FUNC2(assign_sub, sub_ext)
FUNC2(assign_mul, mul_ext)
FUNC2(assign_div, div_ext)
FUNC2(assign_rem, rem_ext)
FUNC2(assign_gcd, gcd_ext)
FUNC2(assign_lcm, lcm_ext)
FUNC2(assign_add_mul, add_mul_ext)
FUNC2(assign_sub_mul, sub_mul_ext)

#undef FUNC2

#define DEF_INCREMENT(f, fun) \
template <typename T, typename Policy> \
inline Checked_Number<T, Policy>& \
Checked_Number<T, Policy>::f() { \
  Policy::handle_result(fun(*this, T(1), Rounding::CURRENT)); \
  return *this; \
} \
template <typename T, typename Policy> \
inline Checked_Number<T, Policy> \
Checked_Number<T, Policy>::f(int) {\
  T r = v;\
  Policy::handle_result(fun(*this, T(1), Rounding::CURRENT));\
  return r;\
}

DEF_INCREMENT(operator ++, assign_add)
DEF_INCREMENT(operator --, assign_sub)

#undef DEF_INCREMENT

template <typename T, typename Policy>
inline void
Checked_Number<T, Policy>::swap(Checked_Number<T, Policy>& y) {
  std::swap(v, y.raw_value());
}

template <typename To, typename To_Policy>
template <typename From, typename From_Policy>
inline Checked_Number<To, To_Policy>&
Checked_Number<To, To_Policy>::operator=(const Checked_Number<From, From_Policy>& y) {
  To_Policy::handle_result(assign(y, Rounding::CURRENT));
  return *this;
}
template <typename To, typename To_Policy>
template <typename From>
inline Checked_Number<To, To_Policy>&
Checked_Number<To, To_Policy>::operator=(const From& y) {
  To_Policy::handle_result(assign(y, Rounding::CURRENT));
  return *this;
}

#define DEF_BINARY_OP_ASSIGN(f, fun) \
template <typename T, typename To_Policy> \
template <typename From_Policy> \
inline Checked_Number<T, To_Policy>& \
Checked_Number<T, To_Policy>::f(const Checked_Number<T, From_Policy>& y) { \
  To_Policy::handle_result(fun(*this, y, Rounding::CURRENT)); \
  return *this; \
} \
template <typename T, typename To_Policy> \
inline Checked_Number<T, To_Policy>& \
Checked_Number<T, To_Policy>::f(const T& y) { \
  To_Policy::handle_result(fun(*this, y, Rounding::CURRENT)); \
  return *this; \
} \
template <typename To, typename To_Policy> \
template <typename From, typename From_Policy> \
inline Checked_Number<To, To_Policy>& \
Checked_Number<To, To_Policy>::f(const Checked_Number<From, From_Policy>& y) { \
  Checked_Number<To, To_Policy> cy(y); \
  To_Policy::handle_result(fun(*this, cy, Rounding::CURRENT)); \
  return *this; \
} \
template <typename To, typename To_Policy> \
template <typename From> \
inline Checked_Number<To, To_Policy>& \
Checked_Number<To, To_Policy>::f(const From& y) { \
  Checked_Number<To, To_Policy> cy(y); \
  To_Policy::handle_result(fun(*this, cy, Rounding::CURRENT)); \
  return *this; \
}

DEF_BINARY_OP_ASSIGN(operator +=, assign_add)
DEF_BINARY_OP_ASSIGN(operator -=, assign_sub)
DEF_BINARY_OP_ASSIGN(operator *=, assign_mul)
DEF_BINARY_OP_ASSIGN(operator /=, assign_div)
DEF_BINARY_OP_ASSIGN(operator %=, assign_rem)

#undef DEF_BINARY_OP_ASSIGN

#define DEF_BINARY_OP_TYPE(f, fun, Type) \
template <typename T, typename Policy> \
inline Checked_Number<T, Policy> \
f(const Type x, const Checked_Number<T, Policy>& y) { \
  Checked_Number<T, Policy> r(x); \
  Policy::handle_result(r.fun(r, y, Rounding::CURRENT)); \
  return r; \
} \
template <typename T, typename Policy> \
inline Checked_Number<T, Policy> \
f(const Checked_Number<T, Policy>& x, const Type y) { \
  Checked_Number<T, Policy> r(y); \
  Policy::handle_result(r.fun(x, r, Rounding::CURRENT)); \
  return r; \
}

#define DEF_BINARY_OP(f, fun) \
template <typename T, typename Policy> \
inline Checked_Number<T, Policy> \
f(const Checked_Number<T, Policy>& x, const Checked_Number<T, Policy>& y) { \
  Checked_Number<T, Policy> r; \
  Policy::handle_result(r.fun(x, y, Rounding::CURRENT)); \
  return r; \
} \
template <typename T1, typename Policy1, \
	  typename T2, typename Policy2> \
inline typename Checked_Pair<T1, Policy1, T2, Policy2>::Checked_Result \
f(const Checked_Number<T1, Policy1>& x, const Checked_Number<T2, Policy2>& y) { \
  typedef typename Checked_Pair<T1, Policy1, T2, Policy2>::Checked_Result Res; \
  Res r; \
  Res rx; \
  Res ry; \
  Res::handle_result(rx.assign(x, Rounding::CURRENT)); \
  Res::handle_result(ry.assign(y, Rounding::CURRENT)); \
  Res::handle_result(r.fun(rx, ry, Rounding::CURRENT)); \
  return r; \
} \
DEF_BINARY_OP_TYPE(f, fun, signed char) \
DEF_BINARY_OP_TYPE(f, fun, short) \
DEF_BINARY_OP_TYPE(f, fun, int) \
DEF_BINARY_OP_TYPE(f, fun, long) \
DEF_BINARY_OP_TYPE(f, fun, long long) \
DEF_BINARY_OP_TYPE(f, fun, unsigned char) \
DEF_BINARY_OP_TYPE(f, fun, unsigned short) \
DEF_BINARY_OP_TYPE(f, fun, unsigned int) \
DEF_BINARY_OP_TYPE(f, fun, unsigned long) \
DEF_BINARY_OP_TYPE(f, fun, unsigned long long) \
DEF_BINARY_OP_TYPE(f, fun, float) \
DEF_BINARY_OP_TYPE(f, fun, double) \
DEF_BINARY_OP_TYPE(f, fun, long double) \
DEF_BINARY_OP_TYPE(f, fun, mpz_class&) \
DEF_BINARY_OP_TYPE(f, fun, mpq_class&)

DEF_BINARY_OP(operator +, assign_add)
DEF_BINARY_OP(operator -, assign_sub)
DEF_BINARY_OP(operator *, assign_mul)
DEF_BINARY_OP(operator /, assign_div)
DEF_BINARY_OP(operator %, assign_rem)

#undef DEF_BINARY_OP_TYPE
#undef DEF_BINARY_OP

#define DEF_COMPARE_TYPE(f, fun, Type) \
template <typename From, typename From_Policy> \
inline bool \
f(const Type x, const Checked_Number<From, From_Policy>& y) { \
  return Checked::fun<Checked::Transparent_Policy, From_Policy>(x, y.raw_value()); \
} \
template <typename From, typename From_Policy> \
inline bool \
f(const Checked_Number<From, From_Policy>& x, const Type y) { \
  return Checked::fun<From_Policy, Checked::Transparent_Policy>(x.raw_value(), y); \
}

#define DEF_COMPARE(f, fun) \
template <typename T1, typename Policy1, \
          typename T2, typename Policy2> \
inline bool \
f(const Checked_Number<T1, Policy1>& x, const Checked_Number<T2, Policy2>& y) { \
  return Checked::fun<Policy1, Policy2>(x.raw_value(), y.raw_value()); \
} \
DEF_COMPARE_TYPE(f, fun, signed char) \
DEF_COMPARE_TYPE(f, fun, short) \
DEF_COMPARE_TYPE(f, fun, int) \
DEF_COMPARE_TYPE(f, fun, long) \
DEF_COMPARE_TYPE(f, fun, long long) \
DEF_COMPARE_TYPE(f, fun, unsigned char) \
DEF_COMPARE_TYPE(f, fun, unsigned short) \
DEF_COMPARE_TYPE(f, fun, unsigned int) \
DEF_COMPARE_TYPE(f, fun, unsigned long) \
DEF_COMPARE_TYPE(f, fun, unsigned long long) \
DEF_COMPARE_TYPE(f, fun, float) \
DEF_COMPARE_TYPE(f, fun, double) \
DEF_COMPARE_TYPE(f, fun, long double) \
DEF_COMPARE_TYPE(f, fun, mpz_class&) \
DEF_COMPARE_TYPE(f, fun, mpq_class&)


DEF_COMPARE(operator ==, eq_ext)
DEF_COMPARE(operator !=, ne_ext)
DEF_COMPARE(operator >=, ge_ext)
DEF_COMPARE(operator >, gt_ext)
DEF_COMPARE(operator <=, le_ext)
DEF_COMPARE(operator <, lt_ext)

#undef DEF_COMPARE_TYPE
#undef DEF_COMPARE

/*! \relates Checked_Number */
template <typename T, typename Policy>
inline Checked_Number<T, Policy>
operator+(const Checked_Number<T, Policy>& x) {
  return x;
}

/*! \relates Checked_Number */
template <typename T, typename Policy>
inline Checked_Number<T, Policy>
operator-(const Checked_Number<T, Policy>& x) {
  Checked_Number<T, Policy> r;
  Policy::handle_result(r.assign_neg(x, Rounding::CURRENT));
  return r;
}

#define DEF_ASSIGN_FUN2_1(f, fun) \
template <typename T, typename Policy> \
inline void \
f(Checked_Number<T, Policy>& x) { \
  Policy::handle_result(x.fun(x, Rounding::CURRENT)); \
}

#define DEF_ASSIGN_FUN2_2(f, fun) \
template <typename T, typename Policy> \
inline void \
f(Checked_Number<T, Policy>& x, const Checked_Number<T, Policy>& y) { \
  Policy::handle_result(x.fun(y, Rounding::CURRENT)); \
}

#define DEF_ASSIGN_FUN3_2(f, fun) \
template <typename T, typename Policy> \
inline void \
f(Checked_Number<T, Policy>& x, const Checked_Number<T, Policy>& y) { \
  Policy::handle_result(x.fun(x, y, Rounding::CURRENT)); \
}

#define DEF_ASSIGN_FUN3_3(f, fun) \
template <typename T, typename Policy> \
inline void \
f(Checked_Number<T, Policy>& x, const Checked_Number<T, Policy>& y, const Checked_Number<T, Policy>& z) { \
  Policy::handle_result(x.fun(y, z, Rounding::CURRENT)); \
}

DEF_ASSIGN_FUN2_1(sqrt_assign, assign_sqrt)
DEF_ASSIGN_FUN2_2(sqrt_assign, assign_sqrt)

DEF_ASSIGN_FUN2_1(negate, assign_neg)
DEF_ASSIGN_FUN2_2(negate, assign_neg)

DEF_ASSIGN_FUN3_2(exact_div_assign, assign_div)
DEF_ASSIGN_FUN3_3(exact_div_assign, assign_div)

DEF_ASSIGN_FUN3_3(add_mul_assign, assign_add_mul)

DEF_ASSIGN_FUN3_3(sub_mul_assign, assign_sub_mul)

DEF_ASSIGN_FUN3_2(gcd_assign, assign_gcd)
DEF_ASSIGN_FUN3_3(gcd_assign, assign_gcd)

DEF_ASSIGN_FUN3_2(lcm_assign, assign_lcm)
DEF_ASSIGN_FUN3_3(lcm_assign, assign_lcm)

#undef DEF_ASSIGN_FUN2_1
#undef DEF_ASSIGN_FUN2_2
#undef DEF_ASSIGN_FUN3_2
#undef DEF_ASSIGN_FUN3_3

/*! \relates Checked_Number */
template <typename T, typename Policy>
inline int
sgn(const Checked_Number<T, Policy>& x) {
  Result r = x.classify(false, false, true);
  switch (r) {
  case V_LT:
    return -1;
  case V_EQ:
    return 0;
  case V_GT:
    return 1;
  default:
    throw(0);
  }
}

/*! \relates Checked_Number */
template <typename T1, typename Policy1,
	  typename T2, typename Policy2>
inline int
cmp(const Checked_Number<T1, Policy1>& x,
    const Checked_Number<T2, Policy2>& y) {
  Result r = Checked::cmp_ext<Policy1, Policy2>(x.raw_value(), y.raw_value());
  switch (r) {
  case V_LT:
    return -1;
  case V_EQ:
    return 0;
  case V_GT:
    return 1;
  default:
    throw(0);
  }
}

/*! \relates Checked_Number */
template <typename T, typename Policy>
inline std::ostream&
operator<<(std::ostream& os, const Checked_Number<T, Policy>& x) {
  Policy::handle_result(Checked::print_ext<Policy>(os, x.raw_value(), Numeric_Format(), Rounding::CURRENT));
  return os;
}

/*! \relates Checked_Number */
template <typename T, typename Policy>
inline std::istream& operator>>(std::istream& is, Checked_Number<T, Policy>& x) {
  Policy::handle_result(Checked::input_ext<Policy>(is, x.raw_value(), Rounding::CURRENT));
  return is;
}

template <typename T, typename Policy>
inline void
Checked_Number<T, Policy>::save_rounding(const Rounding& mode, Rounding_State& current) {
  mode.save<T>(current);
}

template <typename T, typename Policy>
inline void
Checked_Number<T, Policy>::restore_rounding(const Rounding_State& state, const Rounding& current) {
  current.restore<T>(state);
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Checked_Number_inlines_hh)
