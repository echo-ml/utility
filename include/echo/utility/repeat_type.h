#pragma once

namespace echo {

/////////////////
// repeat_type //
/////////////////

namespace detail {
namespace type_traits {
template <class, class T>
struct repeat_type_impl {
  using type = T;
};
}
}

template <class A, class T>
using repeat_type = typename detail::type_traits::repeat_type_impl<A, T>::type;

///////////////////
// repeat_type_c //
///////////////////

namespace detail {
namespace type_traits {
template <unsigned long long, class T>
struct repeat_type_c_impl {
  using type = T;
};
}
}

template <unsigned long long A, class T>
using repeat_type_c =
    typename detail::type_traits::repeat_type_c_impl<A, T>::type;
}
