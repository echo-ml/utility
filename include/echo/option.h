#pragma once

#define DETAIL_NS detail_option

#include <type_traits>
#include <echo/concept.h>

namespace echo {
namespace option {

//------------------------------------------------------------------------------
// Option
//------------------------------------------------------------------------------
template <class ValueType, ValueType Value>
struct Option {
  using type = ValueType;
  static constexpr ValueType value = Value;
};

namespace concept {

namespace DETAIL_NS {
template <class ValueType, ValueType Value>
auto option_impl(Option<ValueType, Value>) -> std::true_type;

auto option_impl(...) -> std::false_type;
}  // namespace detail

template <class T>
constexpr bool option() {
  using Result = decltype(DETAIL_NS::option_impl(std::declval<T>()));
  return Result::value;
}
}  // namespace concept

//------------------------------------------------------------------------------
// NullOption
//------------------------------------------------------------------------------
using NullOption = Option<std::nullptr_t, nullptr>;

constexpr NullOption kNullOption{};

//------------------------------------------------------------------------------
// OptionSet
//------------------------------------------------------------------------------
template <class... Options>
struct OptionSet {};

namespace concept {

namespace DETAIL_NS {
template <class... Options,
          CONCEPT_REQUIRES(and_c<concept::option<Options>()...>())>
auto option_set_impl(OptionSet<Options...>) -> std::true_type;

auto option_set_impl(...) -> std::false_type;
}  // namespace detail

template <class T>
constexpr bool option_set() {
  using Result = decltype(DETAIL_NS::option_set_impl(std::declval<T>()));
  return Result::value;
}

template <class T>
constexpr bool option_list() {
  return option_set<T>() || option<T>();
}
}  // namespace concept

//------------------------------------------------------------------------------
// has_option
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class ValueType>
constexpr bool has_option_impl(OptionSet<>) {
  return false;
}

template <class ValueType, class ValueType2, ValueType2 Value2,
          class... OptionsRest>
constexpr bool has_option_impl(
    OptionSet<Option<ValueType2, Value2>, OptionsRest...>) {
  return std::is_same<ValueType, ValueType2>::value ||
         has_option_impl<ValueType>(OptionSet<OptionsRest...>{});
}
}  // namespace detail

template <class ValueType, class OptionSet,
          CONCEPT_REQUIRES(concept::option_set<OptionSet>())>
constexpr bool has_option() {
  return DETAIL_NS::has_option_impl<ValueType>(OptionSet{});
}

template <class ValueType, class Option,
          CONCEPT_REQUIRES(concept::option<Option>())>
constexpr bool has_option() {
  return std::is_same<ValueType, typename Option::type>::value;
}

//------------------------------------------------------------------------------
// get_option
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class ValueType, class ValueType1, ValueType1 Value1,
          class... OptionsRest,
          CONCEPT_REQUIRES(std::is_same<ValueType, ValueType1>::value)>
constexpr ValueType get_option_impl(
    OptionSet<Option<ValueType1, Value1>, OptionsRest...>) {
  return Value1;
}

template <class ValueType, class ValueType1, ValueType1 Value1,
          class... OptionsRest,
          CONCEPT_REQUIRES(!std::is_same<ValueType, ValueType1>::value)>
constexpr ValueType get_option_impl(
    OptionSet<Option<ValueType1, Value1>, OptionsRest...>) {
  return get_option_impl<ValueType>(OptionSet<OptionsRest...>());
}
}  // namespace detail

template <class ValueType, class OptionSet,
          CONCEPT_REQUIRES(concept::option_set<OptionSet>()),
          CONCEPT_REQUIRES(has_option<ValueType, OptionSet>())>
constexpr auto get_option() {
  return Option<ValueType,
                DETAIL_NS::get_option_impl<ValueType>(OptionSet())>{};
}

template <class ValueType, class OptionSet,
          CONCEPT_REQUIRES(concept::option_set<OptionSet>()),
          CONCEPT_REQUIRES(!has_option<ValueType, OptionSet>())>
constexpr NullOption get_option() {
  return {};
}

