#pragma once

#include "include/algorithm_result.h"
#include "include/marshalling.hpp"
#include "include/types/image.hpp"
#include "include/types/profile.hpp"
#include <QDateTime>
#include <QString>

namespace c3picko {
class Job {
public:
  typedef QString ID;
  Job() = default;
  Job(ID id, Image::ID img_id, QString name, QString description,
      QDateTime job_created, Profile::ID printer, Profile::ID socket, int step);

public:
  Image::ID imgID() const;
  ID id() const;
  qint32 step() const;
  QDateTime job_created() const;
  QString name() const;
  QString description() const;

  void setId(ID id);
  void setCreationDate(QDateTime);

  Profile::ID socket() const;
  Profile::ID printer() const;
  Profile::ID plate() const;

  AlgorithmResult::ID resultID() const;
  void setResultID(const AlgorithmResult::ID &resultID);

private:
  ID id_;
  Image::ID img_id_;
  /**
   * @brief ID of the last detection process.
   */
  AlgorithmResult::ID result_id_;
  QString name_, description_;
  QDateTime job_created_;
  Profile::ID plate_, printer_, socket_;

  /**
   * @brief How far is the configuration of this Job?
   */
  qint32 step_ = 0;
};
MAKE_MARSHALLABLE(Job);
} // namespace c3picko
