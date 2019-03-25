#include "include/algorithm_result.h"
#include <QJsonArray>
#include <QThread>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include "include/colony.hpp"
#include "include/exception.h"

namespace c3picko {
AlgorithmResult::AlgorithmResult(ID id)
    : id_(id),
      stages_succeeded_(false),
      cleanup_succeeded_(false),
      is_finalized_(false) {}

AlgorithmResult::~AlgorithmResult() { cleanup(); }

cv::Mat& AlgorithmResult::newMat(cv::Mat const& copy_from) {
  cv::Mat* new_mat = new cv::Mat();

  copy_from.copyTo(*new_mat);

  stack_.push_back(new_mat);
  return *new_mat;
}

cv::Mat& AlgorithmResult::oldMat() {
  if (!stack_.size() || !stack_.back())
    throw Exception("Assertion failure: Stack empty");
  return *stack_.back();
}

void AlgorithmResult::finalize() {
  if (is_finalized_) throw Exception("Cant finalize twice");
  is_finalized_ = true;
}

AlgorithmResult::ID AlgorithmResult::id() const { return id_; }

cv::Mat& AlgorithmResult::newMat() {
  cv::Mat* new_mat = new cv::Mat();

  stack_.push_back(new_mat);

  return *new_mat;
}

void AlgorithmResult::cleanup() {
  if (cleanup_succeeded_ || cleanup_error_.size()) return;
  /*std::string name;
int			i = 0;

for (void* stage : stack_)
{
                                                                                                                                    cv::Mat&
mat  = *reinterpret_cast<cv::Mat*>(stage); std::string name = "STAGE-" +
std::to_string(i++); if (!mat.cols || !mat.rows) continue;

                                                                                                                                    cv::namedWindow(name,
cv::WINDOW_NORMAL); cv::resizeWindow(name, 1920, 1080); cv::imshow(name, mat);
                                                                                                                                    cv::imwrite("lel"
+ name + ".png", mat);
}

while (cv::waitKey(0) != 'q')
                                                                                                                                    ;
cv::destroyAllWindows();*/

  QString error_prefix, error_infix, error_postfix, error;
  QTextStream(&error_prefix) << "Result "
                             << "c3picko::AlgorithmResult"
                             << " crashed (step=";
  QTextStream(&error_postfix)
      << ",thread=" << QThread::currentThreadId() << ") ";

  try {
    qDeleteAll(stack_);
    stack_.clear();
    cleanup_succeeded_ = true;
  } catch (std::exception const& e) {
    cleanup_error_ = error_prefix + "cleanup" + error_postfix + ": " + e.what();
    qCritical("%s", qPrintable(cleanup_error_));
    cleanup_succeeded_ = false;
  } catch (...) {
    cleanup_error_ = error_prefix + "cleanup" + error_postfix + ": unknown";
    qCritical("%s", qPrintable(cleanup_error_));
    cleanup_succeeded_ = false;
  }
}

QString AlgorithmResult::stageError() const { return stage_error_; }

const std::list<cv::Mat*>& AlgorithmResult::stack() const { return stack_; }

QString AlgorithmResult::cleanupError() const { return cleanup_error_; }

int AlgorithmResult::lastStage() const { return last_stage_; }

bool AlgorithmResult::cleanupSucceeded() const { return cleanup_succeeded_; }

bool AlgorithmResult::stagesSucceeded() const { return stages_succeeded_; }

template <>
QJsonObject Marshalling::toJson(const AlgorithmResult& value) {
  QJsonObject obj;

  obj["id"] = value.id();
  obj["stages_succeeded"] = value.stagesSucceeded();
  obj["cleanup_succeeded"] = value.cleanupSucceeded();
  obj["last_stage"] = value.lastStage();
  obj["stage_error"] = value.stageError();
  obj["cleanup_error"] = value.cleanupError();

  return obj;
}

template <>
AlgorithmResult Marshalling::fromJson(const QJsonObject& obj) {
  throw Exception("Not implemented");
}
}  // namespace c3picko
