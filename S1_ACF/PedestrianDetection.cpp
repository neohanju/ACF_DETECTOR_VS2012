#define EXPORT_LIBRARY

#include "stdafx.h"

#include "PedestrianDetection.h"
#include "PedestrianDetectionST.h"

/*
st_fireDetectorParam::st_fireDetectorParam() {

	flameParam[0] = 1.0;	flameParam[1] = 0.6;
	smokeParam[0] = 0.0;	smokeParam[1] = 0.0;
	flameSensitivity = 60;	smokeSensitivity = 60;
	imType = 1;
}
*/


st_pedestrianDetector::st_pedestrianDetector() {
		
	imWidth = 640;
	imHeight = 480;

	param.nSensitivity = 50;
	param.params.bSHOW_WINDOW = 0;
	param.params.nDET_TH = 0; // detection threshold
	param.params.nS1_DETECTION_TYPE = 1; // 0:Head / 1:Full-body / 2: Full-body from Head
	param.params.nS1_DETECTOR_TYPE = 1; // 0:no openmp - acfDetect / 1:openmp - acfDetect /2:TEXT
	param.params.nS1_INPUT_TYPE = 1; // 0:ETRI %d_%04d.jpg / 1:PETS2009 View_%03d\\frame_%04d.jpg / 2:VIDEO / 3: %d_%04d.jpg / others : %d_%d.jpg
	param.imType = 0;

	memset(bDetectionROI,0,sizeof(char)*DETECTION_NUMBER_OF_ROI);

	pimage = 0;
	pimage2 = 0;
	pDetectionROIMap = 0;
}

st_pedestrianDetector::~st_pedestrianDetector() {

	for(int i=0;i<DETECTION_NUMBER_OF_ROI;++i) {
		if(bDetectionROI[i]==1) {
			//flameROI[i]->~st_roi();
			delete DetectionROI[i];
		}
	}
	if (pimage)
	{
		cvReleaseImage(&pimage);
		pimage = 0;
	}
	if (pimage2)
	{
		cvReleaseImage(&pimage2);
		pimage2 = 0;
	}
	if (pDetectionROIMap)
	{
		cvReleaseImage(&pDetectionROIMap);
		pDetectionROIMap = 0;
	}
	

}

void st_pedestrianDetector::updateDetectionROIMap() {

	CvPoint* ppt;	
	for(int i=0;i<DETECTION_NUMBER_OF_ROI;++i) 
	{		
		if(bDetectionROI[i]==1) 
		{
			ppt = (CvPoint*)malloc(sizeof(CvPoint)*DetectionROI[i]->n);
			for(int j=0;j<DetectionROI[i]->n;++j)
			{
				ppt[j].x = DetectionROI[i]->pts[j].x;
				ppt[j].y = DetectionROI[i]->pts[j].y;
			}		
			cvFillConvexPoly(pDetectionROIMap, ppt, DetectionROI[i]->n, cvScalar(255));		
			free(ppt);
		}			
	}
}


st_roi::st_roi(int nPts) {
	n = nPts;
	pts = (xy*)malloc(sizeof(xy)*n);
}

st_roi::~st_roi() {
	free(pts);
}


