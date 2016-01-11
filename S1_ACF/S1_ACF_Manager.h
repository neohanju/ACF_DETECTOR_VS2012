#pragma once

#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgproc/imgproc_c.h>
#include "cameraModel.h"
#include "mxArray.h"

#include <cmath>
#include <typeinfo>

using namespace std;
using namespace cv;

#define NUM_CAM (1)
const unsigned int CAM_ID[NUM_CAM] = {0};
#define CALIBRATION_PATH ("../data/calibrationInfos/")
#define DETECTOR_PATH ("../data/detector/")

class S1_Point2D
{
public:

	// data
	double x;
	double y;

	// constructors
	S1_Point2D() : x(0), y(0) {}
	S1_Point2D(double x, double y) : x(x), y(y){}
	S1_Point2D(cv::Point2f a) : x((double)a.x), y((double)a.y){}

	// operators
	S1_Point2D& operator=(const S1_Point2D &a){ x = a.x; y = a.y; return *this; }
	S1_Point2D& operator=(const cv::Point &a){ x = a.x; y = a.y; return *this; }
	S1_Point2D& operator=(const cv::Point2f &a){ x = (double)a.x; y = (double)a.y; return *this; }
	S1_Point2D& operator+=(const S1_Point2D &a){ x = x + a.x; y = y + a.y; return *this; }
	S1_Point2D& operator-=(const S1_Point2D &a){ x = x - a.x; y = y - a.y; return *this; }
	S1_Point2D& operator+=(const double s){ x = x + s; y = y + s; return *this; }
	S1_Point2D& operator-=(const double s){ x = x - s; y = y - s; return *this; }
	S1_Point2D& operator*=(const double s){ x = x * s; y = y * s; return *this; }
	S1_Point2D& operator/=(const double s){ x = x / s; y = y / s; return *this; }
	S1_Point2D operator+(const S1_Point2D &a){ return S1_Point2D(x + a.x, y + a.y); }
	S1_Point2D operator+(const double s){ return S1_Point2D(x + s, y + s); }
	S1_Point2D operator-(const S1_Point2D &a){ return S1_Point2D(x - a.x, y - a.y); }
	S1_Point2D operator-(const double s){ return S1_Point2D(x - s, y - s); }
	S1_Point2D operator-(){ return S1_Point2D(-x, -y); }
	S1_Point2D operator*(const double s){ return S1_Point2D(x * s, y * s); }
	S1_Point2D operator/(const double s){ return S1_Point2D(x / s, y / s); }
	bool operator==(const S1_Point2D &a){ return (x == a.x && y == a.y); }
	bool operator==(const cv::Point &a){ return (x == a.x && y == a.y); }

	// methods
	double norm_L2(){ return std::sqrt(x * x + y * y); }
	double dot(const S1_Point2D &a){ return x * a.x + y * a.y; }
	bool onView(const unsigned int width, const unsigned int height)
	{ 
		if(x < 0){ return false; }
		if(x >= (double)width){ return false; }
		if(y < 0){ return false; }
		if(y >= (double)height){ return false; }
		return true; 
	}
	S1_Point2D scale(double scale)
	{
		return S1_Point2D(x * scale, y * scale);
	}

	// data conversion
	cv::Point cv(){ return cv::Point((int)x, (int)y); }
};

class S1_Point3D
{
public:

	// data
	double x;
	double y;
	double z;

	// constructors
	S1_Point3D() : x(0), y(0), z(0) {}
	S1_Point3D(double x, double y, double z) :x(x), y(y), z(z)	{}

	// operators
	S1_Point3D& operator=(const S1_Point3D &a){ x = a.x; y = a.y; z = a.z; return *this; }
	S1_Point3D& operator=(const cv::Point3d &a){ x = a.x; y = a.y; z = a.z; return *this; }
	S1_Point3D& operator+=(const S1_Point3D &a){ x = x + a.x; y = y + a.y; z = z + a.z; return *this; }
	S1_Point3D& operator-=(const S1_Point3D &a){ x = x - a.x; y = y - a.y; z = z - a.z; return *this; }
	S1_Point3D& operator+=(const double s){ x = x + s; y = y + s; z = z + s; return *this; }
	S1_Point3D& operator-=(const double s){ x = x - s; y = y - s; z = z - s; return *this; }
	S1_Point3D& operator*=(const double s){ x = x * s; y = y * s; z = z * s; return *this; }
	S1_Point3D& operator/=(const double s){ x = x / s; y = y / s; z = z / s; return *this; }
	S1_Point3D operator+(const S1_Point3D &a){ return S1_Point3D(x + a.x, y + a.y, z + a.z); }
	S1_Point3D operator+(const double s){ return S1_Point3D(x + s, y + s, z + s); }
	S1_Point3D operator-(const S1_Point3D &a){ return S1_Point3D(x - a.x, y - a.y, z - a.z); }
	S1_Point3D operator-(const double s){ return S1_Point3D(x - s, y - s, z - s); }
	S1_Point3D operator-(){ return S1_Point3D(-x, -y, -z); }
	S1_Point3D operator*(const double s){ return S1_Point3D(x * s, y * s, z * s); }
	S1_Point3D operator/(const double s){ return S1_Point3D(x / s, y / s, z / s); }

