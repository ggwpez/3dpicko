#include "include/resource_path.h"
#include "include/exception.h"

namespace c3picko {
ResourcePath::ResourcePath(QString system_absolute_path)
    : system_absolute_(system_absolute_path) {}

ResourcePath::ResourcePath() {}

ResourcePath ResourcePath::fromSystemAbsolute(QString path) { return path; }

// We have two // now, but the operator+ manages that
ResourcePath ResourcePath::fromServerAbsolute(QString path) {
  return Root() + path;
}

ResourcePath ResourcePath::fromServerRelative(QString path) {
  return Root() + "/" + path;
}

ResourcePath ResourcePath::fromDocRootAbsolute(QString path) {
  return DocRoot() + path;
}

const QString ResourcePath::toSystemAbsolute() const {
  if (system_absolute_.isEmpty()) throw Exception("Empty path");
  return system_absolute_;
}

const QString ResourcePath::toServerAbsolute() const {
  QString root_abs = Root().toSystemAbsolute();

  if (!system_absolute_.startsWith(root_abs))
    throw Exception(
        "System path outside of Root can not be converted to "
        "'ServerAbsolute': " +
        system_absolute_);

  return system_absolute_.mid(root_abs.size());
}

const QString ResourcePath::toDocRootAbsolute() const {
  QString droot_abs = DocRoot().toSystemAbsolute();

  if (!system_absolute_.startsWith(droot_abs))
    throw Exception(
        "System path outside of DocRoot can not be converted to "
        "'DocRootAbsolute': " +
        system_absolute_);

  return system_absolute_.mid(droot_abs.size());
}

bool ResourcePath::exists() const {
  return QFileInfo(system_absolute_).exists();
}

bool ResourcePath::isDir() const { return QFileInfo(system_absolute_).isDir(); }

bool ResourcePath::isFile() const {
  return QFileInfo(system_absolute_).isFile();
}

bool ResourcePath::isEmpty() const { return system_absolute_.isEmpty(); }

void ResourcePath::clear() { system_absolute_ = ""; }

ResourcePath ResourcePath::operator+(const QString& obj) const {
  QString infix;
  if (!obj.startsWith(QDir::separator()) &&
      !system_absolute_.endsWith(QDir::separator()))
    infix = QDir::separator();

  QString result = system_absolute_ + infix + obj;
  return QDir::cleanPath(result);
}

template <>
ResourcePath Marshalling::fromJson(const QJsonObject& obj) {
  return ResourcePath::fromServerAbsolute(obj["server_absolute"].toString());
}

template <>
QJsonObject Marshalling::toJson(const ResourcePath& value) {
  QJsonObject obj;

  obj["server_absolute"] = value.toServerAbsolute();

  return obj;
}
}  // namespace c3picko