//////////////////////////////////////////////////////////////////////////
//
void* InitPedestrianDetect(int imwidth, int imheight, char* pDBPath, int imageType, void* pParam) {

	char filename[_MAX_PATH];
	FILE* fp;

	st_pedestrianDetector* pengine;

	if (imwidth <= 0 || imheight <= 0)
		return 0;

	pengine = new st_pedestrianDetector;

	pengine->imWidth = imwidth;
	pengine->imHeight = imheight;
	
	pengine->param.imType = imageType;
	if(pParam!=0) {
		memcpy(&(pengine->param), pParam, sizeof(st_pedestrianDetectorParam));
		memcpy(pengine->param.params.pDBPath, pDBPath, sizeof(char)*_MAX_PATH);
	}
	else
	{
		st_pedestrianDetectorParam stparam;
		SetDefaultPedParam(&stparam);
		memcpy(stparam.params.pDBPath, pDBPath, sizeof(char)*_MAX_PATH);
		memcpy(&(pengine->param), &stparam, sizeof(st_pedestrianDetectorParam));
	}

	//initialize the detectors
	pengine->pedestrianDetector.Initialize(pengine->param.params);
		

	pengine->pimage = cvCreateImage(cvSize(imwidth, imheight), IPL_DEPTH_8U, 3);
	memset(pengine->pimage->imageData, 0, pengine->pimage->widthStep*pengine->pimage->height);
	pengine->pimage2 = cvCreateImage(cvSize(imwidth, imheight), IPL_DEPTH_8U, 3);
	cvCopy(pengine->pimage, pengine->pimage2);

	pengine->pDetectionROIMap = cvCreateImage(cvSize(imwidth, imheight), IPL_DEPTH_8U, 1);
	memset(pengine->pDetectionROIMap->imageData, 0, pengine->pDetectionROIMap->widthStep*pengine->pDetectionROIMap->height);

	return pengine;
}



//////////////////////////////////////////////////////////////////////////
//
int SetDetectPedROI(void* pengine, int ROIID, int nPoints, int* pPositions) {

	//너무 큰 ROI ID
	if (!pengine)
		return ENGINE_PT_NULL;

	if (ROIID < 0)
		return ROI_ID_NEGATIVE;
	
	if (nPoints <= 0)
		return ROI_PTS_NEGATIVE;

	if(ROIID >= DETECTION_NUMBER_OF_ROI)
		return ROI_IDOVER_LIMIT;

	//이미 설정된 ROI
	if(((st_pedestrianDetector*)pengine)->bDetectionROI[ROIID] == 1)
		return ROI_ALREADY_SET;

	((st_pedestrianDetector*)pengine)->DetectionROI[ROIID] = new st_roi(nPoints);
	int* tt = pPositions;
	for(int i=0;i<nPoints;++i) {
		((st_pedestrianDetector*)pengine)->DetectionROI[ROIID]->pts[i].x = *(tt++);
		((st_pedestrianDetector*)pengine)->DetectionROI[ROIID]->pts[i].y = *(tt++);
	}
	((st_pedestrianDetector*)pengine)->bDetectionROI[ROIID] = 1;

	((st_pedestrianDetector*)pengine)->updateDetectionROIMap();

	return NON_OPERATION_ERROR;
}



//////////////////////////////////////////////////////////////////////////
//
int RemoveDetectPedROI(void* pengine, int ROIID) {

	if (!pengine)
		return ENGINE_PT_NULL;

	//너무 큰 ROI ID
	if(ROIID >= DETECTION_NUMBER_OF_ROI)
		return ROI_IDOVER_LIMIT;

	//설정 안 된 ROI
	if(((st_pedestrianDetector*)pengine)->bDetectionROI[ROIID] == 0)
		return ROI_IDNOT_EXIST;

	//((st_fireDetector*)pengine)->flameROI[ROIID]->~st_roi();
	delete ((st_pedestrianDetector*)pengine)->DetectionROI[ROIID];

 	((st_pedestrianDetector*)pengine)->bDetectionROI[ROIID] = 0;

	((st_pedestrianDetector*)pengine)->updateDetectionROIMap();

	return NON_OPERATION_ERROR;

}



//////////////////////////////////////////////////////////////////////////
//
int SetPedSensitivity(void* pengine, int nSensitivity) {

	if (!pengine)
		return ENGINE_PT_NULL;

	((st_pedestrianDetector*)pengine)->param.nSensitivity = nSensitivity;

	return NON_OPERATION_ERROR;

}



