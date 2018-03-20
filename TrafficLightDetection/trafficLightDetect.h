#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <list>
#include <queue>
#include <cstdlib>
#include <iostream>

using namespace cv;

class TrafficLightDetect {
private :
	cv::Mat InputImg;
	cv::Mat img_resized;
	cv::Mat equaled_img;
	cv::Mat halfMaskImg;
	cv::Mat red;
	cv::Mat green;
	cv::Mat black;
	cv::Mat red2;
	cv::Mat green2;
	cv::Mat black2;
	cv::Mat redContours;
	cv::Mat greenContours;
	cv::Mat otsu;
	cv::Mat rectBack;

	cv::Size presetSize;
	cv::Mat square3x3;
	cv::Mat square5x5;
	cv::Mat square7x7;
	cv::Mat squareEle;
	cv::Mat cross3x3;
	cv::Mat cross5x5;
	cv::Mat xElement;
	cv::Mat diamond;
	float timeCost;
	int flag;
	std::list<Rect> trafficRedLightRect, trafficGreenLightRect;
	void preProcess() {
		presetSize = cv::Size(640, 480);
		square3x3 = cv::Mat(3, 3, CV_8U, cv::Scalar(1));

		square5x5 = cv::Mat(5, 5, CV_8U, cv::Scalar(1));
		square7x7 = cv::Mat(7, 7, CV_8U, cv::Scalar(1));
		squareEle = cv::Mat(11, 11, CV_8U, cv::Scalar(1));
		cross3x3 = cv::Mat(3, 3, CV_8U, cv::Scalar(0));
		diamond = cv::Mat(5, 5, CV_8U, cv::Scalar(1));
		diamond.at<uchar>(0, 0) = 0;
		diamond.at<uchar>(0, 1) = 0;
		diamond.at<uchar>(1, 0) = 0;
		diamond.at<uchar>(4, 4) = 0;
		diamond.at<uchar>(3, 4) = 0;
		diamond.at<uchar>(4, 3) = 0;
		diamond.at<uchar>(4, 0) = 0;
		diamond.at<uchar>(4, 1) = 0;
		diamond.at<uchar>(3, 0) = 0;
		diamond.at<uchar>(0, 4) = 0;
		diamond.at<uchar>(0, 3) = 0;
		diamond.at<uchar>(1, 4) = 0;
		cross3x3.at<uchar>(0, 1) = 1;
		cross3x3.at<uchar>(1, 0) = 1;
		cross3x3.at<uchar>(1, 1) = 1;
		cross3x3.at<uchar>(1, 2) = 1;
		cross3x3.at<uchar>(2, 1) = 1;
		cross5x5 = cv::Mat(5, 5, CV_8U, cv::Scalar(0));
		for (int i = 0; i < 5; i++)	{
			cross5x5.at<uchar>(2, i) = 1;
			cross5x5.at<uchar>(i, 2) = 1;
		}
		Mat img_masked;
		resize(InputImg, img_resized, presetSize, 0, 0, CV_INTER_LINEAR);


		vector<Mat> img_channels;
		//Mat result;
		split(img_resized, img_channels);
		equalizeHist(img_channels[0], img_channels[0]);
		equalizeHist(img_channels[1], img_channels[1]);
		equalizeHist(img_channels[2], img_channels[2]);
		merge(img_channels, equaled_img);
		//processImgForRed(equaled_img, result);

		red = Mat(img_resized.rows, img_resized.cols, CV_8UC1);
		green = Mat(img_resized.rows, img_resized.cols, CV_8UC1);
		black = Mat(img_resized.rows, img_resized.cols, CV_8UC1);
		normalizeImgRGBForColorDetect(equaled_img, red, green, black);
	}
	void normalizeImgRGBForColorDetect(cv::Mat img, cv::Mat& red, cv::Mat& green, cv::Mat& black) {
		Mat result(img.rows, img.cols, CV_32FC3);


		Mat_<Vec3b>::iterator it = img.begin<Vec3b>();
		Mat_<Vec3f>::iterator resultIt = result.begin<Vec3f>();
		Mat_<Vec3b>::iterator itend = img.end<Vec3b>();
		Mat_<Vec3f>::iterator resultItend = result.end<Vec3f>();
		Mat_<uchar>::iterator blackIt = black.begin<uchar>();
		Mat_<uchar>::iterator blackItend = black.end<uchar>();
		int sum = 0;
		for (; it != itend&&resultIt != resultItend; it++, resultIt++,blackIt++) {
			sum = (*it)[0] + (*it)[1] + (*it)[2];
			(*resultIt)[0] = (float)(*it)[0] / sum;
			(*resultIt)[1] = (float)(*it)[1] / sum;
			(*resultIt)[2] = (float)(*it)[2] / sum;
			if ((*it)[0] < 40 && (*it)[1] < 40 && (*it)[2] < 40) {
				(*blackIt) = 255;
			} else {
				(*blackIt) = 0;
			}
		}

		resultIt = result.begin<Vec3f>();
		it = img.begin<Vec3b>();
		Mat_<uchar>::iterator redIt = red.begin<uchar>();
		Mat_<uchar>::iterator redItend = red.end<uchar>();
		Mat_<uchar>::iterator greenIt = green.begin<uchar>();
		Mat_<uchar>::iterator greenItend = green.end<uchar>();

		for (; resultIt != resultItend&&redIt != redItend&&greenIt != greenItend; ++resultIt, ++redIt, ++greenIt) {
			if (((*resultIt)[2] - (*resultIt)[1]) > 0.16 && ((*resultIt)[1] - (*resultIt)[0]) < 0.08 && ((*resultIt)[1] < 0.30)) {
				(*redIt) = 255;
			} else {
				(*redIt) = 0;
			}
			if (((*resultIt)[2] - (*resultIt)[1]) < /*-0.16*/ -0.16 && ((*resultIt)[1] - (*resultIt)[0]) > /*0.18*/ 0.004 && ((*resultIt)[1] > 0.10)) {
				(*greenIt) = 255;
			} else {
				(*greenIt) = 0;
			}
			//if ((*it)[2] > 100 && (*resultIt)[2] > 0.4 && (*resultIt)[1]<0.3 && (*resultIt)[0] < 0.3)
			//	(*redIt) = 255;
			//else
			//	(*redIt) = 0;
			//if ((*it)[2]>100 && (*resultIt)[2]>0.4 && (*resultIt)[1] > 0.3 && (*resultIt)[0] < 0.3)
			//	(*yellowIt) = 255;
			//else
			//	(*yellowIt) = 0;
			//if (((*it)[0] + (*it)[1])>100 && (*resultIt)[2]<0.3 && (*resultIt)[1]>0.45 && (*resultIt)[0] > 0.45)
			//	(*greenIt) = 255;
			//else
			//	(*greenIt) = 0;
		}
	}
public:
	TrafficLightDetect(){
	}
	void run();
	void setflag(string str);
	Mat getResult(Mat img);
	void setInputImg(Mat input);
	void fliter(std::list<Rect>& rectlist,Mat img);
	void reset() {
		trafficRedLightRect.clear();
		trafficGreenLightRect.clear();
	}

	Mat getred(){ return red; }
	Mat getred2(){  return red2; }
	Mat getredContours(){  return redContours; }
	Mat getgreen(){  return green; }
	Mat getgreen2(){  return green2; }
	Mat getgreenContours(){ return greenContours; }
	Mat getblack(){  return black; }
	Mat getotsu(){ return otsu; }
	Mat getrectback(){  return rectBack; }
};