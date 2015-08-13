#pragma once

#include <echo/concept.h>

namespace echo {

template <class Scalar, class IteratorFirst, class IteratorLast,
          CONCEPT_REQUIRES(
              echo::concept::iterator_range<IteratorFirst, IteratorLast>() &&
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
}