template <class ValueType, class Option,
          CONCEPT_REQUIRES(concept::option<Option>()),
          CONCEPT_REQUIRES(has_option<ValueType, Option>())>
constexpr auto get_option() {
  return Option{};
}

template <class ValueType, class Option,
          CONCEPT_REQUIRES(concept::option<Option>()),
          CONCEPT_REQUIRES(!has_option<ValueType, Option>())>
constexpr NullOption get_option() {
  return {};
}

//------------------------------------------------------------------------------
// operator==
//------------------------------------------------------------------------------
template <class ValueType, ValueType Value1, ValueType Value2>
constexpr bool operator==(Option<ValueType, Value1>,
                          Option<ValueType, Value2>) {
  return Value1 == Value2;
}

template <class ValueType, ValueType Value>
constexpr bool operator==(Option<ValueType, Value>, NullOption) {
  return false;
}

template <class ValueType, ValueType Value>
constexpr bool operator==(NullOption, Option<ValueType, Value>) {
  return false;
}

constexpr bool operator==(NullOption, NullOption) { return true; }

//------------------------------------------------------------------------------
// operator!=
//------------------------------------------------------------------------------
template <class ValueType, ValueType Value1, ValueType Value2>
constexpr bool operator!=(Option<ValueType, Value1>,
                          Option<ValueType, Value2>) {
  return Value1 != Value2;
}

template <class ValueType, ValueType Value>
constexpr bool operator!=(Option<ValueType, Value>, NullOption) {
  return false;
}

template <class ValueType, ValueType Value>
constexpr bool operator!=(NullOption, Option<ValueType, Value>) {
  return false;
}

constexpr bool operator!=(NullOption, NullOption) { return true; }

//------------------------------------------------------------------------------
// replace_option
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class Option1, class Option2>
struct ReplaceOptionImpl {
  using type = Option2;
};

template <class ValueType, ValueType Value1, ValueType Value2>
struct ReplaceOptionImpl<Option<ValueType, Value1>, Option<ValueType, Value2>> {
  using type = Option<ValueType, Value1>;
};

template <class Option1, class Option2>
using ReplaceOption = typename ReplaceOptionImpl<Option1, Option2>::type;
}  // namespace detail

//------------------------------------------------------------------------------
// operator|
//------------------------------------------------------------------------------
template <class... Options, class ValueType, ValueType Value,
          CONCEPT_REQUIRES(!has_option<ValueType, OptionSet<Options...>>())>
constexpr auto operator|(OptionSet<Options...>, Option<ValueType, Value>) {
  return OptionSet<Option<ValueType, Value>, Options...>();
}

template <class... Options, class ValueType, ValueType Value,
          CONCEPT_REQUIRES(has_option<ValueType, OptionSet<Options...>>())>
constexpr auto operator|(OptionSet<Options...>, Option<ValueType, Value>) {
  return OptionSet<
      DETAIL_NS::ReplaceOption<Option<ValueType, Value>, Options>...>{};
}

template <class... Options,
          CONCEPT_REQUIRES(concept::option_set<OptionSet<Options...>>())>
constexpr auto operator|(OptionSet<Options...> lhs, OptionSet<>) {
  return lhs;
}

template <class... OptionsLhs, class OptionRhsFirst, class... OptionsRhsRest>
constexpr auto operator|(OptionSet<OptionsLhs...> options_lhs,
                         OptionSet<OptionRhsFirst, OptionsRhsRest...>) {
  return (options_lhs | OptionRhsFirst{}) | OptionSet<OptionsRhsRest...>{};
}

template <class ValueType, ValueType Value, class OptionsRhs,
          CONCEPT_REQUIRES(concept::option_list<OptionsRhs>())>
constexpr auto operator|(Option<ValueType, Value> option,
                         OptionsRhs options_rhs) {
  return OptionSet<Option<ValueType, Value>>{} | options_rhs;
}
}  // namespace option
}  // namespace echo

namespace echo {
using echo::option::get_option;
using echo::option::has_option;
}

#undef DETAIL_NS
