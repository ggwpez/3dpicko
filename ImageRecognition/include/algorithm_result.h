#pragma once

#include "include/colony.hpp"
#include <QString>
#include <vector>

namespace cv {
class Mat;
}
namespace c3picko {
class Colony;
/**
 * @brief Represents a failed or succeeded job.
 */
class AlgorithmResult {
public:
  typedef QString ID;

  AlgorithmResult(ID id);
  ~AlgorithmResult();

  cv::Mat &newMat();
  cv::Mat &newMat(cv::Mat const &copy_from);

  cv::Mat &oldMat();

public:
  ID id_;
  /**
   * @brief Explicit cleanup for debugging
   */
  void cleanup();

  /**
   * @brief The job succeeded
   */
  bool stages_succeeded_;
  bool cleanup_succseeded_;

  /**
   * @brief Last stage that the job was in.
   */
  int last_stage_;
  QString stage_error_, cleanup_error_;

  /**
   * @brief Amount of nanoseconds the job took.
   */
  quint64 took_ns_;

  struct {
  } plate_; // TODO
  std::vector<Colony> colonies_;

  ID id() const;
  bool stagesSucceeded() const;
  bool cleanupSuccseeded() const;
  int lastStage() const;
  quint64 tookNs() const;
  std::vector<Colony> colonies() const;
  QString cleanupError() const;
  QString stageError() const;

private:
  std::list<cv::Mat *> stack_;
};
MAKE_MARSHALLABLE(AlgorithmResult);
}
