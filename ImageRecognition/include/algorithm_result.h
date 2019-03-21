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
 * @brief Represents a failed or succeeded AlgorithmJob.
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
  bool cleanup_succeeded_;

  /**
   * @brief Last stage that the job was in.
   */
  int last_stage_;
  QString stage_error_, cleanup_error_;

  ID id() const;
  bool stagesSucceeded() const;
  bool cleanupSucceeded() const;
  int lastStage() const;
  quint64 tookNs() const;
  QString cleanupError() const;
  QString stageError() const;

  std::list<cv::Mat *> const &stack() const;

private:
  std::list<cv::Mat *> stack_;
};
MAKE_MARSHALLABLE(AlgorithmResult);
} // namespace c3picko
