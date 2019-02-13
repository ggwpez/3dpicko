#ifndef COLONYDETECTION_H
#define COLONYDETECTION_H

#include <opencv2/core/core.hpp>

#include <QElapsedTimer>
#include <QRunnable>

#include <string>

#include "conversion.h"
#include "detectionsettings.h"
#include "include/colony.hpp"
#include "include/colony_type.h"
#include "preprocessing.h"
#include "test.h"

namespace c3picko {

/**
 * @file colonydetection.h
 * @author Mayer, Patrick
 * @brief The Colonydetection class is responsible for detecting colonies.
 */
class ColonyDetector : public QObject, public QRunnable {
  Q_OBJECT
public:
  ColonyDetector(cv::Mat source, QObject *_parent = nullptr);

  void run() override;

signals:
  void OnFinished(qint64 elapsed_ms);

public:
  /**
   * @brief The algorithmHoughCircles function
   * @param src: Image of colonies
   * @param settings_json: Custom settings which should be applied to the image
   * for better results.
   * @return void
   */
  void algoFlour1(std::string settings_json);

  std::vector<Colony> colonies() const;
  std::vector<cv::Vec3f> pos() const;

private:
  cv::Mat &new_stage();
  cv::Mat &new_stage(cv::Mat const &src);
  void render_rrect(cv::Mat &out, cv::RotatedRect rect);
  void drawText(cv::Mat &img, std::vector<cv::Vec3f> &colonies);
  bool roundness(cv::Mat &out, int area, int w, int h,
                 std::vector<cv::Point> const &contour, double roundness);

protected:
  /**
   * @brief Indicates wheter the class was already executed.
   */
  bool did_run_;
  cv::Mat source_;
  std::vector<Colony> colonies_;
  std::vector<cv::Vec3f> pos_;
  std::list<cv::Mat> stages_;
  QElapsedTimer time_;
};
} // namespace c3picko

#endif // COLONYDETECTION_H
