#pragma once

#define DETAIL_NS detail_ascii_table

#include <echo/utility/iterator_range.h>
#include <echo/contract.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace echo {
namespace ascii_table {

//------------------------------------------------------------------------------
// alignment_t
//------------------------------------------------------------------------------
enum class alignment_t { left, right, center };

//------------------------------------------------------------------------------
// InvalidTable
//------------------------------------------------------------------------------
struct InvalidTable : virtual std::runtime_error {
  InvalidTable() : std::runtime_error("InvalidTable") {}
};

//------------------------------------------------------------------------------
// Cell
//------------------------------------------------------------------------------
class Cell {
 public:
  explicit Cell(std::string text, int span = 1,
                alignment_t alignment = alignment_t::left)
      : _text(std::move(text)), _span(span), _alignment(alignment) {}

  Cell(std::string text, alignment_t alignment)
      : _text(std::move(text)), _span(1), _alignment(alignment) {}
  const std::string& text() const { return _text; }
  int span() const { return _span; }
  alignment_t alignment() const { return _alignment; }

 private:
  std::string _text;
  int _span;
  alignment_t _alignment;
};

//------------------------------------------------------------------------------
// get_minimum_width
//------------------------------------------------------------------------------
inline int get_minimum_width(const Cell& cell) {
  return static_cast<int>(cell.text().size()) + 2;
}

//------------------------------------------------------------------------------
// Row
//------------------------------------------------------------------------------
class Row {
 public:
  Row() {}
  explicit Row(std::vector<Cell> cells) : _cells(std::move(cells)) {}
  int span() const {
    int span = 0;
    for (const auto& cell : _cells) span += cell.span();
    return span;
  }
  const auto& cells() const { return _cells; }
  bool is_divider() const { return _cells.empty(); }

