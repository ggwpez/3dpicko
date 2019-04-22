#include "Gcode/gcodeinstruction.h"

#include <sstream>

namespace c3picko {

/**
 * @brief Class for creating g-code command fields in RepRap style.
 */
class GcodeInstruction::GcodeField::Command {
 public:
  /**
   * Creates and returns the g-code command G1 used for moving the nozzle.
   */
  static GcodeField G1() { return GcodeInstruction::GcodeField('G', 1); }
  /**
   * Creates and returns the g-code command G4 used for having the nozzle
   * wait on the spot.
   */
  static GcodeField G4() { return GcodeInstruction::GcodeField('G', 4); }
  /**
   * Creates and returns the g-code command G21 used for setting
   * the units of all coordinates to millimeter.
   */
  static GcodeField G21() { return GcodeInstruction::GcodeField('G', 21); }
  /**
   * Creates and returns the g-code command G28 used for homing
   * all axes.
   */
  static GcodeField G28() { return GcodeInstruction::GcodeField('G', 28); }
  /**
   * Creates and returns the g-code command G90 used for setting the
   * positioning of the nozzle to absolute.
   */
  static GcodeField G90() { return GcodeInstruction::GcodeField('G', 90); }
  /**
   * Creates and returns the g-code command G92 used for gauging
   * the length of the extruded filament
   */
  static GcodeField G92() { return GcodeInstruction::GcodeField('G', 92); }
  /**
   * Creates and returns the g-code command M82 used for setting the
   * extrusion to absolute mode.
   */
  static GcodeField M82() { return GcodeInstruction::GcodeField('M', 82); }
  /**
   * Creates and returns the g-code command M302 used for setting up
   * cold extrusion.
   */
  static GcodeField M302() { return GcodeInstruction::GcodeField('M', 302); }
};

/**
 * @brief Class for creating g-code parameter fields.
 */
class GcodeInstruction::GcodeField::Parameter {
 public:
  /**
   * @brief Creates and returns the Xnnn parameter used for telling
   * the nozzle on which x position it shall be moved to.
   * @param x_coordinate The x coordinate the nozzle should be moved to.
   * Valid values range from 0 to 210.
   * @return the g-code field Xnnn
   */
  static GcodeField X(const float x_coordinate) {
    return GcodeInstruction::GcodeField('X', x_coordinate);
  }

  /**
   * @brief Creates and returns the Ynnn parameter used for telling
   * the nozzle on which y position it shall be moved to.
   * @param y_coordinate The y coordinate the nozzle should be moved to.
   * Valid values range from 0 to 255.
   * @return the g-code field Ynnn
   */
  static GcodeField Y(const float y_coordinate) {
    return GcodeInstruction::GcodeField('Y', y_coordinate);
  }

  /**
   * @brief Creates and returns the Znnn parameter used for telling
   * the nozzle on which x position it shall be moved to.
   * @param z_coordinate The z coordinate the nozzle should be moved to.
   * Valid values range from 0 to 210.
   * @return the g-code field Znnn
   */
  static GcodeField Z(const float z_coordinate) {
    return GcodeInstruction::GcodeField('Z', z_coordinate);
  }

  /**
   * @brief Creates and returns the Ennn parameter used for telling
   * the nozzle the length the filament should be extruded to.
   * @param e_filament_extrusion_length The length the filament should be
   * extruded to in millimeter. Valid values range from 0 to 210.
   * @return the g-code field Ennn
   */
  static GcodeField E(const float e_filament_extrusion_length) {
    return GcodeInstruction::GcodeField('E', e_filament_extrusion_length);
  }

  /**
   * @brief Creates and returns the Fnnn parameter used for setting
   * the movement speed of the 3D printer's nozzle
   * @param f_nozzle_movement_speed The movement speed of the 3D printer's
   * nozzle. Valid values range from 0 to 27000 millimeter/minute.
   * @return the g-code field Fnnn
   */
  static GcodeField F(const int f_nozzle_movement_speed) {
    return GcodeInstruction::GcodeField('F', f_nozzle_movement_speed);
  }

