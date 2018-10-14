#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "GPIOlib.h"

#define PI 3.1415926
#define PREFER_DELTA 0.09
#define PREFER_ANGLE_DELTA 0.09
#define ANGLE 10
#define BIG_ANGLE 20

//Uncomment this line at run-time to skip GUI rendering
#define _DEBUG

using namespace cv;
using namespace std;
using namespace GPIO;

const string CAM_PATH = "/devideo0";
const string MAIN_WINDOW_NAME = "Processed Image";
const string CANNY_WINDOW_NAME = "Canny";

const int CANNY_LOWER_BOUND = 50;
const int CANNY_UPPER_BOUND = 250;
const int HOUGH_THRESHOLD = 150;

const int REC_WIDTH = 500;
const int REC_HEIGHT = 500;


void adjust(float theta1,float theta2){
	float sum = theta1+theta2;
	if(sum<PI-PREFER_ANGLE_DELTA){
		turnTo(-ANGLE);
		delay(100);
		turnTo(0);
	}

	else if(sum>PI+PREFER_ANGLE_DELTA){
		turnTo(ANGLE);
		delay(100);
		turnTo(0);
	}
	else if (theta1 == 0){
		turnTo(-BIGANGLE);
		delay(100);
		turnTo(0);
	}
	else if (theta2 == 0){
		turnTo(BIGANGLE);
		delay(100);
		turnTo(0);
	}

}

int main()

{
                init();
	VideoCapture capture(CAM_PATH);
	//If this fails, try to open as a video camera, through the use of an integer param
	if (!capture.isOpened())
	{
		capture.open(atoi(CAM_PATH.c_str()));
	}


	//init初始化


	double dWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);			//the width of frames of the video
	double dHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);		//the height of frames of the video
	//采集摄像头
	cout << "摄像头 ";
	cout << dWidth + ' ' + dHeight;

	Mat image;
	while (true)
	{
		capture >> image;
		if (image.empty())
			break;

		//Set the ROI for the image
		//调整到合适的位置
		Rect roi(0, image.rows / 3, image.cols, image.rows / 3);
		Mat imgROI = image(roi);

		//Canny algorithm
		Mat contours;
		Canny(imgROI, contours, CANNY_LOWER_BOUND, CANNY_UPPER_BOUND);
#ifdef _DEBUG
		cv::imshow(CANNY_WINDOW_NAME, contours);
#endif

		vector<Vec2f> lines;
		HoughLines(contours, lines, 1, PI / 180, HOUGH_THRESHOLD);
		Mat result(imgROI.size(), CV_8U, Scalar(255));
		imgROI.copyTo(result);
		clog << lines.size() << endl;

		float maxRad = -2 * PI;
		float minRad = 2 * PI;
		//Draw the lines and judge the slope




			float theta1 = 0;
			float theta2 = 0;


			for (vector<Vec2f>::const_iterator it = lines.begin(); it != lines.end(); ++it)
			{
				float rho = (*it)[0];			//First element is distance rho
				float theta = (*it)[1];		//Second element is angle theta

				//Filter to remove vertical and horizontal lines,
				//and atan(0.09) equals about 5 degrees.


				//水平的线
				if(PI/2-PREFER_DELTA<theta&&theta<PI/2+PREFER_DELTA){
					continue;
				}
				else {
					if (rho > 0)
						theta1 = theta;
					else if (rho < 0)
						theta2 = theta;


					//画图
					Point pt1(rho / cos(theta), 0);
					Point pt2((rho - result.rows * sin(theta)) / cos(theta), result.rows);
					line(result, pt1, pt2, Scalar(0, 255, 255), 3, CV_AA);
				}
#ifdef _DEBUG
				clog << "Line: (" << rho << "," << theta << ")\n";
#endif
			}
			adjust(theta1, theta2);
#ifdef _DEBUG
		stringstream overlayedText;
		overlayedText << "Lines: " << lines.size();
		cv::putText(result, overlayedText.str(), Point(10, result.rows - 10), 2, 0.8, Scalar(0, 0, 255), 0);
		cv::imshow(MAIN_WINDOW_NAME, result);
#endif

		lines.clear();
		cv::waitKey(1);
	}
	return 0;
}
