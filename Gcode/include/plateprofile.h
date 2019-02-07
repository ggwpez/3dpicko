﻿#ifndef PLATEPROFILE_H
#define PLATEPROFILE_H

#include <vector>

#include "include/point.h"
#include "include/json_constructable.hpp"

namespace c3picko {

/**
 * @file plateprofile.h
 * @author flopicko
 * @brief Class representing the characteristics of
 * the source, master and goal plate. Since colonies are transferred to
 * the master plate the same way they are to the goal plate
 * these characteristics count for the master plate as well.
 */
class PlateProfile : public JsonConstructable {
 public:
	PlateProfile(QJsonObject const& obj);
  PlateProfile(int number_of_rows,
			   int number_of_columns,
			   float a1_row_offset,
			   float a1_column_offset,
			   float well_spacing_center_to_center,
			   float height_source_plate,
			   float height_master_plate,
			   float height_goal_plate,
			   float well_depth,
			   float culture_medium_thickness);

  int numberOfRows() const;
  int numberOfColumns() const;
  int numberOfWells() const;
  float a1RowOffset() const;
  float a1ColumnOffset() const;
  float wellSpacingCenterToCenter() const;
  float heightSourcePlate() const;
  float heightMasterPlate() const;
  float heightGoalPlate() const;
  float wellDepth() const;
  float cultureMediumThickness() const;

  void write(QJsonObject& obj) const;

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

  /**
   * @brief height_source_plate_ the height of the source plate
   */
  const float height_source_plate_;

  /**
   * @brief height_master_plate_ the height of the source plate
   */
  const float height_master_plate_;

  /**
   * @brief height_goal_plate_ the height of the source plate
   */
  const float height_goal_plate_;

  /**
   * @brief well_height_ the depth of every well
   */
  const float well_depth_;

  /**
   * @brief culture_medium_thickness_ the thickness of the used culture medium
   * inside the source and master plate, for instance agars
   */
  const float culture_medium_thickness_;
};
} // namespace c3picko
#endif // PLATEPROFILE_H
