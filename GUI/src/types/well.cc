#include "include/types/well.h"
#include "include/exception.h"
#include "include/plateprofile.h"

namespace c3picko {
Well::Well(quint8 row, quint8 col, PlateProfile* plate)
    : row_(row), col_(col), plate_(plate) {
  if (plate->numberOfRows() > 16 || plate->numberOfColumns() > 24)
    throw Exception("Biggest plate supported is 384");
  if ((row - 1) > plate->numberOfRows() || (col - 1) > plate->numberOfColumns())
    throw Exception("Well cant be outside of plate");
  if (row < 1 || col < 1)
    throw Exception("Wells start at 1. A1 would be (1,1)");
}

Well& Well::operator++() {
  // -1 +1 = 0
  if (row_ == plate_->numberOfRows()) {
    row_ = 1;

    if (col_ == plate_->numberOfColumns())
      throw Exception("Well is at end of PCR plate and cant advance.");
    else
      ++col_;
  } else
    ++row_;

  return *this;
}

bool Well::operator<(const Well& other) const {
  return index() < other.index();
}

QString Well::toString() const {
  return QString(char('A' + (row_ - 1))) + QString::number(col_);
}

quint8 Well::row() const { return row_; }

quint8 Well::col() const { return col_; }

quint16 Well::index() const {
  return quint16((col_ - 1) * plate_->numberOfRows() + (row_ - 1));
}
}  // namespace c3picko
