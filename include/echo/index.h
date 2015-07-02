#pragma once

#include <echo/htl.h>
#include <type_traits>

namespace echo {

using index_t = long;

//------------------------------------------------------------------------------
// StaticIndex
//------------------------------------------------------------------------------
// template <index_t I>
// struct StaticIndex : std::integral_constant<index_t, I> {};
template<index_t I>
using StaticIndex = htl::integral_constant<index_t, I>;

//------------------------------------------------------------------------------
// ParseDigits
//------------------------------------------------------------------------------
namespace detail {

template <index_t Value, char...>
struct ParseDigitsImpl {
  static constexpr index_t value = Value;
};

template <index_t Value, char First, char... Rest>
struct ParseDigitsImpl<Value, First, Rest...> {
  static constexpr index_t value =
      ParseDigitsImpl<10 * Value + (First - '0'), Rest...>::value;
};

template <char First, char... Rest>
struct ParseDigits {
  static constexpr index_t value = ParseDigitsImpl<0, First, Rest...>::value;
};

}  // end namespace detail

//------------------------------------------------------------------------------
// index - literal
//------------------------------------------------------------------------------
template <char... Chars>
constexpr auto operator"" _index() {
  return StaticIndex<detail::ParseDigits<Chars...>::value>{};
}

//------------------------------------------------------------------------------
// arithmetic operations
//------------------------------------------------------------------------------
#define MAKE_COMPARISON_OPERATOR(OPERATOR)                   \
  template <index_t Lhs, index_t Rhs>                        \
  constexpr std::integral_constant<bool, (Lhs OPERATOR Rhs)> \
  operator OPERATOR(const StaticIndex<Lhs>& lhs,             \
                    const StaticIndex<Rhs>& rhs) {           \
    return {};                                               \
  }

MAKE_COMPARISON_OPERATOR(< )
MAKE_COMPARISON_OPERATOR(> )
MAKE_COMPARISON_OPERATOR(<= )
MAKE_COMPARISON_OPERATOR(>= )
MAKE_COMPARISON_OPERATOR(== )

#undef MAKE_COMPARISON_OPERATOR

#define MAKE_ARITHMETIC_OPERATOR(OPERATOR)                        \
  template <index_t Lhs, index_t Rhs>                             \
  constexpr StaticIndex<(Lhs OPERATOR Rhs)> operator OPERATOR(    \
      const StaticIndex<Lhs>& lhs, const StaticIndex<Rhs>& rhs) { \
    return {};                                                    \
  }

MAKE_ARITHMETIC_OPERATOR(+)
MAKE_ARITHMETIC_OPERATOR(-)
MAKE_ARITHMETIC_OPERATOR(*)
MAKE_ARITHMETIC_OPERATOR(/ )
MAKE_ARITHMETIC_OPERATOR(% )

#undef MAKE_ARITHMETIC_OPERATOR

#define MAKE_UNARY_ARITHMETIC_OPERATOR(OPERATOR)         \
  template <index_t I>                                   \
  constexpr StaticIndex<(OPERATOR I)> operator OPERATOR( \
      const StaticIndex<I>) {                            \
    return {};                                           \
  }

MAKE_UNARY_ARITHMETIC_OPERATOR(+)
MAKE_UNARY_ARITHMETIC_OPERATOR(-)

#undef MAKE_UNARY_ARITHMETIC_OPERATOR

}  // namespace echo
