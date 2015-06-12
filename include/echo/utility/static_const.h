#pragma once

namespace echo {

namespace detail {
namespace static_const {

//////////////////////////
// static_const_storage //
//////////////////////////

template <class T>
struct static_const_storage {
  static constexpr T value = T();
};

template <class T>
constexpr T static_const_storage<T>::value;
}
}

//////////////////
// static_const //
//////////////////

template <class T>
constexpr const T& static_const() {
  return detail::static_const::static_const_storage<T>::value;
}
}
