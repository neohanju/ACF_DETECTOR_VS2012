#pragma once

typedef struct _stSettingParameters
{
	int nS1_INPUT_TYPE;
	int nS1_DETECTION_TYPE;	// 0:Head / 1:Full-body
	int nS1_DETECTOR_TYPE; // 0:no opnmp / 1:ACF /2:TEXT
	int nDET_TH;
	int bSHOW_WINDOW;  // 0:OFF  / 1: ON 
	char pDBPath[_MAX_PATH];
} stSettingParameters;

struct st_pedestrianDetectorParam
{
	int nSensitivity; //0~100
	int nCandidate;
	int imType; //0 RGB , 1 BGR, 2 YUV2
	stSettingParameters params;
};