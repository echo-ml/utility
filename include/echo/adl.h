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
}

namespace {
static constexpr auto& begin = static_const<DETAIL_NS::Begin>();
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
}

namespace {
static constexpr auto& end = static_const<DETAIL_NS::End>();
}
}

#undef DETAIL_NS
