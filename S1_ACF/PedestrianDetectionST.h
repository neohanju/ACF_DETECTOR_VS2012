#pragma once
//#include "Flame/AVSS2015.h"
#include "S1_ACF.h"

#define DETECTION_NUMBER_OF_ROI 128


struct st_fireDetectorParam;

struct st_roi {

	st_roi(int nPts);
	~st_roi();

	struct xy {
		int x, y;
	};
	int n;
	xy* pts;
};


struct st_pedestrianDetector {

	st_pedestrianDetector();
	~st_pedestrianDetector();

	CS1_ACF pedestrianDetector;

	st_pedestrianDetectorParam param;

	int imWidth, imHeight;
	//char pDBPath[1024];	
	int imType;
	//ROI / RON
	void updateDetectionROIMap();

	bool bDetectionROI[DETECTION_NUMBER_OF_ROI];

	st_roi* DetectionROI[DETECTION_NUMBER_OF_ROI];

	IplImage* pimage;
	IplImage* pimage2;
	IplImage* pDetectionROIMap;
};
