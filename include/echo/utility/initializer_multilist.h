#pragma once

#include <echo/concept2.h>
#include <initializer_list>
#include <utility>
#include <cassert>
#include <array>

namespace echo {
namespace initializer {

//////////////////////////
// InitializerMultilist //
//////////////////////////

namespace detail {
namespace initializer_list_accessor {
template <class Value, int N>
struct InitializerMultilistImpl {
  using type = std::initializer_list<
      typename InitializerMultilistImpl<Value, N - 1>::type>;
};

template <class Value>
struct InitializerMultilistImpl<Value, 0> {
  using type = Value;
};
}
}

template <class Scalar, int N>
using InitializerMultilist =
    typename detail::initializer_list_accessor::InitializerMultilistImpl<
        Scalar, N>::type;

/////////////////
// get_extents //
/////////////////

namespace detail {
namespace initializer_list_accessor {

template <int I, class Value, int N, CONCEPT_REQUIRES(I == 0)>
int get_extent(const InitializerMultilist<Value, N>& values) {
  return values.size();
}

template <int I, class Value, int N, CONCEPT_REQUIRES(I != 0)>
int get_extent(const InitializerMultilist<Value, N>& values) {
  return get_extent<I - 1, Value, N - 1>(*std::begin(values));
}

template <class Value, int N, std::size_t... Indexes>
auto get_extents_impl(std::index_sequence<Indexes...>,
                      const InitializerMultilist<Value, N>& values) {
  return std::array<int, N>{get_extent<Indexes, Value, N>(values)...};
}

template <class Value, int N>
auto get_extents(const InitializerMultilist<Value, N>& values) {
  return get_extents_impl<Value, N>(std::make_index_sequence<N>(), values);
}
}
}

//////////////////////////////////
// InitializerMultilistAccessor //
//////////////////////////////////

namespace detail {
namespace initializer_list_accessor {

template <class Indexes, class Value>
class InitializerMultilistAccessorImpl {};

template <class Value>
class InitializerMultilistAccessorImpl<std::index_sequence<>, Value> {
 public:
  InitializerMultilistAccessorImpl(const Value& value) : _value(value) {}
  auto operator()() const { return _value; }

 private:
  Value _value;
};

template <std::size_t IndexFirst, std::size_t... IndexesRest, class Value>
class InitializerMultilistAccessorImpl<
    std::index_sequence<IndexFirst, IndexesRest...>, Value> {
 public:
  InitializerMultilistAccessorImpl(
      InitializerMultilist<Value, sizeof...(IndexesRest) + 1> values)
      : _values(values) {}
  auto operator()(
      int index_first,
      std::enable_if_t<IndexesRest || true, int>... indexes_rest) const {
    assert(index_first < _values.size());
    return InitializerMultilistAccessorImpl<std::index_sequence<IndexesRest...>,
                                            Value> (
        *(std::begin(_values) + index_first))(indexes_rest...);
  }
  auto extents() const {
    return get_extents<Value, sizeof...(IndexesRest) + 1>(_values);
  }

 private:
  InitializerMultilist<Value, sizeof...(IndexesRest) + 1> _values;
};
}
}

template <class Value, int N>
using InitializerMultilistAccessor =
    detail::initializer_list_accessor::InitializerMultilistAccessorImpl<
        std::make_index_sequence<N>, Value>;
}

using initializer::InitializerMultilist;
using initializer::InitializerMultilistAccessor;
}
