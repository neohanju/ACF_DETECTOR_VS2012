// S1_ACF_TEST.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "PedestrianDetection.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat inputFrame;
	cv::Mat inputFrame_resized;

	char inputFilePath[300];
	IplImage *ipl_inputFrame = cvLoadImage("../data/test_tunel.jpg", 1);
	inputFrame = cv::imread("../data/test_tunel.jpg", cv::IMREAD_COLOR);
	st_pedestrianDetectorParam temp;
	SetDefaultPedParam(&temp);

	
	//cv::resize(inputFrame[curCamID], inputFrame_resized[curCamID],cv::Size(640, 480));
	//cv::Size imsize = inputFrame_resized[curCamID].size();

	cv::Size imsize = inputFrame.size();
	int imwidth = imsize.width;
	int imheight = imsize.height;

	void* pPedestrianDetector;

	pPedestrianDetector = InitPedestrianDetect(imwidth, imheight, "../data/detector/", 1, &temp);
 	int pCandidateGate[10] = {NULL};
	int* pDetectionResult = 0;
	// ROI/ RON
	int pts[8] = { 0, 0, imwidth - 1, 0, imwidth - 1, imheight - 1, 0, imheight - 1 };
	SetDetectPedROI(pPedestrianDetector, 0, 4, pts);

	
	pCandidateGate[0] = 1317; pCandidateGate[1] = 224; pCandidateGate[2] = 1917; pCandidateGate[3] = 600; pCandidateGate[4] = 50;
 	pCandidateGate[5] = 515; pCandidateGate[6] = 392; pCandidateGate[7] = 1920; pCandidateGate[8] = 1080; pCandidateGate[9] = 400;	
	//PedestrianDetect(pPedestrianDetector, inputFrame[curCamID].data, 2, pCandidateGate, &pDetectionResult);
	PedestrianDetect(pPedestrianDetector, (unsigned char *) ipl_inputFrame->imageData, 0, 0, &pDetectionResult);

	PedestrianResultFree(&pDetectionResult);

	//free(pDetectionResult);
	ClearPedestrianDetect(&pPedestrianDetector);
	return 0;
}

