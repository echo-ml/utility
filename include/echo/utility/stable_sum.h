#pragma once

#include <echo/concept.h>
#include <echo/adl.h>

namespace echo {

//------------------------------------------------------------------------------
// kahan_sum
//------------------------------------------------------------------------------
template <class Scalar, class IteratorFirst, class IteratorLast,
          CONCEPT_REQUIRES(
              echo::concept::iterator_range<IteratorFirst, IteratorLast>() &&
              echo::concept::input_iterator<IteratorFirst>() &&
              std::is_floating_point<Scalar>() &&
              std::is_convertible<iterator_traits::value_type<IteratorFirst>,
                                  Scalar>())>
Scalar kahan_sum(IteratorFirst first, IteratorLast last) {
  Scalar sum = 0;
  Scalar correction = 0;
  for (IteratorFirst i = first; i != last; ++i) {
    Scalar y = *i - correction;
    Scalar t = sum + y;
    correction = (t - sum) - y;
    sum = t;
  }
  return sum;
}

template <class Scalar, class Range,
          CONCEPT_REQUIRES(echo::concept::input_range<uncvref_t<Range>>())>
Scalar kahan_sum(const Range& range) {
  return kahan_sum<Scalar>(echo::begin(range), echo::end(range));
}

//------------------------------------------------------------------------------
// stable_sum
//------------------------------------------------------------------------------
template <
    class IteratorFirst, class IteratorLast,
    CONCEPT_REQUIRES(
        echo::concept::iterator_range<IteratorFirst, IteratorLast>() &&
        (std::is_same<float, iterator_traits::value_type<IteratorFirst>>() ||
         std::is_same<double, iterator_traits::value_type<IteratorFirst>>()))>
iterator_traits::value_type<IteratorFirst> stable_sum(IteratorFirst i,
                                                      IteratorLast last) {
  using SumScalar = std::conditional_t<
      std::is_same<float, iterator_traits::value_type<IteratorFirst>>::value,
      double, long double>;
  SumScalar result = 0;
  for (; i != last; ++i) result += *i;
  return static_cast<iterator_traits::value_type<IteratorFirst>>(result);
}

template <class Range, CONCEPT_REQUIRES(echo::concept::input_range<Range>())>
auto stable_sum(const Range& range) -> decltype(stable_sum(echo::begin(range),
                                                           echo::end(range))) {
  return stable_sum(echo::begin(range), echo::end(range));
}
}
