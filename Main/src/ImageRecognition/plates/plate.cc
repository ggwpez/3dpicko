#include "ImageRecognition/plates/plate.h"
#include <QDebug>
#include <opencv2/imgproc.hpp>

namespace c3picko {
Plate::Plate(cv::Point2d center, double angle, cv::Rect bb)
    : center_(center),
      angle_(angle),
      aabb_(bb),
      rotation_matrix_(cv::getRotationMatrix2D(center_, angle_, 1)) {}

void Plate::crop(const Plate& original, const Plate& rotated, const cv::Mat& in,
                 cv::Mat& out) {
  cv::warpAffine(in, out, original.rotationMatrix(),
                 cv::Size(in.cols, in.rows));
  out = out(rotated.aabb());
}

cv::Point2d Plate::center() const { return center_; }

cv::Mat const& Plate::rotationMatrix() const { return rotation_matrix_; }

double Plate::angle() const { return angle_; }

cv::Rect Plate::aabb() const { return aabb_; }
}  // namespace c3picko
