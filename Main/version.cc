#include "include/version.h"
#include "include/exception.h"

namespace c3picko {
Version::Version(Version::ID id, QDateTime date)
    : id_(id), date_(date), state_(State::NEW) {}

Version::Version(Version::ID id, QDateTime date, ResourcePath source_dir,
                 ResourcePath build_dir, State type)
    : id_(id),
      date_(date),
      source_dir_(source_dir),
      build_dir_(build_dir),
      state_(type) {}

QString Version::id() const { return id_; }

template <>
QJsonObject Marshalling::toJson(const Version::State& value) {
  QJsonObject obj;

  obj["enum"] = toString(value);

  return obj;
}

template <>
Version::State Marshalling::fromJson(const QJsonObject& obj) {
  return fromString(obj["enum"].toString());
}

template <>
QJsonObject Marshalling::toJson(const Version& value) {
  QJsonObject obj;

  obj["hash"] = value.id();
  obj["date"] = Marshalling::toJson(value.date());
  obj["source_dir"] = Marshalling::toJson(value.sourceDir());
  obj["build_dir"] = Marshalling::toJson(value.buildDir());
  obj["state"] = toString(value.state());

  return obj;
}

template <>
Version Marshalling::fromJson(const QJsonObject& obj) {
  return Version(obj["hash"].toString(),
                 Marshalling::fromJson<QDateTime>(obj["date"]),
                 Marshalling::fromJson<ResourcePath>(obj["source_dir"]),
                 Marshalling::fromJson<ResourcePath>(obj["build_dir"]),
                 Marshalling::fromJson<Version::State>(obj["state"]));
}

QDateTime Version::date() const { return date_; }

Version::State Version::state() const { return state_; }

void Version::setState(const Version::State& state) { state_ = state; }

const ResourcePath& Version::sourceDir() const { return source_dir_; }

const ResourcePath& Version::buildDir() const { return build_dir_; }

QString toString(Version::State state) {
  switch (state) {
    case Version::State::NEW:
      return "NEW";
    case Version::State::READY:
      return "READY";
    case Version::State::CORRUPT:
      return "CORRUPT";

    case Version::State::MARKED_FOR_CLONE:
      return "MARKED_FOR_CLONE";
    case Version::State::CLONING:
      return "CLONING";
    case Version::State::CLONED:
      return "CLONED";
    case Version::State::CLONE_ERROR:
      return "CLONE_ERROR";

    case Version::State::MARKED_FOR_CHECKOUT:
      return "MARKED_FOR_CHECKOUT";
    case Version::State::CHECKING_OUT:
      return "CHECKING_OUT";
    case Version::State::CHECKED_OUT:
      return "CHECKED_OUT";
    case Version::State::CHECK_OUT_ERROR:
      return "CHECK_OUT_ERROR";

    case Version::State::MARKED_FOR_BUILD:
      return "MARKED_FOR_BUILD";
    case Version::State::BUILDING:
      return "BUILDING";
    case Version::State::BUILT:
      return "BUILT";
    case Version::State::BUILD_ERROR:
      return "BUILD_ERROR";

    default:
      throw Exception("Unknown enum value '" + QString::number(int(state)) +
                      " for enum of type Version::Type");
  }
}

Version::State fromString(QString string) {
  if (string == "NEW") return Version::State::NEW;
  if (string == "READY") return Version::State::READY;
  if (string == "CORRUPT") return Version::State::CORRUPT;

  if (string == "MARKED_FOR_CLONE") return Version::State::MARKED_FOR_CLONE;
  if (string == "CLONING") return Version::State::CLONING;
  if (string == "CLONED") return Version::State::CLONED;
  if (string == "CLONE_ERROR") return Version::State::CLONE_ERROR;

  if (string == "MARKED_FOR_CHECKOUT")
    return Version::State::MARKED_FOR_CHECKOUT;
  if (string == "CHECKING_OUT") return Version::State::CHECKING_OUT;
  if (string == "CHECKED_OUT") return Version::State::CHECKED_OUT;
  if (string == "CHECK_OUT_ERROR") return Version::State::CHECK_OUT_ERROR;

  if (string == "MARKED_FOR_BUILD") return Version::State::MARKED_FOR_BUILD;
  if (string == "BUILDING") return Version::State::BUILDING;
  if (string == "BUILT") return Version::State::BUILT;

  throw Exception("Unknown value '" + string + "' for enum Version::State");
}

}  // namespace c3picko
