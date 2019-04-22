#pragma once

#include "Main/marshalling.h"
#include "Main/resource_path.h"

namespace c3picko {
class Version {
 public:
  enum class State {
    // Version is newly retrived from the server
    NEW,
    // Built and rdy to use
    READY,
    // Something beside the cloning or building went wrong
    CORRUPT,

    MARKED_FOR_CLONE,
    CLONING,
    CLONED,
    CLONE_ERROR,

    MARKED_FOR_CHECKOUT,
    CHECKING_OUT,
    CHECKED_OUT,
    CHECK_OUT_ERROR,

    MARKED_FOR_BUILD,
    BUILDING,
    BUILT,
    BUILD_ERROR
  };

  typedef QString ID;
  /**
   * @brief type=Type::NEW
   */
  Version(ID id, QDateTime date);
  Version(ID id,
          QDateTime date, /*ResourcePath sourceDir, ResourcePath buildDir,*/
          State state);

  ID id() const;
  QDateTime date() const;
  State state() const;
  void setState(State const&);

  // ResourcePath const& sourceDir() const;
  // ResourcePath const& buildDir() const;

 private:
  ID id_;
  QDateTime date_;
  // ResourcePath source_dir_;
  // ResourcePath build_dir_;
  State state_;
};
QString toString(Version::State state);  // TODO
Version::State versionStateFromString(QString string);

MAKE_MARSHALLABLE(Version)
}  // namespace c3picko
