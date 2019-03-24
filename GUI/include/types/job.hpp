#pragma once

#include <QDateTime>
#include <QStack>
#include <QString>
#include "include/algorithm_result.h"
#include "include/marshalling.hpp"
#include "include/types/image.hpp"
#include "include/types/profile.hpp"

namespace c3picko {
class Job {
 public:
  typedef QString ID;
  Job() = default;
  Job(ID id, Image::ID img_id, QString name, QString description,
      QDateTime job_created, QStack<AlgorithmResult::ID> results,
      Profile::ID printer, Profile::ID socket, int starting_row,
      int starting_col, int step);

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
  Profile::ID octoprint() const;

  AlgorithmResult::ID resultID() const;
  QStack<AlgorithmResult::ID> resultIDs() const;
  void setResultID(const AlgorithmResult::ID& resultID);

  int startingCol() const;
  void setStartingCol(int startingCol);

  int startingRow() const;
  void setStartingRow(int startingRow);

  void setPlate(const Profile::ID& plate);
  void setOctoprint(const Profile::ID& octoprint);

  QSet<Colony::ID> coloniesToPick() const;
  void setcoloniesToPick(const QSet<Colony::ID>& coloniesToPick);

 private:
  ID id_;
  Image::ID img_id_;
  /**
   * @brief Saves a history of all past detections.
   * .top() is the current result.
   */
  QStack<AlgorithmResult::ID> result_ids_;
  QString name_, description_;
  QDateTime job_created_;
  Profile::ID plate_, printer_, socket_, octoprint_;
  int starting_row_, starting_col_;
  QSet<Colony::ID> colonies_to_pick_;

  /**
   * @brief How far is the configuration of this Job?
   */
  qint32 step_ = 0;
};
MAKE_MARSHALLABLE(Job);
}  // namespace c3picko