  /**
   * @brief Creates and returns the Snnn parameter used for having the nozzle
   * wait  on spot for a certain time.
   * @param s_waiting_time The time in seconds the nozzle of the 3D printer
   * should wait on spot.
   * @return the g-code field Snnn
   */
  static GcodeField S(const int s_waiting_time) {
    return GcodeInstruction::GcodeField('S', s_waiting_time);
  }

  /**
   * @brief Creates and returns the Pnnn parameter used for enabling
   * and disabling cold extrusion.
   * @param flag Allow extrusion at any temperature.
   * @return the g-code field Pnnn
   */
  static GcodeField P(const int flag) {
    return GcodeInstruction::GcodeField('P', flag);
  }
};

std::string GcodeInstruction::GcodeField::ToString() const {
  std::stringstream ss;
  ss << letter_ << number_;
  return ss.str();
}

GcodeInstruction::GcodeField::GcodeField(char letter, float number)
    : letter_(letter), number_(number) {}

GcodeInstruction GcodeInstruction::Home() {
  return GcodeInstruction({GcodeField::Command::G28()});
}

GcodeInstruction GcodeInstruction::SetUnit() {
  return GcodeInstruction({GcodeField::Command::G21()});
}

GcodeInstruction GcodeInstruction::AbsolutePositioning() {
  return GcodeInstruction({GcodeField::Command::G90()});
}

GcodeInstruction GcodeInstruction::ExtrusionModeAbsolute() {
  return GcodeInstruction({GcodeField::Command::M82()});
}

GcodeInstruction GcodeInstruction::MoveToXYZ(const float x_coordinate,
                                             const float y_coordinate,
                                             const float z_coordinate) {
  return GcodeInstruction({GcodeField::Command::G1(),
                           GcodeField::Parameter::X(x_coordinate),
                           GcodeField::Parameter::Y(y_coordinate),
                           GcodeField::Parameter::Z(z_coordinate)});
}

GcodeInstruction GcodeInstruction::MoveToXY(const float x_coordinate,
                                            const float y_coordinate) {
  return GcodeInstruction({
      GcodeField::Command::G1(),
      GcodeField::Parameter::X(x_coordinate),
      GcodeField::Parameter::Y(y_coordinate),
  });
}

GcodeInstruction GcodeInstruction::MoveToZ(const float z_coordinate) {
  return GcodeInstruction(
      {GcodeField::Command::G1(), GcodeField::Parameter::Z(z_coordinate)});
}

GcodeInstruction GcodeInstruction::ExtrudeFilament(
    const float e_filament_extrusion_length) {
  return GcodeInstruction(
      {GcodeField::Command::G1(),
       GcodeField::Parameter::E(e_filament_extrusion_length)});
}

GcodeInstruction GcodeInstruction::SetMovementSpeed(
    const int f_nozzle_movement_speed) {
  return GcodeInstruction({GcodeField::Command::G1(),
                           GcodeField::Parameter::F(f_nozzle_movement_speed)});
}

GcodeInstruction GcodeInstruction::Wait(const int s_waiting_time) {
  return GcodeInstruction(
      {GcodeField::Command::G4(), GcodeField::Parameter::S(s_waiting_time)});
}

GcodeInstruction GcodeInstruction::AllowColdExtrusion() {
  return GcodeInstruction(
      {GcodeField::Command::M302(), GcodeField::Parameter::P(1)});
}

GcodeInstruction GcodeInstruction::GaugeFilamentExtrusionLength(
    const float current_extrusion_length) {
  return GcodeInstruction({GcodeField::Command::G92(),
                           GcodeField::Parameter::E(current_extrusion_length)});
}

GcodeInstruction::GcodeInstruction(std::vector<GcodeField> fields)
    : fields_(fields) {}

std::string GcodeInstruction::ToString() const {
  std::stringstream ss;
  for (const GcodeField &field : fields_) ss << field.ToString() << " ";
  return ss.str();
}
}  // namespace c3picko
