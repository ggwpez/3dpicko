#include "../include/colonydetector.h"
#include "../include/test.h"
#include <list>
#include <set>
#include <unordered_set>

using namespace c3picko;

namespace c3picko {
ColonyDetector::ColonyDetector(cv::Mat source, Algorithm *algo,
                               QObject *_parent)
    : QObject(_parent), did_run_(false), source_(source), algo_(algo) {}

ColonyDetector::~ColonyDetector() { delete algo_; }

void ColonyDetector::run() {
  Q_ASSERT(!did_run_ && "Dont call more than once!");
  did_run_ = true;
  time_.start();

  algo_->run(&source_, &colonies_);

  pos_.resize(colonies_.size());
  for (std::size_t i = 0; i < colonies_.size(); ++i)
    pos_[i] = cv::Vec3d(colonies_[i].x(), colonies_[i].y(),
                        colonies_[i].circumference() / (2 * M_PI));

  emit OnFinished(time_.elapsed());
}

std::vector<Colony> ColonyDetector::colonies() const { return colonies_; }

std::vector<cv::Vec3f> ColonyDetector::pos() const { return pos_; }
} // namespace c3picko
