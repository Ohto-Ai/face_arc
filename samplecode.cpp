#include <windows.h>
#include <opencv2/opencv.hpp>

#include "ArcSoftInterface.hpp"


using namespace std;

#define APPID "7cKbNGbascmsWqh42sPcV62DfmWbM4cq8CD6ygcKJMmX"
#ifdef _WIN64
#define SDKKEY "7TdPs5daz29ZnSCmoUFZPsS8HzsdnXijbmDxNbVdR6X9"
#else defined _WIN32
#define SDKKEY "7TdPs5daz29ZnSCmoUFZPsS8EkeS2KfreXwjVoBf54h5"
#endif

//裁剪图片
void CutIplImage(IplImage* src, IplImage* dst, int x, int y)
{
	CvSize size = cvSize(dst->width, dst->height);//区域大小
	cvSetImageROI(src, cvRect(x, y, size.width, size.height));//设置源图像ROI
	cvCopy(src, dst); //复制图像
	cvResetImageROI(src);//源图像用完后，清空ROI
}

//时间戳转换为日期格式
void timestampToTime(char* timeStamp, char* dateTime, int dateTimeSize)
{
	time_t tTimeStamp = atoll(timeStamp);
	struct tm* pTm = gmtime(&tTimeStamp);
	strftime(dateTime, dateTimeSize, "%Y-%m-%d %H:%M:%S", pTm);
}