//////////////////////////////////////////////////////////////////////////
//
int SetPedSensitivity(void* pengine, int* nSensitivity) {

	if (!pengine)
		return ENGINE_PT_NULL;

	*nSensitivity = ((st_pedestrianDetector*)pengine)->param.nSensitivity;

	return NON_OPERATION_ERROR;

}



//////////////////////////////////////////////////////////////////////////
//
int ClearPedestrianDetect(void** ppengine) {

	//((st_fireDetector*)*ppengine)->~st_fireDetector();
	if (*ppengine)
	{
		((st_pedestrianDetector*)(*ppengine))->pedestrianDetector.Finalize();
		delete ((st_pedestrianDetector*)*ppengine);
		*ppengine = NULL;
		return NON_OPERATION_ERROR;
	}
	else
		return ENGINE_PT_NULL;
}



int SetDefaultPedParam(void* pVoid)
{
	st_pedestrianDetectorParam* pParam;
	int i,len,find;
	char strdetector[] = "data/detector/";
	pParam = (st_pedestrianDetectorParam*)pVoid;
	if (!pParam)
		return PARAM_PT_NULL;
	pParam->nSensitivity = 10; pParam->params.bSHOW_WINDOW = 0;
	pParam->imType = 1; pParam->params.nS1_DETECTION_TYPE = 1;
	pParam->params.nS1_DETECTOR_TYPE = 1;
	pParam->params.nS1_INPUT_TYPE = 1;
	::GetModuleFileName(NULL, pParam->params.pDBPath, _MAX_PATH);
	len = strlen(pParam->params.pDBPath);
	for (i = len; i >= 0; i--)
	{
		if (pParam->params.pDBPath[i] == '\\')
		{
			find = i;
			break;
		}		
	}
	pParam->params.pDBPath[find + 1] = 0;
	strcat_s(pParam->params.pDBPath, _MAX_PATH, strdetector);
	
	//strcpy_s(&pParam->params.pDBPath[find + 1], strlen(strdetector), strdetector);
	//printf("%s", pParam->params.pDBPath);

	
	
	//sprintf_s(pParam->params.pDBPath, "../data/detector/");
	return NON_OPERATION_ERROR;
}

