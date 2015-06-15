#pragma once

#include <echo/concept2.h>
#include <echo/index.h>
#include <initializer_list>
#include <utility>
#include <cassert>
#include <array>
#include <stdexcept>

namespace echo {
namespace initializer {

//////////////////////////
// InitializerMultilist //
//////////////////////////

namespace detail {
namespace initializer_multilist {
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
    typename detail::initializer_multilist::InitializerMultilistImpl<Scalar,
                                                                     N>::type;

/////////////////
// get_extents //
/////////////////

namespace detail {
namespace initializer_multilist {

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

}}

template <class Value, int N>
auto get_extents(const InitializerMultilist<Value, N>& values) {
  return detail::initializer_multilist::get_extents_impl<Value, N>(
      std::make_index_sequence<N>(), values);
}

//////////////////////////////////
// InitializerMultilistAccessor //
//////////////////////////////////

namespace detail {
namespace initializer_multilist {

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
    detail::initializer_multilist::InitializerMultilistAccessorImpl<
        std::make_index_sequence<N>, Value>;

//////////////////////////
// MultilistExtentError //
//////////////////////////

struct MultilistExtentError : std::runtime_error {
  MultilistExtentError() : std::runtime_error("MultilistExtentError") {}
};

/////////////////////////////
// k_indexed_initializable //
/////////////////////////////

namespace concept {
namespace detail {
namespace initializer_multilist {
template <class>
struct KIndexedInitializable {};

template <std::size_t... Indexes>
struct KIndexedInitializable<std::index_sequence<Indexes...>> : Concept {
  template <class Value, class Accessor>
  auto require(Value&& value, Accessor&& accessor) -> list<valid<
      decltype(accessor(repeat_type_c<Indexes, index_t>()...) = value)>()>;
};
}
}

template <int K, class Value, class T>
constexpr bool k_indexed_initializable() {
  return models<detail::initializer_multilist::KIndexedInitializable<
                    std::make_index_sequence<K>>,
                Value, T>();
}
}

////////////////
// initialize //
////////////////

namespace detail {
namespace initializer_multilist {

template <int I, std::size_t N, class Value, class Functor,
          CONCEPT_REQUIRES(I == N)>
void initialize_impl(const Value& value, const std::array<int, N>& extents,
                     const Functor& functor) {
  functor(value);
}

template <int I, std::size_t N, class Values, class Functor,
          CONCEPT_REQUIRES(I != N)>
void initialize_impl(const Values& values, const std::array<int, N>& extents,
                     const Functor& functor) {
  if (values.size() != std::get<I>(extents)) throw MultilistExtentError();
  int index = 0;
  for (auto& values_i : values) {
    auto functor_new = [&](const auto& value, auto... indexes) {
      functor(value, index, indexes...);
    };
    initialize_impl<I + 1>(values_i, extents, functor_new);
    ++index;
  }
}
}
}

template <
    class Value, int K, class Accessor,
    CONCEPT_REQUIRES(concept::k_indexed_initializable<K, Value, Accessor>())>
void initialize(InitializerMultilist<Value, K> values, Accessor&& accessor) {
  auto extents = get_extents<Value, K>(values);
  auto functor = [&](const Value& value, auto... indexes) {
    accessor(indexes...) = value;
  };
  detail::initializer_multilist::initialize_impl<0>(values, extents, functor);
}
}

using initializer::InitializerMultilist;
using initializer::InitializerMultilistAccessor;
}
