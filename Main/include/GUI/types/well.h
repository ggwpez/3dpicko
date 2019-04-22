#pragma once

#include <QtGlobal>

namespace c3picko {
class PlateProfile;
/**
 * @brief Represents a PCR well position.
 */
class Well {
 public:
  Well(quint8 row, quint8 col, PlateProfile* plate);

  /**
   * @brief Pre increment operator
   * @return Advances the well. Throws on error.
   */
  Well& operator++();
  bool operator<(Well const&) const;

  QString toString() const;

  quint8 row() const;
  quint8 col() const;
  /**
   * @brief Index of the well. Example: A1 = 0, B1 = 1, A2 = 8
   */
  quint16 index() const;

 private:
  /**
   * @brief B1 = (2,1)
   */
  quint8 row_, col_;
  PlateProfile* plate_;
};
}  // namespace c3picko
