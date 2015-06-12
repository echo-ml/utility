#pragma once

#include <echo/utility/tuple_concept.h>
#include <echo/concept2.h>
#include <tuple>

namespace echo {
namespace utility {

/////////////////////
// valid_map_apply //
/////////////////////

namespace detail {
namespace map_apply {

template <class Function, class Evaluators, std::size_t... EvaluatorIndexes,
          class... Arguments>
auto valid_map_apply_impl(Function&& function, Evaluators&& evaluators,
                          std::index_sequence<EvaluatorIndexes...>,
                          Arguments&&... arguments)
    -> std::integral_constant<
        bool, and_c<echo::concept::callable<
                  decltype(std::get<EvaluatorIndexes>(evaluators)),
                  Arguments...>()...>() &&
                  echo::concept::callable<Function,
                                          decltype(std::get<EvaluatorIndexes>(
                                              evaluators)(arguments...))...>()>;

auto valid_map_apply_impl(...) -> std::false_type;

template <class Function, class Evaluators, std::size_t NumEvaluators,
          class... Arguments>
constexpr bool valid_map_apply() {
  using Result = decltype(valid_map_apply_impl(
      std::declval<Function>(), std::declval<Evaluators>(),
      std::make_index_sequence<NumEvaluators>(), std::declval<Arguments>()...));
  return Result::value;
}
}
}

///////////////
// map_apply //
///////////////

namespace detail {
namespace map_apply {

template <class Function, class Evaluators, std::size_t... EvaluatorIndexes,
          class... Arguments>
decltype(auto) map_apply_impl(Function&& function, Evaluators&& evaluators,
                              std::index_sequence<EvaluatorIndexes...>,
                              Arguments&&... arguments) {
  return function(std::get<EvaluatorIndexes>(evaluators)(arguments...)...);
}
}
}

template <class Function, class Evaluators, class... Arguments,
          CONCEPT_REQUIRES(tuple::concept::tuple<uncvref_t<Evaluators>>()),
          CONCEPT_REQUIRES(detail::map_apply::valid_map_apply<
              Function, Evaluators,
              std::tuple_size<uncvref_t<Evaluators>>::value, Arguments...>())>
decltype(auto) map_apply(Function&& function, Evaluators&& evaluators,
                         Arguments&&... arguments) {
  return detail::map_apply::map_apply_impl(
      std::forward<Function>(function), std::forward<Evaluators>(evaluators),
      std::make_index_sequence<std::tuple_size<uncvref_t<Evaluators>>::value>(),
      std::forward<Arguments>(arguments)...);
}
}
}
