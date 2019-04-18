#include "include/types/profile.h"
#include <QDebug>
#include <QJsonArray>
#include "include/marshalling.h"
#include "include/octoconfig.h"
#include "include/plateprofile.h"
#include "include/platesocketprofile.h"
#include "include/printerprofile.h"

namespace c3picko {
Profile::Profile(ProfileType type, QString name, ID id,
                 QJsonObject const& settings)
    : type_(type), name_(name), id_(id) {
  // NOTE if you add something here, also add it down at toJson
  if (type_ == ProfileType::PRINTER) {
    printer_ = std::make_shared<PrinterProfile>(
        Marshalling::fromJson<PrinterProfile>(settings));
  } else if (type_ == ProfileType::SOCKET) {
    socket_ = std::make_shared<PlateSocketProfile>(
        Marshalling::fromJson<PlateSocketProfile>(settings));
  } else if (type_ == ProfileType::PLATE) {
    plate_ = std::make_shared<PlateProfile>(
        Marshalling::fromJson<PlateProfile>(settings));
  } else if (type_ == ProfileType::OCTOPRINT) {
    octoprint_ = std::make_shared<pi::OctoConfig>(
        Marshalling::fromJson<pi::OctoConfig>(settings));
  } else {
    plate_ = nullptr;
    qWarning() << "Unknown profile type" << (int)type_;
  }
}

Profile::ID Profile::id() const { return id_; }

void Profile::setId(Profile::ID id) { id_ = id; }

QString Profile::name() const { return name_; }

ProfileType Profile::type() const { return type_; }

std::shared_ptr<PlateProfile> Profile::plate() const { return plate_; }

std::shared_ptr<PlateSocketProfile> Profile::socket() const { return socket_; }

std::shared_ptr<PrinterProfile> Profile::printer() const { return printer_; }

std::shared_ptr<pi::OctoConfig> Profile::octoprint() const {
  return octoprint_;
}

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

  json["settings"] = {
      {makeNumberField("movement_speed", "Movement speed", "", 1, 10800, 1,
                       10800, 10800, "mm/60 s" /* Minute is not a SI unit*/),
       makeVec3Field("cut_filament_position_above_trigger", "Cut position", "",
                     NULL_POINT, PR_MAX, .01, NULL_POINT, NULL_POINT, "mm"),
       makeNumberField("z_coordinate_pushing_the_trigger", "Push trigger at",
                       "", -300, 300, .01, 0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_move_cut_to_pick",
                       "Filament length cut to source", "", -100, 100, .01, 0,
                       0, "mm"),
       makeNumberField("filament_extrusion_length_on_move_pick_to_master",
                       "Filament length source to master", "", -100, 100, .01,
                       0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_move_master_to_goal",
                       "Filament length master to target", "", -100, 100, .01,
                       0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_move_goal_to_cut",
                       "Filament length target to cut", "", -100, 100, .01, 0,
                       0, "mm"),
       makeNumberField("filament_extrusion_length_on_pick",
                       "Filament length pick from source", "", -300, 300, .01,
                       0, 0, "mm"),
       makeNumberField("filament_extrusion_length_on_put_onto_master",
                       "Filament length put onto master", "", -100, 100, .01, 0,
                       0, "mm"),
       makeNumberField("filament_extrusion_length_after_cut",
                       "Filament length after cut", "", -100, 100, .01, 0, 0,
                       "mm"),
       makeNumberField("length_of_removed_filament",
                       "Filament length to be cut", "", 0, 100, .01, 0, 0,
                       "mm"),
       makeNumberField("safety_distance_between_top_surface_of_all_plates_and_"
                       "nozzle_on_move",
                       "Safety distance between nozzle and plates", "", -100,
                       100, .01, 0, 0, "mm")}};

  return json;
}