int main()
{
	MRESULT res = MOK;
	ASF_ActiveFileInfo activeFileInfo = { 0 };
	res = ASFGetActiveFileInfo(&activeFileInfo);
	if (res != MOK)
	{
		printf("ASFGetActiveFileInfo fail: %d\n", res);
	}
	else
	{
		char startDateTime[32];
		timestampToTime(activeFileInfo.startTime, startDateTime, 32);
		printf("startTime: %s\n", startDateTime);
		char endDateTime[32];
		timestampToTime(activeFileInfo.endTime, endDateTime, 32);
		printf("endTime: %s\n", endDateTime);
	}

	const ASF_VERSION version = ASFGetVersion();
	printf("\nVersion:%s\n", version.Version);
	printf("BuildDate:%s\n", version.BuildDate);
	printf("CopyRight:%s\n", version.CopyRight);

	printf("\n************* Face Recognition *****************\n");

	//激活接口,首次激活需联网
	if (ohtoai::arc::ASFFaceEngine::onlineActivation(APPID, SDKKEY))
		printf("ASFActivation sucess\n");
	else
		printf("ASFActivation fail\n");

	//初始化接口
	ohtoai::arc::ASFFaceEngine engine;
	MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | 
		ASF_LIVENESS | ASF_IR_LIVENESS;
	
	if (engine.initEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, 30, 5, mask))
		printf("ASFInitEngine sucess\n");
	else
		printf("ASFInitEngine fail\n");

	// 人脸检测
	IplImage* img1 = cvLoadImage("RGB图像路径");
	IplImage* img2 = cvLoadImage("RGB图像路径");
	IplImage* img3 = cvLoadImage("IR图像路径");

	if (img1 && img2 && img3)
	{
		ASF_MultiFaceInfo detectedFaces1 = { 0 };
		ASF_SingleFaceInfo SingleDetectedFaces1 = { 0 };
		ohtoai::arc::FaceFeature feature{};
		
		IplImage* cutImg1 = cvCreateImage(cvSize(img1->width - img1->width % 4, img1->height), IPL_DEPTH_8U, img1->nChannels);
		CutIplImage(img1, cutImg1, 0, 0);

		ASVLOFFSCREEN offscreen1 = { 0 };
		offscreen1.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
		offscreen1.i32Width = cutImg1->width;
		offscreen1.i32Height = cutImg1->height;
		offscreen1.pi32Pitch[0] = cutImg1->widthStep;
		offscreen1.ppu8Plane[0] = (MUInt8*)cutImg1->imageData;

		res = ASFDetectFacesEx(engine.engineHandle(), &offscreen1, &detectedFaces1);
		if (MOK == res && detectedFaces1.faceNum > 1)
		{
			SingleDetectedFaces1.faceRect.left = detectedFaces1.faceRect[0].left;
			SingleDetectedFaces1.faceRect.top = detectedFaces1.faceRect[0].top;
			SingleDetectedFaces1.faceRect.right = detectedFaces1.faceRect[0].right;
			SingleDetectedFaces1.faceRect.bottom = detectedFaces1.faceRect[0].bottom;
			SingleDetectedFaces1.faceOrient = detectedFaces1.faceOrient[0];

			feature = engine.faceFeatureExtractEx(&offscreen1, &SingleDetectedFaces1).value();
			if (!feature.empty())
			{
				printf("ASFFaceFeatureExtract 1 success\n");
			}
			else
				printf("ASFFaceFeatureExtract 1 fail\n");
		}
		else
			printf("ASFDetectFaces 1 fail: %d\n", res);


		//第二张人脸提取特征
		ASF_MultiFaceInfo	detectedFaces2 = { 0 };
		ASF_SingleFaceInfo SingleDetectedFaces2 = { 0 };
		ohtoai::arc::FaceFeature feature2{};
		IplImage* cutImg2 = cvCreateImage(cvSize(img2->width - img2->width % 4, img2->height), IPL_DEPTH_8U, img2->nChannels);
		CutIplImage(img2, cutImg2, 0, 0);

		ASVLOFFSCREEN offscreen2 = { 0 };
		offscreen2.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
		offscreen2.i32Width = cutImg2->width;
		offscreen2.i32Height = cutImg2->height;
		offscreen2.pi32Pitch[0] = cutImg2->widthStep;
		offscreen2.ppu8Plane[0] = (MUInt8*)cutImg2->imageData;

		res = ASFDetectFacesEx(engine.engineHandle(), &offscreen2, &detectedFaces2);
		if (MOK == res && detectedFaces2.faceNum > 1)
		{
			SingleDetectedFaces2.faceRect.left = detectedFaces2.faceRect[0].left;
			SingleDetectedFaces2.faceRect.top = detectedFaces2.faceRect[0].top;
			SingleDetectedFaces2.faceRect.right = detectedFaces2.faceRect[0].right;
			SingleDetectedFaces2.faceRect.bottom = detectedFaces2.faceRect[0].bottom;
			SingleDetectedFaces2.faceOrient = detectedFaces2.faceOrient[0];

			feature2 = engine.faceFeatureExtractEx(&offscreen2, &SingleDetectedFaces2).value();
			if (feature2.empty())
				printf("ASFFaceFeatureExtractEx 2 fail\n");
		}
		else
			printf("ASFDetectFacesEx 2 fail: %d\n", res);


		// 单人脸特征比对
		MFloat confidenceLevel;
		confidenceLevel = engine.faceFeatureCompare(feature, feature2).value();
		printf("ASFFaceFeatureCompare sucess: %lf\n", confidenceLevel);

		//设置活体置信度 SDK内部默认值为 IR：0.7  RGB：0.5（无特殊需要，可以不设置）
		ASF_LivenessThreshold threshold = { 0 };
		threshold.thresholdmodel_BGR = 0.5;
		threshold.thresholdmodel_IR = 0.7;
		res = ASFSetLivenessParam(engine.engineHandle(), &threshold);
		if (res != MOK)
			printf("ASFSetLivenessParam fail: %d\n", res);
		else
			printf("RGB Threshold: %f  IR Threshold: %f\n", threshold.thresholdmodel_BGR, threshold.thresholdmodel_IR);

		printf("\n*************** RGB Process ***************\n");

		// RGB图像检测
		MInt32 processMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
		res = ASFProcessEx(engine.engineHandle(), &offscreen2, &detectedFaces2, processMask);
		if (res != MOK)
			printf("ASFSProcessEx fail: %d\n", res);
		else
			printf("ASFProcessEx sucess: %d\n", res);

		// 获取年龄
		ASF_AgeInfo ageInfo = { 0 };
		res = ASFGetAge(engine.engineHandle(), &ageInfo);
		if (res != MOK)
			printf("ASFGetAge fail: %d\n", res);
		else
			printf("Age: %d\n", ageInfo.ageArray[0]);

		// 获取性别
		ASF_GenderInfo genderInfo = { 0 };
		res = ASFGetGender(engine.engineHandle(), &genderInfo);
		if (res != MOK)
			printf("ASFGetGender fail: %d\n", res);
		else
			printf("Gender: %d\n", genderInfo.genderArray[0]);

		// 获取3D角度
		ASF_Face3DAngle angleInfo = { 0 };
		res = ASFGetFace3DAngle(engine.engineHandle(), &angleInfo);
		if (res != MOK)
			printf("ASFGetFace3DAngle fail: %d\n", res);
		else
			printf("3DAngle roll: %f   yaw: %f   pitch: %f\n", angleInfo.roll[0], angleInfo.yaw[0], angleInfo.pitch[0]);

		//获取RGB活体信息
		ASF_LivenessInfo rgbLivenessInfo = { 0 };
		res = ASFGetLivenessScore(engine.engineHandle(), &rgbLivenessInfo);
		if (res != MOK)
			printf("ASFGetLivenessScore fail: %d\n", res);
		else
			printf("RGB Liveness: %d\n", rgbLivenessInfo.isLive[0]);


		printf("\n********** IR Process *************\n");

		ASF_MultiFaceInfo	detectedFaces3 = { 0 };
		IplImage* cutImg3 = cvCreateImage(cvSize(img3->width - img3->width % 4, img3->height), IPL_DEPTH_8U, img3->nChannels);
		CutIplImage(img3, cutImg3, 0, 0);

		//opencv读图时会将灰度图读成RGB图，需要转换成GRAY图进行IR活体检测
		cv::Mat grayMat;
		cv::Mat copyCutImg3(cutImg3, false);	//IplImage转Mat 设为ture为深拷贝
		cv::cvtColor(copyCutImg3, grayMat, CV_BGR2GRAY);

		ASVLOFFSCREEN offscreen3 = { 0 };
		offscreen3.u32PixelArrayFormat = ASVL_PAF_GRAY;
		offscreen3.i32Width = grayMat.cols;
		offscreen3.i32Height = grayMat.rows;
		offscreen3.pi32Pitch[0] = grayMat.step;
		offscreen3.ppu8Plane[0] = grayMat.data;

		res = ASFDetectFacesEx(engine.engineHandle(), &offscreen3, &detectedFaces3);
		if (res != MOK && detectedFaces3.faceNum > 1)
			printf("ASFDetectFacesEx fail: %d\n", res);
		else
			printf("Face num: %d\n", detectedFaces3.faceNum);

		//IR图像活体检测
		MInt32 processIRMask = ASF_IR_LIVENESS;
		res = ASFProcessEx_IR(engine.engineHandle(), &offscreen3, &detectedFaces3, processIRMask);
		if (res != MOK)
			printf("ASFProcessEx_IR fail: %d\n", res);
		else
			printf("ASFProcessEx_IR sucess: %d\n", res);

		//获取IR活体信息
		ASF_LivenessInfo irLivenessInfo = { 0 };
		res = ASFGetLivenessScore_IR(engine.engineHandle(), &irLivenessInfo);
		if (res != MOK)
			printf("ASFGetLivenessScore_IR fail: %d\n", res);
		else
			printf("IR Liveness: %d\n", irLivenessInfo.isLive[0]);

		cvReleaseImage(&cutImg1);
		cvReleaseImage(&cutImg2);
		cvReleaseImage(&cutImg3);


	}
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&img3);

	//反初始化
	
	if (engine.uninitEngine())
		printf("ALUninitEngine sucess\n");
	else
		printf("ALUninitEngine fail");

	getchar();
	return 0;
}
