#pragma once

#include "include/colony.hpp"
#include <opencv2/core.hpp>

#include "include/algorithm.h"

namespace c3picko
{
class Algo1Test : public Algorithm
{
	Q_OBJECT
  public:
	Algo1Test();
	virtual ~Algo1Test() override;

	static void cvt(Algorithm* base, const cv::Mat* input, cv::Mat** output);
	static void threshold(Algorithm* base, const cv::Mat* input, cv::Mat** output);
	static void erodeAndDilate(Algorithm* base, const cv::Mat* input, cv::Mat** output);
	static void label(Algorithm* base, const cv::Mat* input, std::vector<Colony>** output);
	static void relativeFiltering(Algorithm* base, std::vector<Colony>* input, std::vector<Colony>** output);

	static void cleanup(Algorithm* base);

	virtual inline Algorithm* cloneEmpty() const override { return new Algo1Test(); }

	void render_rrect(cv::Mat& out, cv::RotatedRect rect);
	void drawText(cv::Mat& img, cv::Mat& output, std::vector<cv::Vec3f>& colonies);

  private:
};
}
