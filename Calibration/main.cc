#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
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

int usage(int argc, char **argv) {
  for (int i = 1; i < argc; ++i)
    qDebug().nospace() << "Arg " << i << ": " << argv[i];

  qCritical() << "Usage examples:\n"
              << "Main --detect <dir>\n"
              << "Main --transform <dir>";

  return 0;
}

template <typename T>
QJsonObject toJson(cv::Mat mat) {
  QJsonObject ret;
  ret["cols"] = mat.cols;
  ret["rows"] = mat.rows;
  ret["type"] = mat.type();

  QJsonArray data;
  for (int r = 0; r < mat.rows; ++r)
    for (int c = 0; c < mat.cols; ++c) {
      data.push_back(mat.at<T>(r, c));
    }
  ret["data"] = data;

  return ret;
}

template <typename T>
cv::Mat matFromJson(QJsonObject obj) {
  cv::Mat ret(obj["rows"].toInt(), obj["cols"].toInt(), obj["type"].toInt());

  QJsonArray data = obj["data"].toArray();
  for (int r = 0; r < ret.rows; ++r)
    for (int c = 0; c < ret.cols; ++c) {
      ret.at<T>(r, c) = data[c + r * ret.cols].toVariant().value<T>();
    }

  return ret;
}

int detect(QStringList images) {
  const cv::Size pattern(7, 7);
  std::vector<std::vector<Point3f>> object_points;
  std::vector<std::vector<Point2f>> image_points;
  Mat undistored, mapx, mapy;
  Mat camera, dist, optimised;
  int good = 0, used = 0, all = images.size();

  std::vector<Point3f> object_blueprint;
  for (int i = 0; i < pattern.height; ++i)
    for (int j = 0; j < pattern.width; ++j)
      object_blueprint.emplace_back(i * 2.3, j * 2.3, 0);  // Size in cm

  while (!images.empty() && ++used) {
    std::cerr << "Reading " << images.first().toStdString() << "\n";
    // cam.read(view);
    Mat image = imread(images.first().toStdString()), grey;
    resize(image, image, Size(image.cols / 2, image.rows / 2));
    // qWarning() << "Reading image " << images.first();
    images.pop_front();

    std::vector<Point2f> corners;
    int w = image.cols, h = image.rows;
    cv::cvtColor(image, grey, CV_BGR2GRAY);

    bool found = findChessboardCorners(grey, pattern, corners,
                                       CALIB_CB_ADAPTIVE_THRESH |
                                           CALIB_CB_NORMALIZE_IMAGE |
                                           CALIB_CB_FAST_CHECK);

    if (found) {
      cornerSubPix(grey, corners, Size(11, 11), Size(-1, -1),
                   TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, .001));

      std::vector<Mat> rvecs;  // translation
      std::vector<Mat> tvecs;  // rotation

      image_points.push_back(corners);
      object_points.push_back(object_blueprint);

      camera = Mat();
      double thresh = calibrateCamera(object_points, image_points, {w, h},
                                      camera, dist, rvecs, tvecs);
      cv::Rect roi{0, 0, 0, 0};
      optimised = getOptimalNewCameraMatrix(camera, dist, {w, h}, 1 /* TODO */,
                                            {w, h}, &roi);
      std::cerr << "Reprojection error " << thresh << "\n";

      ++good;

      // drawChessboardCorners(view, pattern, Mat(corners), found);
      // drawChessboardCorners(undistored, pattern, Mat(corners), found);

      // imshow("remapped", remapped);
      // waitKey(25);
      // while (waitKey() != 'q')
      //;
    } else
      ;  // qWarning() << "Bad Image";
  }

  QJsonObject ret;
  ret["camera"] = toJson<double>(camera);
  ret["dist"] = toJson<double>(dist);
  ret["optimised"] = toJson<double>(optimised);
  std::cout << QJsonDocument(ret).toJson().toStdString() << "\n";

  std::cerr << "camera\n"
            << camera << "\ndist\n"
            << dist << "\noptimised\n"
            << optimised << "\n";
  std::cerr << "Images: Used " << used << ", Good " << good << ", All " << all
            << "\n";

  return 0;
}

/**
 * @brief Test: Lowers side ratio error for detected plate by 0.08-0.17%
 */
int transform(QStringList images) {
  QTextStream in(stdin);
  QJsonParseError error;
  QJsonObject json = QJsonDocument::fromJson(qPrintable(in.readAll()), &error)
                         .object();  // wtf

  if (error.error != QJsonParseError::NoError) {
    qFatal("Json: %s", qPrintable(error.errorString()));
    return 1;
  }

  cv::Mat camera, dist, optimised;
  camera = matFromJson<double>(json["camera"].toObject());
  dist = matFromJson<double>(json["dist"].toObject());
  optimised = matFromJson<double>(json["optimised"].toObject());
  std::cerr << "camera " << camera << "\ndist\n"
            << dist << "\noptimised\n"
            << optimised << "\n";

  while (!images.empty()) {
    cv::Mat img, undistorted;

    img = cv::imread(images.front().toStdString());
    cv::undistort(img, undistorted, camera, dist, optimised);

    QString new_name(images.front() + "_undistored");
    cv::imwrite(new_name.toStdString(), undistorted);
    std::cerr << "Wrote " << new_name.toStdString() << "\n";
    images.pop_front();
  }

  return 0;
}

int main(int argc, char **argv) {
  if (argc != 3) return usage(argc, argv);
  std::string mode(argv[1]);

  QStringList images;
  for (int i = 2; i < argc; ++i) {
    QString path(argv[2]);
    QDirIterator it(path);

    while (it.hasNext()) {
      QString path = it.next();

      if (path.toLower().endsWith(".jpg")) images << path;
    }
  }

  if (mode == "--detect") return detect(images);
  if (mode == "--transform")
    return transform(images);
  else
    return usage(argc, argv);
}