 private:
  std::vector<Cell> _cells;
};

using Table = std::vector<Row>;

namespace DETAIL_NS {
//------------------------------------------------------------------------------
// CellLayout
//------------------------------------------------------------------------------
struct CellLayout {
  int span, minimum_width, width;
};

//------------------------------------------------------------------------------
// RowLayout
//------------------------------------------------------------------------------
using RowLayout = std::vector<CellLayout>;

//------------------------------------------------------------------------------
// TableLayout
//------------------------------------------------------------------------------
using TableLayout = std::vector<RowLayout>;

//------------------------------------------------------------------------------
// get_subcell_layouts
//------------------------------------------------------------------------------
template <class NextRow>
auto get_subcell_layouts(NextRow& next_row, int span, int i_start) {
  int i = i_start;
  int span_sum = 0;
  while (span_sum < span && i < next_row.size()) {
    span_sum += next_row[i].span;
    ++i;
  }
  if (span_sum != span) throw InvalidTable();
  return make_iterator_range(&next_row[i_start], &next_row[i]);
}

//------------------------------------------------------------------------------
// assign_minimum_widths
//------------------------------------------------------------------------------
inline void assign_minimum_widths(const RowLayout& next_row,
                                  RowLayout& this_row) {
  int i = 0;
  for (auto& cell_layout : this_row) {
    int span = cell_layout.span;

    auto subcell_layouts = get_subcell_layouts(next_row, span, i);

    int minimum_width_sum = 0;
    for (const auto& subcell_layout : subcell_layouts) {
      minimum_width_sum += subcell_layout.minimum_width;
    }
    minimum_width_sum += subcell_layouts.size() - 1;

    cell_layout.minimum_width =
        std::max(cell_layout.minimum_width, minimum_width_sum);

    i += static_cast<int>(subcell_layouts.size());
  }
}

//------------------------------------------------------------------------------
// assign_width
//------------------------------------------------------------------------------
inline void assign_width(int width,
                         IteratorRange<CellLayout*> subcell_layouts) {
  for (auto& subcell_layout : subcell_layouts) {
    subcell_layout.width = subcell_layout.minimum_width;
    width -= subcell_layout.minimum_width;
  }
  width -= subcell_layouts.size() - 1;

  int i = 0;
  while (width > 0) {
    auto& cell_layout = subcell_layouts[i % subcell_layouts.size()];
    ++cell_layout.width;
    --width;
    ++i;
  }
}

//------------------------------------------------------------------------------
// assign_widths
//------------------------------------------------------------------------------
inline void assign_widths(const RowLayout& prev_row, RowLayout& this_row) {
  int i = 0;
  for (const auto& cell_layout_layout : prev_row) {
    auto subcell_layouts =
        get_subcell_layouts(this_row, cell_layout_layout.span, i);
    assign_width(cell_layout_layout.width, subcell_layouts);

    i += static_cast<int>(subcell_layouts.size());
  }
}

//------------------------------------------------------------------------------
// fit_table_layout
//------------------------------------------------------------------------------
inline void fit_table_layout(TableLayout& table_layout) {
  if (table_layout.empty()) throw InvalidTable();

  for (auto& cell_layout : table_layout.back()) {
    if (cell_layout.span != 1) throw InvalidTable();
  }

  for (int i = static_cast<int>(table_layout.size()) - 1; i-- > 0;) {
    auto& this_row = table_layout[i];
    const auto& next_row = table_layout[i + 1];

    assign_minimum_widths(next_row, this_row);
  }

  for (auto& cell_layout : table_layout.front())
    cell_layout.width = cell_layout.minimum_width;

  for (int i = 1; i < table_layout.size(); ++i) {
    const auto& prev_row = table_layout[i - 1];
    auto& this_row = table_layout[i];
    assign_widths(prev_row, this_row);
  }
}

//------------------------------------------------------------------------------
// compute_table_layout
//------------------------------------------------------------------------------
inline TableLayout compute_table_layout(const Table& table) {
  TableLayout table_layout;
  for (const auto& row : table) {
    RowLayout row_layout;
    if (!row.is_divider()) {
      for (const auto& cell : row.cells())
        row_layout.push_back({cell.span(), get_minimum_width(cell), 0});
      table_layout.emplace_back(std::move(row_layout));
    }
  }
  fit_table_layout(table_layout);
  return table_layout;
}

//------------------------------------------------------------------------------
// print_cell
//------------------------------------------------------------------------------
inline void print_cell(std::ostream& out, int width, const Cell& cell) {
  CONTRACT_EXPECT { CONTRACT_ASSERT(get_minimum_width(cell) <= width); };
  const int padding = width - static_cast<int>(cell.text().size());
  int left_padding, right_padding;
  switch (cell.alignment()) {
    case alignment_t::left:
      left_padding = 1;
      right_padding = padding - left_padding;
      break;
    case alignment_t::right:
      right_padding = 1;
      left_padding = padding - right_padding;
      break;
    case alignment_t::center:
      left_padding = padding / 2;
      right_padding = padding - left_padding;
      break;
  }
  out << std::string(left_padding, ' ') << cell.text()
      << std::string(right_padding, ' ');
}

//------------------------------------------------------------------------------
// print_divider
//------------------------------------------------------------------------------
inline void print_divider(std::ostream& out, const RowLayout& adjacent_row) {
  out << '+';
  for (const auto& cell : adjacent_row) {
    for (int i = 0; i < cell.width; ++i) out << '-';
    out << '+';
  }
  out << '\n';
}

//------------------------------------------------------------------------------
// print_row
//------------------------------------------------------------------------------
inline void print_row(std::ostream& out, const RowLayout& row_layout,
                      const Row& row) {
  const auto& cells = row.cells();
  out << '|';
  for (int i = 0; i < cells.size(); ++i) {
    const auto& cell_layout = row_layout[i];
    print_cell(out, cell_layout.width, cells[i]);
    out << '|';
  }
  out << '\n';
}
}

//------------------------------------------------------------------------------
// print_table
//------------------------------------------------------------------------------
inline void print_table(std::ostream& out, const Table& table) {
  using namespace DETAIL_NS;
  auto table_layout = compute_table_layout(table);
  print_divider(out, table_layout[0]);
  int row_index = 0;
  for (const auto& row : table) {
    if (row.is_divider()) {
      if (row_index >= table_layout.size())
        throw InvalidTable();
      print_divider(out, table_layout[row_index]);
      continue;
    }
    print_row(out, table_layout[row_index], row);
    ++row_index;
  }
  print_divider(out, table_layout.back());
}
}
}

#undef DETAIL_NS
