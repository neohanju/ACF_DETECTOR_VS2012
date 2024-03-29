#pragma once

#ifdef	EXPORT_LIBRARY
#define EXPORT_LIBRARY_DECL	extern "C" __declspec(dllexport)
#else
#define EXPORT_LIBRARY_DECL extern "C" __declspec(dllimport)
#endif

#include "S1_PED_OUTST.h"


//	ERROR CODE	START
#define NON_OPERATION_ERROR		0

//메모리 포인터 에러
#define ENGINE_PT_NULL			-1	
#define PARAM_PT_NULL			-2
#define IMAGE_PT_NULL			-3
#define ALARM_PT_NULL			-4

//ROI설정 에러
#define ROI_ID_NEGATIVE			-101
#define ROI_PTS_NEGATIVE		-102
#define ROI_IDOVER_LIMIT		-103
#define ROI_ALREADY_SET			-104
#define ROI_IDNOT_EXIST			-105
//	ERROR CODE	END
EXPORT_LIBRARY_DECL int SetDefaultPedParam(void* pParam);

EXPORT_LIBRARY_DECL void* InitPedestrianDetect(int imwidth, int imheight, char* pDBPath, int imageType, void* pParam);

EXPORT_LIBRARY_DECL int SetDetectPedROI(void* pengine, int ROIID, int nPoints, int* pPositions);

EXPORT_LIBRARY_DECL int RemoveDetectPedROI(void* pengine, int ROIID);

EXPORT_LIBRARY_DECL int SetPedSensitivity(void* pengine, int nSensitivity);

EXPORT_LIBRARY_DECL int GetPedSensitivity(void* pengine, int* nSensitivity);

EXPORT_LIBRARY_DECL int ClearPedestrianDetect(void** ppengine);
//최경택 수정 pCandidateGet NULL 부분과 ppDetectionResult의 이중포인터 사용 2015-11-05
EXPORT_LIBRARY_DECL int PedestrianDetect(void* pengine, unsigned char* pImage, int nCandidate, int* pCandidateGate, int** pDetectionResult);
//최경택 수정 PedestrianDetect에서 반환한 메모리 해제하는 함수 2015-11-05
EXPORT_LIBRARY_DECL void PedestrianResultFree(int** pDetectionResult);



