#include <echo/concept.h>

namespace echo {

///////////
// Range //
///////////

template <class Iterator>
class Range {
 public:
  Range(Iterator first, Iterator last) : _first(first), _last(last) {}
  Iterator begin() const { return _first; }
  Iterator end() const { return _last; }

 private:
  Iterator _first, _last;
};

template <class Iterator>
auto make_range(Iterator first, Iterator last) {
  return Range<Iterator>(first, last);
}
}
