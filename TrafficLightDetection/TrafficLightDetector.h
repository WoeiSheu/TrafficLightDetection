#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>
#include "Context.h"

using namespace cv;

#define MY_COLOR_PURPLE Scalar(255,0,255)
#define MY_COLOR_RED Scalar(0,0,255)
#define MY_COLOR_GREEN Scalar(0,255,0)
#define MY_COLOR_YELLOW Scalar(60,255,255)
#define MY_COLOR_BLUE Scalar(255,0,0)
#define MY_COLOR_WHITE Scalar(255,255,255)

static const LightState STATE_TRANSITION_MATRIX[5][8] = { 
	{ GREEN, RED, YELLOW, REDYELLOW, GREEN, GREEN, GREEN, UNDEFINED },
	{ YELLOW, RED, YELLOW, REDYELLOW, GREEN, GREEN, GREEN, UNDEFINED },
	{ RED, RED, REDYELLOW, REDYELLOW, GREEN, GREEN, GREEN, UNDEFINED },
	{ REDYELLOW, REDYELLOW, REDYELLOW, REDYELLOW, GREEN, GREEN, GREEN, UNDEFINED },
	{ UNDEFINED, RED, YELLOW, REDYELLOW, GREEN, GREEN, GREEN, UNDEFINED }
};

#define RED_DRAW_CENTER Point(465,465)
#define YELLOW_DRAW_CENTER Point(465,500)
#define GREEN_DRAW_CENTER Point(465,535)
#define LIGHT_DRAW_RADIUS 15

#define TL_COUNT 1

#define MIN_WIDTH 50
#define MIN_HEIGHT 50
#define MAX_WIDTH 300
#define MAX_HEIGHT 200

class TrafficLightDetector {
public:
public:
private:
	Mat showMask, redMask, blueMask, greenMask;
public:
	TrafficLightDetector();
	LightState brightnessDetect(const Mat &input);
	void colorDetect(const Mat &input, Mat &output, const Rect coords, int Hmin, int Hmax);
	vector<Context> contexts;

	double getBrightnessRatioInCircle(const Mat &input, const Point center, const int radius);
	int getCurrentLightsCode(bool display_red, bool display_yellow, bool display_green);
	LightState determineState(LightState previousState, int currentLightsCode);

	void setContexts(TrafficLightDetector &detector);
	void drawTrafficLights(Mat &targetImg, LightState lightState);
protected:
private:
};