#include "include/masterandgoalplateprofile.h"

namespace c3picko {

	MasterAndGoalPlateProfile::MasterAndGoalPlateProfile(const QJsonObject& obj)
		: JsonConstructable(obj),
		  number_of_rows_(obj["number_of_rows"].toInt()),
		  number_of_columns_(obj["number_of_columns"].toInt()),
		  number_of_wells_(number_of_columns_ * number_of_rows_),
		  a1_row_offset_(obj["a1_row_offset"].toDouble()),
		  a1_column_offset_(obj["a1_column_offset"].toDouble()),
		  well_spacing_center_to_center_(obj["well_spacing_center_to_center"].toDouble())
	{

	}

	MasterAndGoalPlateProfile::MasterAndGoalPlateProfile(int number_of_rows,
													 int number_of_columns,
													 float a1_row_offset,
													 float a1_column_offset,
													 float well_spacing_center_to_center)
  : JsonConstructable(QJsonObject()),
	number_of_rows_(number_of_rows),
	number_of_columns_(number_of_columns),
	number_of_wells_(number_of_columns * number_of_rows),
	a1_row_offset_(a1_row_offset),
	a1_column_offset_(a1_column_offset),
	well_spacing_center_to_center_(well_spacing_center_to_center)
	{

}

int MasterAndGoalPlateProfile::numberOfRows() const {
  return number_of_rows_;
}

int MasterAndGoalPlateProfile::numberOfColumns() const {
  return number_of_columns_;
}

int MasterAndGoalPlateProfile::numberOfWells() const {
  return number_of_wells_;
}

float MasterAndGoalPlateProfile::a1RowOffset() const {
  return a1_row_offset_;
}

float MasterAndGoalPlateProfile::a1ColumnOffset() const {
  return a1_column_offset_;
}

float MasterAndGoalPlateProfile::wellSpacingCenterToCenter() const {
	return well_spacing_center_to_center_;
}

void MasterAndGoalPlateProfile::write(QJsonObject&) const
{

}
} // namespace c3picko
