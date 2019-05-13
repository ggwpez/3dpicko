#pragma once

#include <QDateTime>
#include <QStack>
#include <QString>
#include "GUI/types/image.h"
#include "GUI/types/profile.h"
#include "ImageRecognition/algorithm_result.h"
#include "Main/marshalling.h"

namespace c3picko {
class Report;
class AlgorithmJob;
/**
 * @brief A pick job, will be filled with data step by step by the use throught
 * the GUI.
 */
class Job {
 public:
  typedef QString ID;
  Job() = default;
  Job(ID id, Image::ID img_id, QString name, QString description,
	  QDateTime created, QStack<AlgorithmResult::ID> results,
	  Profile::ID printer, Profile::ID socket, int starting_row,
	  int starting_col, int step, ResourcePath report_path);

 public:
  Image::ID imgID() const;
  ID id() const;
  qint32 step() const;
  QDateTime created() const;
  QString name() const;
  QString description() const;

  void setId(ID id);
  void setCreationDate(QDateTime);
  void setReportPath(ResourcePath const&);
  ResourcePath reportPath() const;

  Profile::ID socket() const;
  Profile::ID printer() const;
  Profile::ID plate() const;
  Profile::ID octoprint() const;

  AlgorithmResult::ID resultID() const;
  QStack<AlgorithmResult::ID> resultIDs() const;
  std::shared_ptr<AlgorithmJob> resultJob() const;
  /**
   * @brief TODO this is messy
   */
  void setResultAlgorithmJob(std::shared_ptr<AlgorithmJob> job);
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
  std::shared_ptr<AlgorithmJob> result_job_;
  QString name_, description_;
  QDateTime created_;
  QStack<AlgorithmResult::ID> result_ids_;
  Profile::ID printer_, socket_, plate_, octoprint_;
  int starting_row_, starting_col_;
  QSet<Colony::ID> colonies_to_pick_;
  /**
   * @brief How far is the configuration of this Job?
   */
  qint32 step_ = 0;
  ResourcePath report_path_;
};
MAKE_MARSHALLABLE(Job);
}  // namespace c3picko
