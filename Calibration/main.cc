#include <iomanip>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <stack>

using namespace std;
using namespace cv;

const std::string root = "/home/vados/src/bc_pics/calibration/";

Mat calc_mat() {}

int main(int argc, char** argv)
{
	std::stack<std::string> image_paths;
	for (int i = 1; i < argc; ++i)
	{
		image_paths.push(root + argv[i]);
	}

	// cv::VideoCapture cam(1);

	bool done = false;
	Mat  view, grey, remapped;
	Size pattern(7, 7);
	int  good = 0;

	std::vector<Point3f> object_blueprint;
	for (int i = 0; i < pattern.height; ++i)
		for (int j = 0; j < pattern.width; ++j)
			object_blueprint.emplace_back(i * 2.3, j * 2.3, 0);

	std::vector<std::vector<Point3f>> object_points;
	std::vector<std::vector<Point2f>> image_points;
	Mat								  undistored, mapx, mapy;
	Mat								  camera, dist, optimised;

	while (!done && !image_paths.empty())
	{
		// cam.read(view);
		view = imread(image_paths.top());
		// resize(view, view, Size(view.cols / 2, view.rows / 2));
		std::cerr << "Reading image " << image_paths.top() << "\n";
		image_paths.pop();

		std::vector<Point2f> corners;
		int					 w = view.rows, h = view.cols;
		cv::cvtColor(view, grey, CV_BGR2GRAY);

		bool found
			= findChessboardCorners(grey, pattern, corners, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK);

		if (found)
		{
			++good;
			cornerSubPix(grey, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, .1));

			std::vector<Mat> rvecs;
			std::vector<Mat> tvecs;

			image_points.push_back(corners);
			object_points.push_back(object_blueprint);

			camera		  = Mat();
			double thresh = calibrateCamera(object_points, image_points, pattern, camera, dist, rvecs, tvecs);
			optimised	 = getOptimalNewCameraMatrix(camera, dist, {w, h}, 1, {w, h});

			initUndistortRectifyMap(camera, dist, Mat(), optimised, {w, h}, CV_16SC2, mapx, mapy);
			undistort(view, undistored, camera, dist);

			// remapped = Mat();
			// remap(view, remapped, mapx, mapy, INTER_LINEAR);

			std::cerr << "Thresh " << std::setprecision(3) << thresh << std::endl;

			drawChessboardCorners(view, pattern, Mat(corners), found);
			drawChessboardCorners(undistored, pattern, Mat(corners), found);

			imshow("orig", view);
			imshow("undistord", undistored);
			// Mat grid = imread(root + "hig_colonies.png");
			// undistort(grid, undistored, camera, dist, camera);
			// imwrite(root + "hit_colonies_transform.jpg", undistored);

			// imshow("remapped", remapped);
			// waitKey(25);
			// while (waitKey() != 'q')
			//;
		}
		else
			std::cerr << "Bad Image\n";
		// imshow("frame", view);
		// waitKey(25);
	}
	undistort(imread(root + "grid.png"), undistored, camera, dist);
	imwrite(root + "out_grid.png", undistored);

	std::cout << camera;

	return 0;
}
>>>>>>> 3c287fdaf8b072277ea804555fdf8cb856f5d227
