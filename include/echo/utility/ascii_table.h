#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <boost/range.hpp>

namespace echo { namespace ascii_table {

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

  const std::string& text() const { return _text; }
  int span() const { return _span; }
  alignment_t alignment() const { return _alignment; }
  int minimum_width() const { return static_cast<int>(_text.size()) + 2; }

 private:
  std::string _text;
  int _span;
  alignment_t _alignment;
};

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

//------------------------------------------------------------------------------
// CellLayout
//------------------------------------------------------------------------------
struct CellLayout {
  int span, minimum_width, width;
};

using RowLayout = std::vector<CellLayout>;
using TableLayout = std::vector<RowLayout>;

//------------------------------------------------------------------------------
// get_subcells
//------------------------------------------------------------------------------
template <class NextRow>
auto get_subcells(NextRow& next_row, int span, int i_start) {
  int i = i_start;
  int span_sum = 0;
  while (span_sum < span && i < next_row.size()) {
    span_sum += next_row[i].span;
    ++i;
  }
  if (span_sum != span) throw InvalidTable();
  return boost::make_iterator_range(&next_row[i_start], &next_row[i]);
}

//------------------------------------------------------------------------------
// assign_minimum_widths
//------------------------------------------------------------------------------
void assign_minimum_widths(const RowLayout& next_row, RowLayout& this_row) {
  int i = 0;
  for (auto& cell : this_row) {
    int span = cell.span;

    auto subcells = get_subcells(next_row, span, i);

    int minimum_width_sum = -1;
    for (const auto& subcell : subcells)
      minimum_width_sum += subcell.minimum_width+1;

    cell.minimum_width = std::max(cell.minimum_width, minimum_width_sum);

    i += static_cast<int>(subcells.size());
  }
}

//------------------------------------------------------------------------------
// assign_width
//------------------------------------------------------------------------------
void assign_width(int width, boost::iterator_range<CellLayout*> subcells) {
  int column_divider_widths = 0;
  for (auto& subcell : subcells) {
    subcell.width = subcell.minimum_width;
    width -= subcell.minimum_width;
    ++column_divider_widths;
  }
  width -= column_divider_widths-1;

  int i = 0;
  while (width > 0) {
    auto& cell = subcells[i % subcells.size()];
    ++cell.width;
    --width;
    ++i;
  }
}

//------------------------------------------------------------------------------
// assign_widths
//------------------------------------------------------------------------------
void assign_widths(const RowLayout& prev_row,
                   RowLayout& this_row) {
  int i = 0;
  for (const auto& cell_layout : prev_row) {
    auto subcells = get_subcells(this_row, cell_layout.span, i);
    assign_width(cell_layout.width, subcells);

    i += static_cast<int>(subcells.size());
  }
}

//------------------------------------------------------------------------------
// fit_table_layout
//------------------------------------------------------------------------------
void fit_table_layout(TableLayout& table_layout) {
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
TableLayout compute_table_layout(const Table& table) {
  TableLayout table_layout;
  for (const auto& row : table) {
    RowLayout row_layout;
    if (!row.is_divider()) {
      for (const auto& cell : row.cells())
        row_layout.push_back({cell.span(), cell.minimum_width(), 0});
      table_layout.emplace_back(std::move(row_layout));
    }
  }
  fit_table_layout(table_layout);
  return table_layout;
}

//------------------------------------------------------------------------------
// print_divider
//------------------------------------------------------------------------------
void print_divider(std::ostream& out, const RowLayout& adjacent_row) {
  out << '+';
  for(const auto& cell : adjacent_row) {
    for(int i=0; i<cell.width; ++i)
      out << '-';
    out << '+';
  }
  out << '\n';
}

//------------------------------------------------------------------------------
// print_row
//------------------------------------------------------------------------------
void print_row(std::ostream& out,
  const RowLayout& row_layout,
  const Row& row)
{
  const auto& cells = row.cells();
  out << '|';
  for(int i=0; i<cells.size(); ++i) {
    const auto& cell_layout = row_layout[i];
    out << std::string(cell_layout.width, ' ');
    out << '|';
  }
  out << '\n';
}

//------------------------------------------------------------------------------
// print_table
//------------------------------------------------------------------------------
void print_table(std::ostream& out, const Table& table) {
  auto table_layout = compute_table_layout(table);
  print_divider(out, table_layout[0]);
  int row_index=0;
  for (const auto& row : table) {
    if (row.is_divider()) {
      print_divider(out, table_layout[row_index]);
      continue;
    }
    print_row(out, table_layout[row_index], row);
    ++row_index;
  }
  print_divider(out, table_layout.back());
}

}}