QJsonObject Profile::socketTemplate() {
  QJsonObject json;

  json["settings"] = {
      {makeVec2Field("global_origin_of_source_plate",
                     "Source plate cutout origin", "", NULL_POINT, PR_MAX, .01,
                     NULL_POINT, NULL_POINT, "mm"),
       makeVec2Field("global_origin_of_master_plate",
                     "Master plate cutout origin", "", NULL_POINT, PR_MAX, .01,
                     NULL_POINT, NULL_POINT, "mm"),
       makeVec2Field("global_origin_of_goal_plate",
                     "Target plate cutout origin", "", NULL_POINT, PR_MAX, .01,
                     NULL_POINT, NULL_POINT, "mm"),
       makeEnumField("orientation_of_goal_plate",
                     "Target and master plate orientation", "",
                     {{"kFirstRowFirstColumnAtCutoutOrigin",
                       "Well 'A1' at cutout origin."},
                      {"kLastRowFirstColumnAtCutoutOrigin",
                       "Well 'm1' at cutout origin. (m=last row)"}},
                     "kFirstRowFirstColumnAtCutoutOrigin"),
       makeNumberField("depth_of_cutout_the_source_plate_lies_in",
                       "Source plate cutout depth", "", 0, 300, .01, 0, 0,
                       "mm"),
       makeNumberField("depth_of_cutout_the_master_plate_lies_in",
                       "Master plate cutout depth", "", 0, 300, .01, 0, 0,
                       "mm"),
       makeNumberField("depth_of_cutout_the_goal_plate_lies_in",
                       "Target plate cutout depth", "", 0, 300, .01, 0, 0,
                       "mm"),
       makeVec3Field("socket_origin_offset", "Socket origin offset", "",
                     NULL_POINT, PR_MAX, .01, NULL_POINT, NULL_POINT, "mm")}};

  return json;
}

QJsonObject Profile::plateTemplate() {
  QJsonObject json;

  json["settings"] = {
      {makeNumberField("number_of_rows", "Number of rows", "", 1, 100, 1, 0, 0,
                       ""),
       makeNumberField("number_of_columns", "Number of columns", "", 1, 100, 1,
                       0, 0, ""),
       makeNumberField("a1_row_offset", "A1 row offset", "", -1000, 1000, .01,
                       0, 0, "mm"),
       makeNumberField("a1_column_offset", "A1 column offset", "", -1000, 1000,
                       .01, 0, 0, "mm"),
       makeNumberField("well_spacing_center_to_center",
                       "Well spacing center to center", "", -100, 100, .01, 0,
                       0, "mm"),
       makeNumberField("height_source_plate", "Source plate height", "", -100,
                       100, .01, 0, 0, "mm"),
       makeNumberField("height_master_plate", "Master plate height", "", -100,
                       100, .01, 0, 0, "mm"),
       makeNumberField("height_goal_plate", "Target plate height", "", -100,
                       100, .01, 0, 0, "mm"),
       makeNumberField("well_depth", "Well depth", "", 0, 100, .01, 0, 0, "mm"),
       makeNumberField("culture_medium_thickness_source_plate",
                       "Source medium thickness", "", 0, 200, .01, 0, 0, "mm"),
       makeNumberField("culture_medium_thickness_master_plate",
                       "Master medium thickness", "", 0, 200, .01, 0, 0,
                       "mm")}};

  return json;
}

QJsonObject Profile::octoprintTemplate() {
  QJsonObject json;

  json["settings"] = {
      {makeTextField("address", "Hostname",
                     "IPv4/IPv6/Hostname of the OctoPrint server. Probably a "
                     "Raspberry Pi.",
                     "", ""),
       makeTextField("api_key", "ApiKey", "ApiKey of the OctoPrint", "", "", 32,
                     32),
       makeTextField("protocol", "Protocol", "", "http", "http", 3, 12),
       makeNumberField("port", "Port", "Port of the OctoPrint", 0, 65535, 1, 80,
                       80, "")}};

  return json;
}

c3picko::Profile::operator PlateProfile*() const { return plate_.get(); }

c3picko::Profile::operator PlateSocketProfile*() const { return socket_.get(); }

c3picko::Profile::operator PrinterProfile*() const { return printer_.get(); }

c3picko::Profile::operator pi::OctoConfig*() const { return octoprint_.get(); }

template <>
QJsonObject Marshalling::toJson(const Profile& value) {
  QJsonObject obj;

  // TODO also dumb
  QJsonObject settings;
  if (value.type() == ProfileType::PRINTER)
    settings = Marshalling::toJson(*value.printer());
  else if (value.type() == ProfileType::SOCKET)
    settings = Marshalling::toJson(*value.socket());
  else if (value.type() == ProfileType::PLATE)
    settings = Marshalling::toJson(*value.plate());
  else if (value.type() == ProfileType::OCTOPRINT)
    settings = Marshalling::toJson(*value.octoprint());
  else
    qWarning() << "Cant write profile of unknown type";

  obj["settings"] = settings;
  obj["type"] = profileToString(value.type());
  obj["profile_name"] = value.name();
  obj["id"] = value.id();

  return obj;
}

template <>
Profile Marshalling::fromJson(const QJsonObject& obj) {
  return Profile(profileFromString(obj["type"].toString()),
                 Marshalling::fromJson<QString>(obj["profile_name"]),
                 Marshalling::fromJson<QString>(obj["id"]),
                 obj["settings"].toObject());
}

}  // namespace c3picko
