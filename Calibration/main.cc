#include <iomanip>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

Mat calc_mat() {}

int main() {
  cv::VideoCapture cam(0);

  bool done = false;
  Mat view, grey;
  Size pattern(7, 7);
  int good = 0;

  std::vector<Point3f> object_blueprint;
  for (int i = 0; i < pattern.height; ++i)
	for (int j = 0; j < pattern.width; ++j)
	  object_blueprint.emplace_back(i, j, 0);

  std::vector<std::vector<Point3f>> object_points;
  std::vector<std::vector<Point2f>> image_points;

  object_points.push_back(object_blueprint);

  while (!done) {
	std::vector<Point2f> corners;
	cam.read(view);
	int w = view.rows, h = view.cols;
	cv::cvtColor(view, grey, CV_BGR2GRAY);

	bool found = findChessboardCorners(grey, pattern, corners,
									   CALIB_CB_ADAPTIVE_THRESH |
										   CALIB_CB_NORMALIZE_IMAGE |
										   CALIB_CB_FAST_CHECK);

	if (found) {
	  ++good;
	  cornerSubPix(grey, corners, Size(11, 11), Size(-1, -1),
				   TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, .1));

	  Mat camera, dist;
	  std::vector<Mat> rvecs;
	  std::vector<Mat> tvecs;

	  image_points = {corners};

	  double thresh = calibrateCamera(object_points, image_points, pattern,
									  camera, dist, rvecs, tvecs);
	  Mat optimised =
		  getOptimalNewCameraMatrix(camera, dist, {w, h}, 1, {w, h});

	  Mat undistored, mapx, mapy;
	  undistort(view, undistored, camera, dist, optimised);
	  initUndistortRectifyMap(camera, dist, Mat(), optimised, {w, h}, 5, mapx,
							  mapy);
	  Mat dst = view;
	  remap(view, dst, mapx, mapy, INTER_LINEAR);

	  view = dst;

	  std::cout << "Thresh " << std::setprecision(3) << thresh << std::endl;
	}
	drawChessboardCorners(view, pattern, Mat(corners), found);

	imshow("frame", view);
	waitKey(25);
  }

  return 0;
}
