#include "include/gcodegenerator.h"

namespace c3picko {

GcodeGenerator::GcodeGenerator(
    const PlateSocketProfile& plate_socket_profile,
    const PrinterProfile& printer_profile,
    const MasterAndGoalPlateProfile& master_and_goal_plate_profile)
    : plate_socket_profile_(plate_socket_profile),
      printer_profile_(printer_profile),
      master_and_goal_plate_profile_(master_and_goal_plate_profile),
      gcode_lower_filament_onto_colony_(CreateGcodeLowerFilamentOntoColony()),
      gcode_lower_filament_onto_master_(CreateGcodeLowerFilamentOntoMaster()),
      gcode_lower_filament_inside_well_(CreateGcodeLowerFilamentInsideWell()),
      gcode_raise_filament_above_plate_(CreateGcodeRaiseFilamentAbovePlate()),
      gcode_move_to_cut_filament_position_(
          CreateGcodeMoveToCutFilemantPosition()),
      gcode_extrude_new_filament_(CreateGcodeExtrudeNewFilament()),
      global_well_xy_coordinates_(ComputeGlobalWellCoordinates()),
      global_master_xy_coordinates_(ComputeGlobalMasterCoordinates()) {}

std::vector<GcodeInstruction>
GcodeGenerator::CreateGcodeForTheEntirePickingProcess(
    int row, int column,
    std::vector<LocalColonyCoordinates> local_colony_coordinates) {
  int starting_well = ComputeStartingWell(row, column);
  std::vector<GcodeInstruction> gcodes = Init();
  gcodes.reserve(local_colony_coordinates.size());
  gcodes.push_back(gcode_raise_filament_above_plate_);

  for (size_t i = 0; i < local_colony_coordinates.size(); ++i) {
    const GlobalColonyCoordinates global_colony_coordinate =
        MapLocalColonyCoordinateToGlobal(local_colony_coordinates.at(i));
    const GlobalWellCoordinates& global_well_coordinate =
        global_well_xy_coordinates_.at(starting_well + i);
    const GlobalMasterCoordinates& global_master_coordinate =
        global_master_xy_coordinates_.at(starting_well + i);

    gcodes.push_back(
        GcodeInstruction::MoveToXY(global_colony_coordinate.xCoordinate(),
                                   global_colony_coordinate.yCoordinate()));
    gcodes.push_back(gcode_extrude_new_filament_);
    gcodes.push_back(gcode_lower_filament_onto_colony_);
    gcodes.push_back(gcode_raise_filament_above_plate_);
    gcodes.push_back(
        GcodeInstruction::MoveToXY(global_master_coordinate.xCoordinate(),
                                   global_master_coordinate.yCoordinate()));
    gcodes.push_back(gcode_lower_filament_onto_master_);
    gcodes.push_back(gcode_raise_filament_above_plate_);
    gcodes.push_back(
        GcodeInstruction::MoveToXY(global_well_coordinate.xCoordinate(),
                                   global_well_coordinate.yCoordinate()));
    gcodes.push_back(gcode_lower_filament_inside_well_);
    gcodes.push_back(gcode_raise_filament_above_plate_);
    gcodes.push_back(gcode_move_to_cut_filament_position_);
  }

  return gcodes;
}

std::vector<GcodeInstruction> GcodeGenerator::Init() {
  std::vector<GcodeInstruction> gcode_init;
  gcode_init.reserve(4);
  gcode_init.push_back(GcodeInstruction::SetUnit());
  gcode_init.push_back(GcodeInstruction::AbsolutePositioning());
  gcode_init.push_back(GcodeInstruction::ExtrusionModeAbsolute());
  gcode_init.push_back(GcodeInstruction::Home());
  return gcode_init;
}

GlobalColonyCoordinates GcodeGenerator::MapLocalColonyCoordinateToGlobal(
    LocalColonyCoordinates& local_colony) {
  return GlobalColonyCoordinates(
      local_colony.xCoordinate() +
          plate_socket_profile_.originOfSourcePlate().xCoordinate(),
      local_colony.yCoordinate() +
          plate_socket_profile_.originOfSourcePlate().yCoordinate());
}

GcodeInstruction GcodeGenerator::CreateGcodeLowerFilamentOntoColony() {
  return GcodeInstruction::MoveToZ(
      printer_profile_.zCoordinateFilamentInsideSourcePlate());
}

GcodeInstruction GcodeGenerator::CreateGcodeLowerFilamentOntoMaster() {
  return GcodeInstruction::MoveToZ(
      printer_profile_.zCoordinateFilamentInsideMasterPlate());
}

GcodeInstruction GcodeGenerator::CreateGcodeLowerFilamentInsideWell() {
  return GcodeInstruction::MoveToZ(
      printer_profile_.zCoordinateFilamentInsideGoalPlate());
}

GcodeInstruction GcodeGenerator::CreateGcodeRaiseFilamentAbovePlate() {
  return GcodeInstruction::MoveToZ(
      printer_profile_.zCoordinateFilamentAbovePlates());
}

GcodeInstruction GcodeGenerator::CreateGcodeMoveToCutFilemantPosition() {
  return GcodeInstruction::MoveToXY(
      printer_profile_.cutFilamentPosition().xCoordinate(),
      printer_profile_.cutFilamentPosition().yCoordinate());
}

GcodeInstruction GcodeGenerator::CreateGcodeExtrudeNewFilament() {
  return GcodeInstruction::ExtrudeFilament(
      printer_profile_.filamentExtrusionLength());
}

std::vector<GlobalWellCoordinates>
GcodeGenerator::ComputeGlobalWellCoordinates() {
  return ComputeGlobalWellAndMasterCoordinates(
      plate_socket_profile_.originOfGoalPlate());
}

std::vector<GlobalMasterCoordinates>
GcodeGenerator::ComputeGlobalMasterCoordinates() {
  return ComputeGlobalWellAndMasterCoordinates(
      plate_socket_profile_.originOfMasterPlate());
}

std::vector<Point> GcodeGenerator::ComputeGlobalWellAndMasterCoordinates(
    const Point& origin_of_plate) {
  std::vector<GlobalWellCoordinates> global_well_coordinates;
  global_well_coordinates.reserve(
      master_and_goal_plate_profile_.numberOfWells());

  switch (plate_socket_profile_.orientationOfGoalPlate()) {
    case kFirstRowFirstColumnAtCutoutOrigin:
      ComputeGlobalCoordinatesFirstRowFirstColumnOrientation(
          global_well_coordinates, origin_of_plate);
      break;
    case kLastRowFirstColumnAtCutoutOrigin:
      ComputeGlobalCoordinatesLastRowFirstColumnOrientation(
          global_well_coordinates, origin_of_plate);
      break;
  }

  return global_well_coordinates;
}

void GcodeGenerator::ComputeGlobalCoordinatesFirstRowFirstColumnOrientation(
    std::vector<Point>& global_well_coordintes, const Point& origin_of_plate) {
  for (int i = 0; i < master_and_goal_plate_profile_.numberOfColumns(); ++i) {
    for (int j = 0; j < master_and_goal_plate_profile_.numberOfRows(); ++j) {
      global_well_coordintes.push_back(Point(
          master_and_goal_plate_profile_.a1RowOffset() +
              j * master_and_goal_plate_profile_.wellSpacingCenterToCenter() +
              origin_of_plate.xCoordinate(),
          master_and_goal_plate_profile_.a1ColumnOffset() +
              i * master_and_goal_plate_profile_.wellSpacingCenterToCenter() +
              origin_of_plate.yCoordinate()));
    }
  }
}

void GcodeGenerator::ComputeGlobalCoordinatesLastRowFirstColumnOrientation(
    std::vector<Point>& global_well_coordintes, const Point& origin_of_plate) {
  for (int i = 0; i < master_and_goal_plate_profile_.numberOfRows(); ++i) {
    for (int j = master_and_goal_plate_profile_.numberOfColumns() - 1; j >= 0;
         --j) {
      global_well_coordintes.push_back(Point(
          master_and_goal_plate_profile_.a1ColumnOffset() +
              i * master_and_goal_plate_profile_.wellSpacingCenterToCenter() +
              origin_of_plate.xCoordinate(),
          master_and_goal_plate_profile_.a1RowOffset() +
              j * master_and_goal_plate_profile_.wellSpacingCenterToCenter() +
              origin_of_plate.yCoordinate()));
    }
  }
}

int GcodeGenerator::ComputeStartingWell(int row, int column) const {
  return (column - 1) * master_and_goal_plate_profile_.numberOfRows() + row -
         1;  // -1 to accomodate to vector iterating starting from 0
}
}  // namespace c3picko
