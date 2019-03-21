#include "include/types/profile.hpp"
#include "include/marshalling.hpp"
#include "include/plateprofile.h"
#include "include/platesocketprofile.h"
#include "include/printerprofile.h"
#include <QDebug>
#include <QJsonArray>

namespace c3picko {
Profile::Profile(QString type, QString name, ID id, QJsonObject const &settings)
    : type_(type), name_(name), id_(id) {
  if (type_ == "printer-profile") {
    printer_ = std::make_shared<PrinterProfile>(
        Marshalling::fromJson<PrinterProfile>(settings));
  } else if (type_ == "socket-profile") {
    socket_ = std::make_shared<PlateSocketProfile>(
        Marshalling::fromJson<PlateSocketProfile>(settings));
  } else if (type_ == "plate-profile") {
    plate_ = std::make_shared<PlateProfile>(
        Marshalling::fromJson<PlateProfile>(settings));
  } else {
    plate_ = nullptr;
    qWarning() << "Unknown profile type" << type_;
  }
}

Profile::ID Profile::id() const { return id_; }

void Profile::setId(Profile::ID id) { id_ = id; }

QString Profile::name() const { return name_; }

QString Profile::type() const { return type_; }

std::shared_ptr<PlateProfile> Profile::plate() const { return plate_; }

std::shared_ptr<PlateSocketProfile> Profile::socket() const { return socket_; }

std::shared_ptr<PrinterProfile> Profile::printer() const { return printer_; }

QJsonObject Profile::makeTextField(Profile::ID id, QString name,
                                   QString description, QString placeholder,
                                   QString defaultValue, int min, int max) {
  return QJsonObject({{"id", id},
                      {"name", name},
                      {"type", "text"},
                      {"placeholder", placeholder},
                      {"defaultValue", defaultValue},
                      {"description", description},
                      {"min", min},
                      {"max", max}});
}

QJsonObject Profile::makeNumberField(Profile::ID id, QString name,
                                     QString description, double min,
                                     double max, double step,
                                     double placeholder, double defaultValue,
                                     QString unit) {
  return QJsonObject({{"id", id},
                      {"name", name},
                      {"type", "number"},
                      {"placeholder", placeholder},
                      {"defaultValue", defaultValue},
                      {"description", description},
                      {"min", min},
                      {"max", max},
                      {"step", step},
                      {"unit", unit}});
}

QJsonObject Profile::makeVec2Field(Profile::ID id, QString name,
                                   QString description, Point min, Point max,
                                   double step, Point placeholder,
                                   Point defaultValue, QString unit) {
  return makeVecField(id, name, 2, description, min, max, step, placeholder,
                      defaultValue, unit);
}

QJsonObject Profile::makeVec3Field(Profile::ID id, QString name,
                                   QString description, Point min, Point max,
                                   double step, Point placeholder,
                                   Point defaultValue, QString unit) {
  return makeVecField(id, name, 3, description, min, max, step, placeholder,
                      defaultValue, unit);
}

QJsonObject Profile::makeVecField(Profile::ID id, QString name, int vec_size,
                                  QString description, Point min, Point max,
                                  double step, Point placeholder,
                                  Point defaultValue, QString unit) {
  return QJsonObject({{"id", id},
                      {"name", name},
                      {"type", "vector" + QString::number(vec_size)},
                      {"placeholder", Marshalling::toJson(placeholder)},
                      {"defaultValue", Marshalling::toJson(defaultValue)},
                      {"description", description},
                      {"min", Marshalling::toJson(min)},
                      {"max", Marshalling::toJson(max)},
                      {"step", step},
                      {"unit", unit}});
}

QJsonObject Profile::makeEnumField(Profile::ID id, QString name,
                                   QString description, QVariantMap options,
                                   QString defaultValue) {
  return QJsonObject({{"id", id},
                      {"name", name},
                      {"type", "radio"},
                      {"description", description},
                      {"defaultValue", defaultValue},
                      {"options", QJsonObject::fromVariantMap(options)}});
}

#define PR_MAX Point(300, 300)
#define NULL_POINT Point(0, 0)

QJsonObject Profile::printerTemplate() {
  QJsonObject json;

  // TODO new members dont yet have descriptions
  json["settings"] = {
      {makeNumberField("movement_speed", "Movement speed",
                       "The speed the nozzle is moved with.", 1, 10800, 1,
                       10800, 10800, "mm/60 s" /* Minute is not a SI unit*/),
       makeVec3Field("cut_filament_position_above_trigger", "Cut position",
                     "The (x,y,z) position, in the coordinate system of the "
                     "printer, the nozzle needs to move to in order to be "
                     "above the trigger of the scissor and directly above the "
                     "center of the space between the scissors blades.",
                     NULL_POINT, PR_MAX, .01, NULL_POINT, NULL_POINT, "mm"),
       makeNumberField("z_coordinate_pushing_the_trigger", "Push trigger at",
                       "The global z coordinate (at x and y of cut-position) "
                       "the trigger of the scissor is definitely pushed.",
                       -300, 300, .01, 0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_move_cut_to_pick",
                       "filament_extrusion_length_on_move_cut_to_pick", "",
                       -100, 100, .01, 0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_move_pick_to_master",
                       "filament_extrusion_length_on_move_pick_to_master", "",
                       -100, 100, .01, 0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_move_master_to_goal",
                       "filament_extrusion_length_on_move_master_to_goal", "",
                       -100, 100, .01, 0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_move_goal_to_cut",
                       "filament_extrusion_length_on_move_goal_to_cut", "",
                       -100, 100, .01, 0, 0, "mm"),
       makeNumberField(
           "filament_extrusion_length_on_pick", "Filament offset (pick)",
           "Offset to the length up to which the filament will be extruded "
           "when picking from source- and placing on masterplate.",
           -300, 300, .01, 0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_put_onto_master",
                       "filament_extrusion_length_on_put_onto_master", "", -100,
                       100, .01, 0, 0, "mm"),
       makeNumberField("filament_extrusion_length_after_cut",
                       "filament_extrusion_length_after_cut", "", -100, 100,
                       .01, 0, 0, "mm"),
       makeNumberField("length_of_removed_filament",
                       "length_of_removed_filament", "", 0, 100, .01, 0, 0,
                       "mm"),
       makeNumberField("safety_distance_between_top_surface_of_all_plates_and_"
                       "nozzle_on_move",
                       "safety_distance_between_top_surface_of_all_plates_and_"
                       "nozzle_on_move",
                       "", -100, 100, .01, 0, 0, "mm")}};

  return json;
}

QJsonObject Profile::socketTemplate() {
  QJsonObject json;

  json["settings"] = {
      {makeVec2Field("global_origin_of_source_plate",
                     "Source plate cutout origin",
                     "The (x,y) origin of the slot/cut-out of the source plate "
                     "given as a point of the coordinate system of the socket.",
                     NULL_POINT, PR_MAX, .01, NULL_POINT, NULL_POINT, "mm"),
       makeVec2Field("global_origin_of_master_plate",
                     "Master plate cutout origin",
                     "The (x,y) origin of the slot/cut-out of the master plate "
                     "given as a point of the coordinate system of the socket.",
                     NULL_POINT, PR_MAX, .01, NULL_POINT, NULL_POINT, "mm"),
       makeVec2Field("global_origin_of_goal_plate", "Goal plate cutout origin",
                     "The (x,y) origin of the slot/cut-out of the goal plate "
                     "given as a point of the coordinate system of the socket.",
                     NULL_POINT, PR_MAX, .01, NULL_POINT, NULL_POINT, "mm"),
       makeEnumField("orientation_of_goal_plate",
                     "Goal and master plate orientation",
                     "Orientation of goal- and masterplate compared to the "
                     "cutout it is lying in.",
                     {{"kFirstRowFirstColumnAtCutoutOrigin",
                       "Well 'A1' at cutout origin."},
                      {"kLastRowFirstColumnAtCutoutOrigin",
                       "Well 'm1' at cutout origin. (m=last row)"}},
                     "kFirstRowFirstColumnAtCutoutOrigin"),
       makeNumberField("depth_of_cutout_the_source_plate_lies_in",
                       "Source plate cutout depth",
                       "The source plate cutout depth.", 0, 300, .01, 0, 0,
                       "mm"),
       makeNumberField("depth_of_cutout_the_master_plate_lies_in",
                       "Master plate cutout depth",
                       "The Master plate cutout depth.", 0, 300, .01, 0, 0,
                       "mm"),
       makeNumberField("depth_of_cutout_the_goal_plate_lies_in",
                       "Goal plate cutout depth",
                       "The goal plate cutout depth.", 0, 300, .01, 0, 0, "mm"),
       makeVec3Field("socket_origin_offset", "Socket origin offset",
                     "The (x,y,z) offset of the socket origin (with z-origin "
                     "on socket surface) to the origin of the printers "
                     "coordinate system (home).",
                     NULL_POINT, PR_MAX, .01, NULL_POINT, NULL_POINT, "mm")}};

  return json;
}

QJsonObject Profile::plateTemplate() {
  QJsonObject json;

  json["settings"] = {
      {makeNumberField("number_of_rows", "Number of rows",
                       "Number of rows the goal plate has.", 1, 100, 1, 0, 0,
                       ""),
       makeNumberField("number_of_columns", "Number of columns",
                       "Number of columns the goal plate has.", 1, 100, 1, 0, 0,
                       ""),
       makeNumberField("a1_row_offset", "A1 row offset",
                       "The offset of the center of the first well A1 to the "
                       "upper edge of the goal plate.",
                       -1000, 1000, .01, 0, 0, "mm"),
       makeNumberField("a1_column_offset", "A1 column offset",
                       "The offset of the center of the first well A1 to the "
                       "left edge of the goal plate.",
                       -1000, 1000, .01, 0, 0, "mm"),
       makeNumberField("well_spacing_center_to_center",
                       "Well spacing center to center",
                       "The distance between the center of a well to the "
                       "center of any directly adjacent well.",
                       -100, 100, .01, 0, 0, "mm"),
       makeNumberField("height_source_plate", "Source plate height",
                       "The height of the source plate.", -100, 100, .01, 0, 0,
                       "mm"),
       makeNumberField("height_master_plate", "Master plate height",
                       "The height of the master plate.", -100, 100, .01, 0, 0,
                       "mm"),
       makeNumberField("height_goal_plate", "Goal plate height",
                       "The height of the goal plate.", -100, 100, .01, 0, 0,
                       "mm"),
       makeNumberField("well_depth", "Well depth", "The depth of every well.",
                       0, 100, .01, 0, 0, "mm"),
       makeNumberField(
           "culture_medium_thickness_source_plate", "Source medium thickness",
           "The Thickness of the culture medium in the source plate.", 0, 200,
           .01, 0, 0, "mm"),
       makeNumberField(
           "culture_medium_thickness_master_plate", "Master medium thickness",
           "The Thickness of the culture medium in the master plate.", 0, 200,
           .01, 0, 0, "mm")}};

  return json;
}

c3picko::Profile::operator PlateProfile *() const { return plate_.get(); }

c3picko::Profile::operator PlateSocketProfile *() const {
  return socket_.get();
}

c3picko::Profile::operator PrinterProfile *() const { return printer_.get(); }

template <> QJsonObject Marshalling::toJson(const Profile &value) {
  QJsonObject obj;

  // TODO also dumb
  QJsonObject settings;
  if (value.type() == "printer-profile")
    settings = Marshalling::toJson(*value.printer());
  else if (value.type() == "socket-profile")
    settings = Marshalling::toJson(*value.socket());
  else if (value.type() == "plate-profile")
    settings = Marshalling::toJson(*value.plate());
  else
    qWarning() << "Cant write profile of unknown type";

  obj["settings"] = settings;
  obj["type"] = value.type();
  obj["profile_name"] = value.name();
  obj["id"] = value.id();

  return obj;
}

template <> Profile Marshalling::fromJson(const QJsonObject &obj) {
  return Profile(Marshalling::fromJson<QString>(obj["type"]),
                 Marshalling::fromJson<QString>(obj["profile_name"]),
                 Marshalling::fromJson<QString>(obj["id"]),
                 obj["settings"].toObject());
}

} // namespace c3picko
