/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 flopicko flobotic@protonmail.com
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef PLATEPROFILE_H
#define PLATEPROFILE_H

#include <vector>

#include "Gcode/platetype.h"
#include "Gcode/point.h"

namespace d3picko {
/**
 * @file plateprofile.h
 * @author flopicko
 * @brief Class representing the characteristics of
 * the source, master and goal plate.
 */
class PlateProfile {
 public:
  PlateProfile(int number_of_rows, int number_of_columns, PlateType plateType,
			   float a1_row_offset, float a1_column_offset,
			   float well_spacing_center_to_center, float height_source_plate,
			   float height_master_plate, float height_goal_plate,
			   float well_depth, float culture_medium_thickness_source_plate,
			   float culture_medium_thickness_master_plate,
			   int timesToLowerFilamentIntoWell, float red_frame_width,
			   float red_frame_height);

  int numberOfRows() const;
  int numberOfColumns() const;
  int numberOfWells() const;
  PlateType plateType() const;
  float a1RowOffset() const;
  float a1ColumnOffset() const;
  float wellSpacingCenterToCenter() const;
  float heightSourcePlate() const;
  float heightMasterPlate() const;
  float heightGoalPlate() const;
  float wellDepth() const;
  float cultureMediumThicknessSourcePlate() const;
  float cultureMediumThicknessMasterPlate() const;
  int timesToLowerFilamentIntoWell() const;
  float redFrameWidth() const;
  float redFrameHeight() const;

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
   * @brief plate_type_ Type of the plate. Used in the plate detection
   * algorithm.
   */
  const PlateType plate_type_;

  /**
   * @brief a1_row_offset_ The offset of the center of the first well A1 to the
   * upper
   * edge of the goal plate.
   */
  const float a1_row_offset_;

  /**
   * @brief a1_column_offset_ The offset of the center of the first well A1 to
   * the left
   * edge of the goal plate.
   */
  const float a1_column_offset_;

  /**
   * @brief well_spacing_center_to_center_ The distance between the center of a
   * well
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
   * @brief well_depth_ the depth of every well,
   * this value is used for setting the length
   * of the extruded filament while dipping the filament
   * into the fluid inside the well
   */
  const float well_depth_;

  /**
   * @brief culture_medium_thickness_source_plate_ the thickness of the used
   * culture medium inside the source plate, for instance agars
   */
  const float culture_medium_thickness_source_plate_;

  /**
   * @brief culture_medium_thickness_master_plate_ the thickness of the used
   * culture medium inside the master plate, for instance agars
   */
  const float culture_medium_thickness_master_plate_;

  /**
   * @brief
   * times_to_lower_filament_into_well_ the number of time that the filament
   * should tip into the PCR well fluid, 1-2 should be enought .
   */
  const int times_to_lower_filament_into_well_;

  /**
   * @brief red_frame_width_ width of the red frame that is placed on the
   * source plate in mm.
   */
  const float red_frame_width_;

  /**
   * @brief red_frame_height_ width of the red frame that is placed on the
   * source plate in mm.
   */
  const float red_frame_height_;
};
}  // namespace d3picko
#endif	// PLATEPROFILE_H