//////////////////////////////////////////////////////////////////////////
//
//최경택 수정 pCandidateGet NULL 부분과 ppDetectionResult의 이중포인터 사용 2015-11-05
int PedestrianDetect(void* pengine, unsigned char* pImage, int nCandidate, int* pCandidateGate, int** ppDetectionResult) 
{
	double heightRatio;
	bool bFlame, bSmoke;
	double modelHeight = 100;
	double w_r, h_r;
	int ndetected = 0;
	*ppDetectionResult = 0;
	
	if (!pengine)
		return ENGINE_PT_NULL;

	if (!pImage)
		return IMAGE_PT_NULL;
	/*
	if (!ppDetectionResult)
		return ALARM_PT_NULL;
	*/

	
	memcpy(((st_pedestrianDetector*)pengine)->pimage->imageData, pImage, ((st_pedestrianDetector*)pengine)->imWidth*((st_pedestrianDetector*)pengine)->imHeight*3);

	vector<stDetection> vecDetectionResult, vecDetectionResult_temp;
	//Test
	if(((st_pedestrianDetector*)pengine)->param.nSensitivity > 0 & nCandidate == 0) {
		//memset(((st_pedestrianDetector*)pengine)->pimage->imageData, 0, ((st_pedestrianDetector*)pengine)->imWidth*((st_pedestrianDetector*)pengine)->imHeight * 3);			
		vecDetectionResult = ((st_pedestrianDetector*)pengine)->pedestrianDetector.Run(((st_pedestrianDetector*)pengine)->pimage, 0, 0, ((st_pedestrianDetector*)pengine)->param.params);
		for( int i = 0 ; i < (int) vecDetectionResult_temp.size(); i++) {
			if(vecDetectionResult_temp[i].score >((st_pedestrianDetector*)pengine)->param.nSensitivity) {
				vecDetectionResult.push_back(vecDetectionResult_temp[i]);
			}
		}
		int kk = 1;
		//최경택 수정 Memory Allocation하는 부분 주의
		*ppDetectionResult = (int*)malloc(sizeof(int)*(vecDetectionResult.size()*6 + 1));
		for( int i = 0 ; i < (int) vecDetectionResult.size(); i++) {
			ndetected++;
			(*ppDetectionResult)[kk++] = 1;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].score;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].box.x;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].box.y;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].box.x + vecDetectionResult[i].box.w;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].box.y + vecDetectionResult[i].box.h;
		}
		(*ppDetectionResult)[0] = ndetected;
	}
	else if(((st_pedestrianDetector*)pengine)->param.nSensitivity > 0 & nCandidate != 0) {
		for(int k = 0 ; k < 5*nCandidate; k = k + 5) {
			cv::Mat matimage = cvarrToMat(((st_pedestrianDetector*)pengine)->pimage);
			cv::Mat roi = matimage(cv::Rect(pCandidateGate[k],pCandidateGate[k+1],pCandidateGate[k+2]-pCandidateGate[k]-1,pCandidateGate[k+3]-pCandidateGate[k+1]-1));
			heightRatio = modelHeight/pCandidateGate[k+4];
			h_r = roi.rows;
			w_r = roi.cols;
			cv::resize(roi, roi, cv::Size(w_r*heightRatio,h_r*heightRatio));
			IplImage *proi = new IplImage(roi);
			vecDetectionResult_temp = ((st_pedestrianDetector*)pengine)->pedestrianDetector.Run(proi, 0, 0, ((st_pedestrianDetector*)pengine)->param.params);
			for( int i = 0 ; i < (int) vecDetectionResult_temp.size(); i++) {
				if(vecDetectionResult_temp[i].score >((st_pedestrianDetector*)pengine)->param.nSensitivity) {
					ndetected++;
					vecDetectionResult_temp[i].box.x = pCandidateGate[k] + vecDetectionResult_temp[i].box.x/heightRatio;
					vecDetectionResult_temp[i].box.y = pCandidateGate[k+1] + vecDetectionResult_temp[i].box.y/heightRatio;
					vecDetectionResult_temp[i].box.w = vecDetectionResult_temp[i].box.w/heightRatio;
					vecDetectionResult_temp[i].box.h = vecDetectionResult_temp[i].box.h/heightRatio;
					vecDetectionResult.push_back(vecDetectionResult_temp[i]);
				}
			}
			delete [] proi;
			vecDetectionResult_temp.clear();
		}
		int kk = 1;
		//최경택 수정 Memory Allocation하는 부분 주의
		*ppDetectionResult = (int*)malloc(sizeof(int)*(vecDetectionResult.size()*6 + 1));
		for( int i = 0 ; i < (int) vecDetectionResult.size(); i++) {
			(*ppDetectionResult)[kk++] = 1;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].score;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].box.x;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].box.y;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].box.x + vecDetectionResult[i].box.w;
			(*ppDetectionResult)[kk++] = vecDetectionResult[i].box.y + vecDetectionResult[i].box.h;
		}
	}

	if(((st_pedestrianDetector*)pengine)->param.params.bSHOW_WINDOW == 1) {
		cv::Mat matimage = cvarrToMat(((st_pedestrianDetector*)pengine)->pimage);
		((st_pedestrianDetector*)pengine)->pedestrianDetector.visualize(&matimage,0,&vecDetectionResult);
	}	

	return NON_OPERATION_ERROR;
}

//최경택 수정 PedestrianDetect에서 반환한 메모리 해제하는 함수 2015-11-05
void PedestrianResultFree(int** pDetectionResult)
{
	if (*pDetectionResult)
	{
		free(*pDetectionResult);
		*pDetectionResult = 0;
	}
}