	bool operator==(const S1_Point3D &a){ return (x == a.x && y == a.y && z == a.z); }
	bool operator==(const cv::Point3d &a){ return (x == a.x && y == a.y && z == a.z); }

	// methods
	double norm_L2(){ return std::sqrt(x * x + y * y + z * z); }
	double dot(const S1_Point3D &a){ return x * a.x + y * a.y + z * a.z; }

	// data conversion
	cv::Point3d cv(){ return cv::Point3d(x, y, z); }
};

class S1_Rect
{
public:
	double x;
	double y;
	double w;
	double h;

	// constructors
	S1_Rect() : x(0), y(0), w(0), h(0) {}
	S1_Rect(double x, double y, double w, double h) :x(x), y(y), w(w), h(h) {}

	// operators
	S1_Rect& operator=(const S1_Rect &a){ x = a.x; y = a.y; w = a.w; h = a.h; return *this; }
	S1_Rect& operator=(const cv::Rect &a){ x = a.x; y = a.y; w = a.width; h = a.height; return *this; }
	bool operator==(const S1_Rect &a){ return (x == a.x && y == a.y && w == a.w && h == a.h); }
	bool operator==(const cv::Rect &a){ return (x == a.x && y == a.y && w == a.width && h == a.height); }

	// methods	
	S1_Point2D bottomCenter(){ return S1_Point2D(x + std::ceil(w/2.0), y + h); }
	S1_Point2D center(){ return S1_Point2D(x + std::ceil(w/2.0), y + std::ceil(h/2.0)); }
	S1_Point2D reconstructionPoint()
	{
		return this->bottomCenter();
		//switch(S1_INPUT_TYPE)
		//{
		//case 1:
		//	return this->bottomCenter();
		//	break;
		//default:
		//	return this->center();
		//	break;
		//}
	}
	S1_Rect cropWithSize(const double width, const double height)
	{
		double newX = max(0.0, x);
		double newY = max(0.0, y);
		double newW = min(width - newX - 1, w);
		double newH = min(height - newY - 1, h);
		return S1_Rect(newX, newY, newW, newH);
	}
	S1_Rect scale(double scale)
	{
		return S1_Rect(x * scale, y * scale, w * scale, h * scale);
	}
	double area(){ return w * h; }
	bool contain(const S1_Point2D &a){ return (a.x >= x && a.x < x + w && a.y >= y && a.y < y + h); }
	bool contain(const cv::Point2f &a){ return ((double)a.x >= x && (double)a.x < x + w && (double)a.y >= y && (double)a.y < y + h); }
	bool overlap(const S1_Rect &a)
	{ 
		return (max(x + w, a.x + a.w) - min(x, a.x) < w + a.w) && (max(y + h, a.y + a.h) - min(y, a.y) < h + a.h) ? true : false; 
	}
	double distance(const S1_Rect &a)
	{ 
		S1_Point3D descriptor1 = S1_Point3D(x + w/2.0, y + h/2.0, w);
		S1_Point3D descriptor2 = S1_Point3D(a.x + a.w/2.0, a.y + a.h/2.0, a.w);
		return (descriptor1 - descriptor2).norm_L2() / min(w, a.w);
	}

	// conversion
	cv::Rect cv(){ return cv::Rect((int)x, (int)y, (int)w, (int)h); }
};

typedef struct _stACFParameters
{
	mxArray *mxArray_opts;
	mxArray	*mxArray_clf;
	mxArray	*mxArray_info1;
	mxArray	*mxArray_info2;
	mxArray	*mxArray_info3;

	mxArray *pr_pPyramid;
	mxArray *pr_pChns;
	mxArray *pr_shrink;
	mxArray *pr_pColor;
	mxArray *pr_colorSpace;
	mxArray *pr_nPerOct;
	mxArray *pr_nOctUp;
	mxArray *pr_minDs;
	mxArray *pr_nApprox;
	mxArray *pr_smooth;
	mxArray *pr_smooth_color;
	mxArray *pr_enabled;
	mxArray *pr_lambdas;
	mxArray *pr_pad;
	mxArray *pr_padWith0,*pr_padWith1,*pr_padWith2;
} stACFParameters;

typedef struct _stDetection
{
	S1_Rect box;
	float score;
	std::vector<S1_Rect> vecPartBoxes;
} stDetection;

typedef struct _stCalibrationInfos
{
	unsigned int nCamIdx;
	Etiseo::CameraModel cCamModel;
	cv::Mat matProjectionSensitivity;
	cv::Mat matDistanceFromBoundary;
} stCalibrationInfo;


