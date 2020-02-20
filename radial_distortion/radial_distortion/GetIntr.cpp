#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>
#include <fstream>

#define PATH "photos/"
#define NUM 20

using namespace std;
using namespace cv;

int main() {
	// save loaded imgs
	Mat image_in_orign;
	Mat image_in;
	// save file path
	vector<string> filelist;
	// save rotations and transitions
	vector<Mat> rvecs, tvecs;
	Mat cameraMatrix;
	Mat distCoeffs;
	int flags = 0;
	vector<Point2f> corners;
	vector<vector<Point2f> > corners2;
	vector<Point3f> worldPoints;
	vector<vector<Point3f> > worldPoints2;

	/****************************************************************/
	/*-------------      Read all images in path     ---------------*/
	/****************************************************************/
	for (int i = 1; i <= NUM; i++) {
		stringstream ss;
		ss << PATH << "photo (" << i << ")" << ".jpg";
		filelist.push_back(ss.str());
	}

	/****************************************************************/
	/*----------     Generate a set of object_points       ---------*/
	/****************************************************************/
	for (int j = 0; j < 6; j++) {
		for (int k = 0; k < 8; k++) {
			worldPoints.push_back(Point3f(j*1.0, k*1.0, 0.0f));
		}
	}

	/****************************************************************/
	/*-------------         Find Conner Point        ---------------*/
	/****************************************************************/
	for (int i = 0; i < filelist.size(); i++) {
		image_in_orign = imread(filelist[i]);
		resize(image_in_orign, image_in, Size(960, 540));
		bool found = findChessboardCorners(image_in, Size(8, 6), corners, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
		corners2.push_back(corners);
		drawChessboardCorners(image_in, Size(8, 6), corners, found);
		worldPoints2.push_back(worldPoints);
	}
	
	/****************************************************************/
	/*--------------         Calibrate Camera        ---------------*/
	/****************************************************************/
	calibrateCamera(worldPoints2, corners2, image_in.size(), cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_FIX_PRINCIPAL_POINT);

	FileStorage intrSaveFile("../generate_birdeye_view/intrinsics.xml", FileStorage::WRITE);
	intrSaveFile << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
	intrSaveFile.release();

	system("pause");
}