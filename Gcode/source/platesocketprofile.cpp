#include "include/platesocketprofile.h"
#include <QDebug>

namespace c3picko {

	PlateSocketProfile::PlateSocketProfile(const QJsonObject& obj)
		: JsonConstructable(obj),
		  global_origin_of_source_plate_(obj["global_origin_of_source_plate"].toObject()),
			  global_origin_of_master_plate_(obj["global_origin_of_master_plate"].toObject()),
			  global_origin_of_goal_plate_(obj["global_origin_of_goal_plate"].toObject()),
			  orientation_of_goal_plate_(fromJson(obj["orientation_of_goal_plate"])),
			  socket_origin_offset_x_(obj["socket_origin_offset_x"].toDouble()),
			  socket_origin_offset_y_(obj["socket_origin_offset_y"].toDouble())
	{

	}

	PlateSocketProfile::PlateSocketProfile(const Point& global_origin_of_source_plate,
									   const Point& global_origin_of_master_plate,
									   const Point& global_origin_of_goal_plate,
									   GoalPlateOrientation orientation_of_goal_plate,
									   float socket_origin_offset_x,
									   float socket_origin_offset_y)
  : JsonConstructable(QJsonObject()),
	global_origin_of_source_plate_(global_origin_of_source_plate),
	global_origin_of_master_plate_(global_origin_of_master_plate),
	global_origin_of_goal_plate_(global_origin_of_goal_plate),
	orientation_of_goal_plate_(orientation_of_goal_plate),
	socket_origin_offset_x_(socket_origin_offset_x),
	socket_origin_offset_y_(socket_origin_offset_y) {

}

float PlateSocketProfile::originOffsetX() const {
  return socket_origin_offset_x_;
}

float PlateSocketProfile::originOffsetY() const {
  return socket_origin_offset_y_;
}

Point PlateSocketProfile::originOfSourcePlate() const {
  return global_origin_of_source_plate_;
}

Point PlateSocketProfile::originOfMasterPlate() const {
  return global_origin_of_master_plate_;
}

Point PlateSocketProfile::originOfGoalPlate() const {
  return global_origin_of_goal_plate_;
}

GoalPlateOrientation PlateSocketProfile::orientationOfGoalPlate() const {
	return orientation_of_goal_plate_;
}

void PlateSocketProfile::write(QJsonObject&) const
{

}

GoalPlateOrientation fromJson(QJsonValue const& obj)
{
	QString value = obj.toString();

	if (value == "kFirstRowFirstColumnAtCutoutOrigin")
		return kFirstRowFirstColumnAtCutoutOrigin;
	else if (value == "kLastRowFirstColumnAtCutoutOrigin")
		return kLastRowFirstColumnAtCutoutOrigin;
	else
	{
		qCritical() << "Unknown enum value" << value << "for enum GoalPlateOrientation";
		return GoalPlateOrientation(2);
	}
}

} // namespace c3pick
