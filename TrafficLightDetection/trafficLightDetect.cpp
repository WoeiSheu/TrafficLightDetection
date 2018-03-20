#include "stdafx.h"
#include "trafficLightDetect.h"

using namespace cv;


void TrafficLightDetect::run() {
	preProcess();
	morphologyEx(red, red2, cv::MORPH_OPEN, square3x3);
	morphologyEx(green, green2, cv::MORPH_OPEN, square3x3);

	morphologyEx(red2, red2, cv::MORPH_CLOSE, diamond);
	morphologyEx(green2, green2, cv::MORPH_CLOSE, diamond);
	morphologyEx(black, black2, cv::MORPH_CLOSE, square5x5);
	morphologyEx(black2, black2, cv::MORPH_OPEN, square3x3);

	Mat tmpred, tmpgreen, tmpblack;
	float rectanglerity;
	float Area;
	float threshlod;
	red2.copyTo(tmpred);
	green2.copyTo(tmpgreen);
	black2.copyTo(tmpblack);
	std::list<Rect> RedlightList;
	Rect LightRect0;
	std::list<Rect> GreenlightList;
	std::list<Rect> blackList;
	std::list<Rect> rectList;
	std::vector<std::vector<cv::Point>> RedLightContours;
	std::vector<std::vector<cv::Point>> greenLightContours;
	std::vector<std::vector<cv::Point>> blackContours;
	findContours(tmpred, RedLightContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	// Eliminate too short or too long contours
	int cmin1 = 10;  // minimum contour length
	int cmax1 = 1000; // maximum contour length
	std::vector<std::vector<cv::Point>>::const_iterator Reditc = RedLightContours.begin();
	while (Reditc != RedLightContours.end()) {

		if (Reditc->size() < cmin1 || Reditc->size() > cmax1) {
			Reditc = RedLightContours.erase(Reditc);
		} else {
			LightRect0 = boundingRect(cv::Mat(*Reditc));
			RedlightList.push_back(LightRect0);
			++Reditc;
		}
	}

	img_resized.copyTo(redContours);
	drawContours(redContours, RedLightContours, -1, Scalar(0, 0, 255), 2);

	findContours(tmpgreen, greenLightContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	// Eliminate too short or too long contours
	int cmin2 = 10;  // minimum contour length
	int cmax2 = 1000; // maximum contour length
	std::vector<std::vector<cv::Point>>::const_iterator greenItc = greenLightContours.begin();
	while (greenItc != greenLightContours.end()) {

		if (greenItc->size() < cmin2 || greenItc->size() > cmax2) {
			greenItc = greenLightContours.erase(greenItc);
		} else {
			LightRect0 = boundingRect(cv::Mat(*greenItc));
			GreenlightList.push_back(LightRect0);
			++greenItc;
		}
	}

	img_resized.copyTo(greenContours);
	drawContours(greenContours, greenLightContours, -1, Scalar(0, 255, 0), 2);

	// Find traffic light outline
	Mat gray_img;
	Mat img_thresold;
	Mat img_binary;
	cvtColor(equaled_img, gray_img, CV_BGR2GRAY);
	threshold(gray_img, img_thresold, 0, 255, cv::THRESH_OTSU);
	img_thresold.copyTo(otsu);
	//morphologyEx(img_thresold, img_binary, cv::MORPH_OPEN, square5x5);

	morphologyEx(img_thresold, img_binary, cv::MORPH_CLOSE, squareEle);
	morphologyEx(img_thresold, img_thresold, cv::MORPH_OPEN, square5x5);
	//morphologyEx(img_binary, img_binary, cv::MORPH_OPEN, square3x3);
	Mat tmpPic1;
	img_binary.copyTo(tmpPic1);

	// Get the contours of the connected components
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(tmpPic1,
		contours, // a vector of contours 
		CV_RETR_LIST, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours


	// Eliminate too short or too long contours
	int cmin = 60;  // minimum contour length
	int cmax = 2000; // maximum contour length
	std::vector<std::vector<cv::Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end()) {

		if (itc->size() < cmin || itc->size() > cmax) {
			itc = contours.erase(itc);
		} else {
			LightRect0 = boundingRect((*itc));
			Area = contourArea((*itc));
			rectanglerity = (float)(Area / LightRect0.area());
			threshlod = 0.75;

			if (rectanglerity > threshlod&&LightRect0.height>LightRect0.width) {
				++itc;
			} else {
				itc = contours.erase(itc);
			}
		}
	}




	Rect r0;
	float propotion;

	itc = contours.begin();
	while (itc != contours.end()) {
		r0 = cv::boundingRect(cv::Mat(*itc));
		Area = contourArea((*itc));
		propotion = (float)r0.height / r0.width;
		rectanglerity = (float)(Area / r0.area());
		//if (abs(propotion - 13.0f / 6.0f) > 0.8)
		//{
		//	itc = contours.erase(itc);
		//	continue;
		//}   
		if (rectanglerity < 0.8) {
			itc = contours.erase(itc); 
			continue;
		} else {
			rectList.push_back(r0);
			itc++;
		}

	}



	// Combine light color and outline

	std::list<Rect>::iterator RectIt;
	std::list<Rect>::iterator RedRectIt;
	std::list<Rect>::iterator GreenRectIt;
	Rect backGroundRect, redLightRect, greenLightRect, minRect, andRect;
	float distanceThreshold = 15.0f;
	float a, b, c, d;
	float tmpNum;
	int areaTh;
	for (RectIt = rectList.begin(); RectIt != rectList.end(); RectIt++)	{
		backGroundRect = (*RectIt);
		a = backGroundRect.x + backGroundRect.width / 2;
		c = backGroundRect.y;
		for (RedRectIt = RedlightList.begin(); RedRectIt != RedlightList.end(); RedRectIt++) {
			redLightRect = (*RedRectIt);
			b = redLightRect.x + redLightRect.width / 2;
			d = redLightRect.y;
			andRect = redLightRect&backGroundRect;
			minRect = redLightRect | backGroundRect;
			
			areaTh = 300;
			if (andRect.area() > areaTh || andRect == redLightRect)	{
				if (abs(a - b) < distanceThreshold&&abs(c - d)<10) {
					trafficRedLightRect.push_back(minRect);
					break;
				}
			} else if (flag !=3) {

				tmpNum = (float)minRect.height / minRect.width;
				if (minRect.area() < 2500 && tmpNum > 2 && tmpNum < 5)	{
					trafficRedLightRect.push_back(minRect);
					break;
				}
			}
		}
		for (GreenRectIt = GreenlightList.begin(); GreenRectIt != GreenlightList.end(); GreenRectIt++) {
			greenLightRect = *GreenRectIt;
			b = greenLightRect.x + greenLightRect.width / 2;
			d = greenLightRect.y + greenLightRect.height;
			c = backGroundRect.y + backGroundRect.height;
			andRect = greenLightRect&backGroundRect;
			minRect = greenLightRect | backGroundRect;
			if (andRect.area() > areaTh || andRect == greenLightRect) {
				if (abs(a - b) < distanceThreshold&&d > c) {
					trafficGreenLightRect.push_back(minRect);
					break;
				}
			} else {

				tmpNum = (float)minRect.height / minRect.width;
				if (minRect.area() < 2500 && tmpNum > 2 && tmpNum < 5) {
					trafficGreenLightRect.push_back(minRect);
					break;
				}
			}
		}
	}

	fliter(trafficRedLightRect, img_resized);
	fliter(trafficGreenLightRect, img_resized);

	int mixAreaTH = 2500;
	if (flag == 2) {
		mixAreaTH = 400;
	}
	if (trafficRedLightRect.size()==0&&trafficGreenLightRect.size()==0&&flag!=4) {
		//std::cout << "not found" << std::endl;
		findContours(tmpblack, blackContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		// Eliminate too short or too long contours
		int cmin3 = 30;  // minimum contour length
		int cmax3 = 500; // maximum contour length
		std::vector<std::vector<cv::Point>>::const_iterator blackItc = blackContours.begin();
		while (blackItc != blackContours.end()) {

			if (blackItc->size() < cmin3 || blackItc->size() > cmax3) {
				blackItc = blackContours.erase(blackItc);
			} else {
				LightRect0 = boundingRect(cv::Mat(*blackItc));
				Area = contourArea((*blackItc));
				rectanglerity = (float)(Area / LightRect0.area());
				if (LightRect0.area() > 1000) {
					threshlod = 0.9;
				} else {
					threshlod = 0.6;
				}
				if (rectanglerity > threshlod) {
					blackList.push_back(LightRect0);
					rectList.push_back(LightRect0);
					++blackItc;
				} else {
					blackItc = blackContours.erase(blackItc);
				}
			}
		}

		for (RectIt = rectList.begin(); RectIt != rectList.end(); RectIt++) {
			backGroundRect = (*RectIt);
			a = backGroundRect.x + backGroundRect.width / 2;
			c = backGroundRect.y + backGroundRect.height / 2;
			for (RedRectIt = RedlightList.begin(); RedRectIt != RedlightList.end(); RedRectIt++) {
				redLightRect = (*RedRectIt);
				b = redLightRect.x + redLightRect.width / 2;
				d = redLightRect.y + redLightRect.height / 2;
				andRect = redLightRect&backGroundRect;
				if (andRect.area() > 300) {
					if (abs(a - b) < distanceThreshold)	{
						trafficRedLightRect.push_back(backGroundRect);
						break;
					}
				} else {
					minRect = redLightRect | backGroundRect;
					tmpNum = (float)minRect.height / minRect.width;

					if (minRect.area() < mixAreaTH && tmpNum > 2 && tmpNum < 5)	{
						trafficRedLightRect.push_back(minRect);
						break;
					}
				}
			}
			for (GreenRectIt = GreenlightList.begin(); GreenRectIt != GreenlightList.end(); GreenRectIt++) {
				greenLightRect = *GreenRectIt;
				b = greenLightRect.x + greenLightRect.width / 2;
				d = greenLightRect.y + greenLightRect.height / 2;
				andRect = greenLightRect&backGroundRect;
				if (andRect.area() > 300) {
					if (abs(a - b) < distanceThreshold)	{
						trafficGreenLightRect.push_back(backGroundRect);
						break;
					}
				} else {
					minRect = greenLightRect | backGroundRect;
					tmpNum = (float)minRect.height / minRect.width;
					if (minRect.area() < mixAreaTH && tmpNum > 2 && tmpNum < 5)	{
						trafficGreenLightRect.push_back(minRect);
						break;
					}
				}
			}
		}
	}


	RectIt = rectList.begin();
	img_resized.copyTo(rectBack);
	while (RectIt != rectList.end()) {
		rectangle(rectBack, (*RectIt), Scalar(0), 3);
		RectIt++;
	}

	fliter(trafficRedLightRect, img_resized);
	fliter(trafficGreenLightRect, img_resized);


	// Draw Rect

}

Mat TrafficLightDetect::getResult(Mat img)
{
	Mat img_result;
	img.copyTo(img_result);
	Rect tmpRect;

	for (int i = trafficRedLightRect.size(); i > 0; i--) {
		tmpRect = trafficRedLightRect.front();
		trafficRedLightRect.pop_front();
		//rectangle(img_result, tmpRect, Scalar(0, 0, 255), 3);
		Point center(tmpRect.x + tmpRect.width/2, tmpRect.y);
		circle(img_result, center, tmpRect.width, Scalar(0, 0, 255), 3);
	}

	for (int i = trafficGreenLightRect.size(); i > 0; i--) {
		tmpRect = trafficGreenLightRect.front();
		trafficGreenLightRect.pop_front();
		//rectangle(img_result, tmpRect, Scalar(0, 255, 0), 3);
		Point center(tmpRect.x + tmpRect.width/2, tmpRect.y + tmpRect.height);
		circle(img_result, center, tmpRect.width, Scalar(0, 255, 0), 3);
	}
	return img_result;
}

void TrafficLightDetect::setInputImg(Mat input) {
	InputImg = input;
}

void TrafficLightDetect::fliter(std::list<Rect>& rectlist,Mat img)
{
	std::list<Rect>::iterator it = rectlist.begin();
	while (it!=rectlist.end()) {
		if (((*it).y > img.rows / 2) || ((*it).x + (*it).width > (img.cols - 200))) {
			it = rectlist.erase(it);
		} else {
			it++;
		}
	}
}

void TrafficLightDetect::setflag(string str) {
	flag = 1;
}