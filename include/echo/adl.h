#pragma once

#define DETAIL_NS detail_adl

#include <echo/utility/static_const.h>
#include <algorithm>

namespace echo {

//------------------------------------------------------------------------------
// begin
//------------------------------------------------------------------------------
namespace DETAIL_NS {
struct Begin {
  template <class T>
  auto operator()(T&& x) const {
    using std::begin;
    return begin(x);
  }
};
struct CBegin {
  template <class T>
  auto operator()(T&& x) const {
    using std::cbegin;
    return cbegin(x);
  }
};
}

namespace {
static constexpr auto& begin = static_const<DETAIL_NS::Begin>();
static constexpr auto& cbegin = static_const<DETAIL_NS::CBegin>();
}

//------------------------------------------------------------------------------
// end
//------------------------------------------------------------------------------
namespace DETAIL_NS {
struct End {
  template <class T>
  auto operator()(T&& x) const {
    using std::end;
    return end(x);
  }
};
struct CEnd {
  template <class T>
  auto operator()(T&& x) const {
    using std::cend;
    return cend(x);
  }
};
}

namespace {
static constexpr auto& end = static_const<DETAIL_NS::End>();
static constexpr auto& cend = static_const<DETAIL_NS::CEnd>();
}

//------------------------------------------------------------------------------
// max
//------------------------------------------------------------------------------
namespace DETAIL_NS {
struct Max {
  template <class X, class Y>
  auto operator()(X&& x, Y&& y) const {
    using std::max;
    return max(std::forward<X>(x), std::forward<Y>(y));
  }
};
}

namespace {
static constexpr auto& max = static_const<DETAIL_NS::Max>();
}

//------------------------------------------------------------------------------
// min
//------------------------------------------------------------------------------
namespace DETAIL_NS {
struct Min {
  template <class X, class Y>
  auto operator()(X&& x, Y&& y) const {
    using std::min;
    return min(std::forward<X>(x), std::forward<Y>(y));
  }
};
}

namespace {
static constexpr auto& min = static_const<DETAIL_NS::Min>();
}
}

#undef DETAIL_NS
