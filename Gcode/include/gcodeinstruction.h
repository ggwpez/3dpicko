#ifndef GCODEINSTRUCTION_H
#define GCODEINSTRUCTION_H

#include <string>
#include <vector>

namespace c3picko {

/**
 * @file gcodeinstruction.h
 * @author flopicko
 * @brief Class representing a single g-code instruction
 * in RepRap style.
 *
 */
class GcodeInstruction {
 public:
  /**
   * @brief Home Creates a gcode instruction for homing all axes
   * @return a gcode instruction of the form "G28"
   */
  static GcodeInstruction Home();

  /**
   * @brief Home Creates a gcode instruction for setting the units
   * of all coordinates to millimeter
   * @return a gcode instruction of the form "G21"
   */
  static GcodeInstruction SetUnit();

  /**
   * @brief Home Creates a gcode instruction for setting the
   * positioning of the nozzle to absolute
   * @return a gcode instruction of the form "G90"
   */
  static GcodeInstruction AbsolutePositioning();

  /**
   * @brief Home Creates a gcode instruction for setting the
   * extrusion to absolute mode.
   * @return a gcode instruction of the form "M82"
   */
  static GcodeInstruction ExtrusionModeAbsolute();
  /**
   * @brief Creates a gcode instruction for moving the nozzle
   * to a new xyz Coordinate. The length of filament and
   * the movement speed of the nozzle are not changed.
   * @param x_coordinate a floating point number between 0 and 210
   * @param y_coordinate a floating point number between 0 and 255
   * @param z_coordinate a floating point number 0 and 210
   * @return a gcode instruction of the form "G1 Xx_coordinate Yy_coordinate
   * Zz_coordinate", x_coordinate, y_coordinate, z_coordinate standing for the
   * actual values
   */
  static GcodeInstruction MoveToXYZ(const float x_coordinate,
                                    const float y_coordinate,
                                    const float z_coordinate);

  /**
   * @brief Creates a gcode instruction for moving the nozzle
   * to a new xy Coordinate. The length of filament, the z-position
   * and the movement speed of the nozzle are not changed.
   * @param x_coordinate a floating point number between 0 and 210
   * @param y_coordinate a floating point number between 0 and 255
   * @return a gcode instruction of the form "G1 Xx_coordinate Yy_coordinate",
   * x_coordinate, y_coordinate standing for the actual values
   */
  static GcodeInstruction MoveToXY(const float x_coordinate,
                                   const float y_coordinate);

  /**
   * @brief Creates a gcode instruction for moving the nozzle
   * to a new z Coordinate. The length of filament, the x- and y-position
   * and the movement speed of the nozzle are not changed.
   * @param z_coordinate a floating point number between 0 and 255
   * @return a gcode instruction of the form "G1 Zz_coordinate",
   * z_coordinate standing for the actual value
   */
  static GcodeInstruction MoveToZ(const float z_coordinate);

  /**
   * @brief Creates a gcode instruction for extruding the filament
   * by the given length. The x-, y-, z-position
   * and the movement speed of the nozzle are not changed.
   * @param e_filament_extrusion_length the desired length of the extruded
   * filament
   * @return a gcode instruction of the form "G1 Ee_filament_extrusion_length",
   * e_filament_extrusion_length standing for the actual value
   */
  static GcodeInstruction ExtrudeFilament(
      const float e_filament_extrusion_length);

  /**
   * @brief Creates a gcode instruction for changing the movement speed
   * of the nozzle to the given one. The x-, y-, z-position
   * of the nozzle and the length of the extruded filament
   * are not changed.
   * @param f_nozzle_movement_speed the desired movement speed of the nozzle, in
   * mm/min
   * @return a gcode instruction of the form "G1 Ff_nozzle_movement_speed",
   * f_nozzle_movement_speed standing for the actual value
   */
  static GcodeInstruction SetMovementSpeed(const int f_nozzle_movement_speed);

  /**
   * @brief Creates a gcode instruction for having the nozzle wait on spot.
   * @param s_waiting_time the desired waiting time in seconds.
   * @return a gcode instruction of the form "G4 Ss_waiting_time",
   * s_waiting_time standing for the actual value
   */
  static GcodeInstruction Wait(const int s_waiting_time);

  /**
   * @brief Creates a gcode instruction for allowing cold extrusion.
   * @return a gcode instruction of the form "M302 P1",
   */
  static GcodeInstruction AllowColdExtrusion();

  /**
   * @brief Creates a gcode instruction for gauging
   * the length of the extruded filament, for instance
   * setting the current extrusion length to zero
   * @param current_extrusion_length the current length of the extruded filament
   * @return a gcode instruction of the form "G92 E0"
   */
  static GcodeInstruction GaugeFilamentExtrusionLength(
      const float current_extrusion_length);

  /**
   * Returns a string representation of
   * this single gcode command, such as
   * "G1 X35 Y120 Z320".
   */
  std::string ToString() const;

 private:
  /**
   * @brief Class representing single g-code field in RepRap style.
   * A field is in most cases a command or a parameter.
   */
  class GcodeField {
   public:
    /**
     * @brief Class for creating g-code command fields in RepRap style.
     */
    class Command;

    /**
     * @brief Class for creating g-code parameter fields.
     */
    class Parameter;

    /**
     * Returns a string representation of this single field,
     * a concatination of letter_ and number_,
     * such as "G1" or "X32.4"
     */
    std::string ToString() const;

   private:
    explicit GcodeField(char letter, float number);
    /**
     * @brief letter_ the letter of this field
     */
    const char letter_;
    /**
     * @brief number_ the number of this field,
     * can be an integer or floating point number,
     * depending on what is allowed for a certain
     * letter.
     */
    const float number_;
  };

  /**
   * @brief Creates a g_code instruction consisting of the given fields.
   * @param fields the fields the g-code instruction should consists of
   * @return a g-code instruction in RepRap style
   */
  explicit GcodeInstruction(std::vector<GcodeField> fields);

  /**
   * @brief fields_ the list of fields
   * this g-code instruction consists of.
   */
  std::vector<GcodeField> fields_;
};
}  // namespace c3picko
#endif  // GCODEINSTRUCTION_H
