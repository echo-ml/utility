#pragma once

#include <echo/concept.h>
#include <echo/index.h>
#include <iterator>

namespace echo {

//------------------------------------------------------------------------------
// IteratorRange
//------------------------------------------------------------------------------
template <class Iterator>
class IteratorRange {
 public:
  IteratorRange(Iterator first, Iterator last) : _first(first), _last(last) {}
  Iterator begin() const { return _first; }
  Iterator end() const { return _last; }
  std::size_t size() const { return std::distance(_first, _last); }
  decltype(auto) operator[](index_t i) const { return *std::next(_first, i); }

 private:
  Iterator _first, _last;
};

template <class Iterator>
auto make_iterator_range(Iterator first, Iterator last) {
  return IteratorRange<Iterator>(first, last);
}
}
