#pragma once

#define DETAIL_NS detail_adl

#include <echo/utility/static_const.h>

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
}

#undef DETAIL_NS
