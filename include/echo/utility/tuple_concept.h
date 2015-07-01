#pragma once

#define detail_tuple

#include <tuple>
#include <echo/concept.h>

namespace echo {
namespace tuple {

namespace concept {

//------------------------------------------------------------------------------
// tuple
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class... T>
using ConsumeTypes = std::true_type;

template <class T, std::size_t... Indexes>
auto gettable(T&& t, std::index_sequence<Indexes...>)
    -> ConsumeTypes<decltype(std::get<Indexes>(t))...>;

auto gettable(...) -> std::false_type;

template <class T, std::size_t... Indexes>
auto tuple_elementable(T&& t, std::index_sequence<Indexes...>)
    -> ConsumeTypes<std::tuple_element_t<Indexes, uncvref_t<T>>...>;

auto tuple_elementable(...) -> std::false_type;

struct Tuple : Concept {
  template <class T>
  auto require(T&& t)
      -> list<convertible<decltype(std::tuple_size<T>::value), std::size_t>(),
              same<decltype(gettable(
                       std::forward<T>(t),
                       std::make_index_sequence<std::tuple_size<T>::value>())),
                   std::true_type>(),
              same<decltype(tuple_elementable(
                       std::forward<T>(t),
                       std::make_index_sequence<std::tuple_size<T>::value>())),
                   std::true_type>()>;
};
}

template <class T>
constexpr bool tuple() {
  return models<DETAIL_NS::Tuple, T>();
}
}
}
}

#undef DETAIL_NS
