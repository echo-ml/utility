#pragma once

#include <echo/utility/static_const.h>
#include <utility>

namespace echo {
namespace functional {

#define ECHO_MAKE_FUNCTIONAL(UPPER_NAME, LOWER_NAME, SYMBOL)        \
  namespace detail {                                                \
  namespace functional {                                            \
  struct UPPER_NAME {                                               \
    template <class X, class Y>                                     \
    constexpr auto operator()(X && x, Y&& y) const                  \
        -> decltype(std::forward<X>(x) SYMBOL std::forward<Y>(y)) { \
      return std::forward<X>(x) SYMBOL std::forward<Y>(y);          \
    }                                                               \
  };                                                                \
  }                                                                 \
  }                                                                 \
  namespace {                                                       \
  static constexpr auto& LOWER_NAME =                               \
      static_const<detail::functional::UPPER_NAME>();               \
  }
ECHO_MAKE_FUNCTIONAL(And, and_, &&)
ECHO_MAKE_FUNCTIONAL(Or, or_, || )
ECHO_MAKE_FUNCTIONAL(Less, less, < )
ECHO_MAKE_FUNCTIONAL(LessOrEqual, less_or_equal, <= )
ECHO_MAKE_FUNCTIONAL(greater, greater, > )
ECHO_MAKE_FUNCTIONAL(greaterOrEqual, greater_or_equal, >= )
ECHO_MAKE_FUNCTIONAL(Equal, equal, == )
#undef ECHO_MAKE_FUNCTIONAL
}
}
