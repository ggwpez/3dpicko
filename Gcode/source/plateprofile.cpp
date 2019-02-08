﻿#include "include/plateprofile.h"

namespace c3picko {

	PlateProfile::PlateProfile(const QJsonObject& obj)
		: JsonConstructable (obj),
		  a1_column_offset_(obj["a1_column_offset"].toString().toDouble()),
		  a1_row_offset_(obj["a1_row_offset"].toString().toDouble()),
		  culture_medium_thickness_(obj["culture_medium_thickness"].toString().toDouble()),
		  height_goal_plate_(obj["height_goal_plate"].toString().toDouble()),
		  height_master_plate_(obj["height_master_plate"].toString().toDouble()),
		  height_source_plate_(obj["height_source_plate"].toString().toDouble()),
		  number_of_columns_(obj["number_of_columns"].toString().toDouble()),
		  number_of_rows_(obj["number_of_rows"].toString().toDouble()),
		  well_depth_(obj["well_depth"].toString().toDouble()),
		  well_spacing_center_to_center_(obj["well_spacing_center_to_center"].toString().toDouble()),
		  number_of_wells_(number_of_columns_ * number_of_rows_)
	{

	}

	PlateProfile::PlateProfile(int number_of_rows,
						   int number_of_columns,
						   float a1_row_offset,
						   float a1_column_offset,
						   float well_spacing_center_to_center,
						   float height_source_plate,
						   float height_master_plate,
						   float height_goal_plate,
						   float well_depth,
						   float culture_medium_thickness)
  : JsonConstructable (QJsonObject()),
	  number_of_rows_(number_of_rows),
	number_of_columns_(number_of_columns),
	number_of_wells_(number_of_columns * number_of_rows),
	a1_row_offset_(a1_row_offset),
	a1_column_offset_(a1_column_offset),
	well_spacing_center_to_center_(well_spacing_center_to_center),
	height_source_plate_(height_source_plate),
	height_master_plate_(height_master_plate),
	height_goal_plate_(height_goal_plate),
	well_depth_(well_depth),
	culture_medium_thickness_(culture_medium_thickness) {

}

int PlateProfile::numberOfRows() const {
  return number_of_rows_;
}

int PlateProfile::numberOfColumns() const {
  return number_of_columns_;
}

int PlateProfile::numberOfWells() const {
  return number_of_wells_;
}

float PlateProfile::a1RowOffset() const {
  return a1_row_offset_;
}

float PlateProfile::a1ColumnOffset() const {
  return a1_column_offset_;
}

float PlateProfile::wellSpacingCenterToCenter() const {
  return well_spacing_center_to_center_;
}

float PlateProfile::heightSourcePlate() const {
  return height_source_plate_;
}

float PlateProfile::heightMasterPlate() const {
  return height_master_plate_;
}

float PlateProfile::heightGoalPlate() const {
  return height_goal_plate_;
}

float PlateProfile::wellDepth() const {
  return well_depth_;
}

float PlateProfile::cultureMediumThickness() const {
	return culture_medium_thickness_;
}

void PlateProfile::write(QJsonObject& obj) const
{
	obj["a1_column_offset"] = QString::number(a1_column_offset_);
	obj["a1_row_offset"] = QString::number(a1_row_offset_);
	obj["culture_medium_thickness"] = QString::number(culture_medium_thickness_);
	obj["height_goal_plate"] =QString::number( height_goal_plate_);
	obj["height_master_plate"] = QString::number(height_master_plate_);
	obj["height_source_plate"] = QString::number(height_source_plate_);
	obj["number_of_columns"] = QString::number(number_of_columns_);
	obj["number_of_rows"] = QString::number(number_of_rows_);
	obj["well_depth"] = QString::number(well_depth_);
	obj["well_spacing_center_to_center"] = QString::number(well_spacing_center_to_center_);
	// FIXME number_of_Wells_
}
} // namespace c3picko