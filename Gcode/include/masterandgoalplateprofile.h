#ifndef MASTERANDGOALPLATEPROFILE_H
#define MASTERANDGOALPLATEPROFILE_H

#include <vector>

#include "include/point.h"

namespace c3picko {

/**
 * @file masterandgoalplateprofile.h
 * @author flopicko
 * @brief Class representing the characteristics of
 * a goal plate. Since colonies are transferred to
 * the master plate the same way they are to the goal plate
 * these characteristics count for the master plate as well.
 */
class MasterAndGoalPlateProfile {
 public:
  MasterAndGoalPlateProfile(int number_of_rows,
                            int number_of_columns,
                            float a1_row_offset,
                            float a1_column_offset,
                            float well_spacing_center_to_center);

  int numberOfRows() const;
  int numberOfColumns() const;
  int numberOfWells() const;
  float a1RowOffset() const;
  float a1ColumnOffset() const;
  float wellSpacingCenterToCenter() const;

 private:
  /**
   * @brief number_of_rows_ The number of rows the goal plate has.
   */
  const int number_of_rows_;

  /**
   * @brief number_of_columns_ The number of columns the goal plate has.
   */
  const int number_of_columns_;

  /**
   * @brief number_of_wells_ The number of wells the goal plate has.
   */
  const int number_of_wells_;

  /**
   * @brief a1_row_offset_ The offset of the center of the first well A1 to the upper
   * edge of the goal plate.
   */
  const float a1_row_offset_;

  /**
   * @brief a1_column_offset_ The offset of the center of the first well A1 to the left
   * edge of the goal plate.
   */
  const float a1_column_offset_;

  /**
   * @brief well_spacing_center_to_center_ The distance between the center of a well
   * to the center of any directly adjacent wells.
   */
  const float well_spacing_center_to_center_;
};
} // namespace c3picko
#endif // MASTERANDGOALPLATEPROFILE_H
