#include "include/plateprofile.h"

namespace c3picko {
PlateProfile::PlateProfile(int number_of_rows, int number_of_columns,
                           float a1_row_offset, float a1_column_offset,
                           float well_spacing_center_to_center,
                           float height_source_plate, float height_master_plate,
                           float height_goal_plate, float well_depth,
                           float culture_medium_thickness)
    : number_of_rows_(number_of_rows), number_of_columns_(number_of_columns),
      number_of_wells_(number_of_columns * number_of_rows),
      a1_row_offset_(a1_row_offset), a1_column_offset_(a1_column_offset),
      well_spacing_center_to_center_(well_spacing_center_to_center),
      height_source_plate_(height_source_plate),
      height_master_plate_(height_master_plate),
      height_goal_plate_(height_goal_plate), well_depth_(well_depth),
      culture_medium_thickness_(culture_medium_thickness) {}

int PlateProfile::numberOfRows() const { return number_of_rows_; }

int PlateProfile::numberOfColumns() const { return number_of_columns_; }

int PlateProfile::numberOfWells() const { return number_of_wells_; }

float PlateProfile::a1RowOffset() const { return a1_row_offset_; }

float PlateProfile::a1ColumnOffset() const { return a1_column_offset_; }

float PlateProfile::wellSpacingCenterToCenter() const {
  return well_spacing_center_to_center_;
}

float PlateProfile::heightSourcePlate() const { return height_source_plate_; }

float PlateProfile::heightMasterPlate() const { return height_master_plate_; }

float PlateProfile::heightGoalPlate() const { return height_goal_plate_; }

float PlateProfile::wellDepth() const { return well_depth_; }

float PlateProfile::cultureMediumThickness() const {
  return culture_medium_thickness_;
}
} // namespace c3picko
