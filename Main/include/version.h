#pragma once

#include "include/marshalling.hpp"

namespace c3picko {
class Version {
 public:
  typedef QString ID;
  Version(ID id, QDateTime date);

  ID id() const;
  QDateTime date() const;

 private:
  ID id_;
  QDateTime date_;
};
MAKE_MARSHALLABLE(Version)
}  // namespace c3picko
