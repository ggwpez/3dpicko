#ifndef PLATESOCKETPROFILE_H
#define PLATESOCKETPROFILE_H

#include "include/point.h"
#include "include/json_constructable.hpp"

namespace c3picko {

/**
 * @brief The GoalPlateOrientation enum Specifying which corner
 * of the goal plate is at the origin of the cutout it is bordered in.
 */
enum GoalPlateOrientation {
  /** well A1 is at the origin of the cutout */
  kFirstRowFirstColumnAtCutoutOrigin,
  /** well m1 is at the origin of the cutout,
	  where m stands for the last row */
  //kFirstRowLastColumnAtCutoutOrigin, // makes no sense
  /** well An is at the origin of the cutout,
	  where n stands for the last column */
  kLastRowFirstColumnAtCutoutOrigin,
  /** well mn is at the origin of the cutout,
	  where m stands for the last row and
	  n stands for the last column */
  //kLastRowLastColumnAtCutoutOrigin // makes no sense
};

GoalPlateOrientation fromJson(const QJsonValue& obj);

/**
 * @file platesocketprofile.h
 * @author flopicko
 * @brief Class representing the needed characteristics of
 * the used plate socket.
 *
 */
class PlateSocketProfile : public JsonConstructable {
 public:
explicit PlateSocketProfile(QJsonObject const& obj);
  explicit PlateSocketProfile(const Point& global_origin_of_source_plate,
							  const Point& global_origin_of_master_plate,
							  const Point& global_origin_of_goal_plate,
							  GoalPlateOrientation orientation_of_goal_plate,
							  float socket_origin_offset_x = 0.0,
							  float socket_origin_offset_y = 0.0);

  float originOffsetX() const;
  float originOffsetY() const;
  Point originOfSourcePlate() const;
  Point originOfMasterPlate() const;
  Point originOfGoalPlate() const;
  GoalPlateOrientation orientationOfGoalPlate() const;

  void write(QJsonObject&) const;

 private:
  /**
   * @brief global_origin_of_source_plate_ the origin of
   * the slot/cut-out of the source plate given as a point
   * of the coordinate system of the socket
   */
  const Point global_origin_of_source_plate_;

  /**
   * @brief global_origin_of_master_plate_ the origin of
   * the slot/cut-out of the master plate given as a point
   * of the coordinate system of the socket
   */
  const Point global_origin_of_master_plate_;

  /**
   * @brief global_origin_of_goal_plate_ the origin of
   * the slot/cut-out of the goal plate given as a point
   * of the coordinate system of the socket
   */
  const Point global_origin_of_goal_plate_;

  /**
   * @brief orientation_of_goal_plate_ the orientation of the goal plate
   * compared to the cutout it is lying in.
   */
  const GoalPlateOrientation orientation_of_goal_plate_;

  /**
   * @brief socket_origin_offset_x_ the offset of the origin
   * of the socket to the origin of the printers coordinate system
   * in x direction
   */
  const float socket_origin_offset_x_;

  /**
   * @brief socket_origin_offset_y_ the offset of the origin
   * of the socket to the origin of the printers coordinate system
   * in y direction
   */
  const float socket_origin_offset_y_;
};
} // namespace c3picko
#endif // PLATESOCKETPROFILE_H
