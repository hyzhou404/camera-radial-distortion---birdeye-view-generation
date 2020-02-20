#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>
#include <fstream>
#define CP1X 410
#define CP1Y 297
#define CP2X 590
#define CP2Y 297
#define CP3X 120
#define CP3Y 430
#define CP4X 907
#define CP4Y 430

#define WP1X 960/2 - 200
#define WP1Y 540/2 - 150
#define WP2X 960/2 + 200
#define WP2Y 540/2 - 150
#define WP3X 960/2 - 200
#define WP3Y 540/2 + 150
#define WP4X 960/2 + 200
#define WP4Y 540/2 + 150

using namespace std;
using namespace cv;

int main() {
	Mat cameraMatrix;
	Mat distCoeffs;
	FileStorage intrLoadFile("intrinsics.xml", FileStorage::READ);
	intrLoadFile["cameraMatrix"] >> cameraMatrix;
	intrLoadFile["distCoeffs"] >> distCoeffs;
	intrLoadFile.release();

	VideoCapture capture("video.mp4");

	Mat orign_frame, frame, undistort_frame, birdeye_view;

	// check if capture was successful
	if (!capture.isOpened()) throw "Error reading video";

	while (true) {
		capture.read(orign_frame);
		// check if we succeeded
		if (orign_frame.empty()) {
			cout << "video end!";
			break;
		}
		resize(orign_frame, frame, Size(960, 540));

		// undistort the frame
		undistort(frame, undistort_frame, cameraMatrix, distCoeffs);

		// sign points
		/*circle(undistort_frame, Point(CP1X, CP1Y), 3, Scalar(0, 0, 255), -1);
		circle(undistort_frame, Point(CP2X, CP2Y), 3, Scalar(0, 0, 255), -1);
		circle(undistort_frame, Point(CP3X, CP3Y), 3, Scalar(0, 0, 255), -1);
		circle(undistort_frame, Point(CP4X, CP4Y), 3, Scalar(0, 0, 255), -1);*/

		// generate bird-eye view
		vector<Point2f> cameraConers = { Point2f(CP1X, CP1Y), Point2f(CP2X, CP2Y), Point2f(CP3X, CP3Y), Point2f(CP4X, CP4Y) };
		vector<Point2f> worldConers = { Point2f(WP1X, WP1Y), Point2f(WP2X, WP2Y), Point2f(WP3X, WP3Y), Point2f(WP4X, WP4Y) };
		Mat matrix = getPerspectiveTransform(cameraConers, worldConers);
		warpPerspective(undistort_frame, birdeye_view, matrix, Size(960, 540));

		// show live and wait for a key with timeout long enough to show images
		imshow("undistort", undistort_frame);
		imshow("bird-eye view", birdeye_view);
		int key = waitKey(1);
		if (key > 0) {
			break;
		}
	}
	system("pause");
	return 0;
}
