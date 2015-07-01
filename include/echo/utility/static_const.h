#pragma once

#define DETAIL_NS detail_static_storage

namespace echo {

//------------------------------------------------------------------------------
// static_const_storage
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class T>
struct static_const_storage {
  static constexpr T value = T();
};

template <class T>
constexpr T static_const_storage<T>::value;
}

//------------------------------------------------------------------------------
// static_const
//------------------------------------------------------------------------------
template <class T>
constexpr const T& static_const() {
  return DETAIL_NS::static_const_storage<T>::value;
}
}
