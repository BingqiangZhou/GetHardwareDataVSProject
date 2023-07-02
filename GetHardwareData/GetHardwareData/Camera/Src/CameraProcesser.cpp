#include "CameraProcesser.h"

#include "Common.h"

#include "ImageConvert.h"

extern int nImgNo;

CameraProcesser::CameraProcesser()
{
	ret = IMV_OK;
	cameraIndex = 0;
	devHandle = NULL;
	width = 0;
	height = 0;
	//nImgNo = 0;
	bExitThreadFlag = false;
	bExitedThread = false;
}

CameraProcesser::~CameraProcesser()
{
}

int CameraProcesser::GetCameraId() {
	// �����豸 
	// discover camera 
	IMV_DeviceList deviceInfoList;
	ret = IMV_EnumDevices(&deviceInfoList, interfaceTypeAll);
	if (IMV_OK != ret)
	{
		printf("Enumeration devices failed! ErrorCode[%d]\n", ret);
		getchar();
		return IMV_ERROR;
	}

	if (deviceInfoList.nDevNum < 1)
	{
		printf("no camera\n");
		getchar();
		return IMV_ERROR;
	}

	// ��ӡ���������Ϣ�����,����,��������Ϣ,�ͺ�,���к�,�û��Զ���ID,IP��ַ�� 
	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
	displayDeviceInfo(deviceInfoList);

	// ѡ����Ҫ���ӵ���� 
	// Select one camera to connect to  
	if (deviceInfoList.nDevNum > 1)
	{
		cameraIndex = selectDevice(deviceInfoList.nDevNum);
	}
	return IMV_OK;
}

int CameraProcesser::SetCameraConfig() {
	
	//����ͼ���ʽ
	ret = IMV_SetEnumFeatureSymbol(devHandle, "PixelFormat", /*"Mono8"*/"BayerRG12");
	if (IMV_OK != ret)
	{
		printf("Get feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// �����Զ���ƽ��
	ret = IMV_SetEnumFeatureSymbol(devHandle, "BalanceWhiteAuto", "Continuous");
	if (IMV_OK != ret)
	{
		printf("Get feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// ����֡��
	//ret = IMV_SetDoubleFeatureValue(devHandle, "AcquisitionFrameRate", 36.0);
	//if (IMV_OK != ret)
	//{
	//	printf("Get feature value failed! ErrorCode[%d]\n", ret);
	//	return ret;
	//}

	switch (getCameraFrameMethod)
	{
		// ��ǰGET_SINGAL_FRAME_RECURRENT �� GRAB_CALLBACK ��ʽ ������ͬ��������Ĭ��������
	//case GET_SINGAL_FRAME_RECURRENT:
	//	break;
	//case GRAB_CALLBACK:
	//	break;
	case FRAME_SOFT_TRIGGER:
		// ���ô���ģʽ 
		// Set trigger mode to On 
		ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerMode", "On");
		if (IMV_OK != ret)
		{
			printf("Set triggerMode value failed! ErrorCode[%d]\n", ret);
			return ret;
		}
		break;
	default:
		// ���ô���ģʽ 
		// Set trigger mode to On 
		ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerMode", "Off");
		if (IMV_OK != ret)
		{
			printf("Set triggerMode value failed! ErrorCode[%d]\n", ret);
			return ret;
		}
		// �����Զ��ع⣬��ע�͵���������`TriggerMode`����Ϊ`On`֮��`ExposureAuto`���Բ����ڣ��������`ExposureAuto`���Իᱨ����
		ret = IMV_SetEnumFeatureSymbol(devHandle, "ExposureAuto", "Continuous");
		//IMV_EnumEntryList pEnumEntryList;
		//IMV_GetEnumFeatureEntrys(devHandle, "ExposureAuto", &pEnumEntryList);
		//ret = IMV_SetEnumFeatureValue(devHandle, "ExposureAuto", 2);
		if (IMV_OK != ret)
		{
			printf("Get feature value failed! ErrorCode[%d]\n", ret);
			return ret;
		}
		break;
	}
	//// �����豸���õ�ָ����λ��CameraConfig_before.xml
	//ret = IMV_SaveDeviceCfg(devHandle, "CameraConfig_before.xml");
	//if (IMV_OK != ret)
	//{
	//	printf("configuration of camera save failed!\n", ret);
	//	return ret;
	//}

	//// �����豸����XML�ļ���CameraConfig.zip
	//ret = IMV_DownLoadGenICamXML(devHandle, "CameraConfig.zip");
	//if (IMV_OK != ret)
	//{
	//	printf("configuration of camera save failed!\n", ret);
	//	return ret;
	//}

	//// �����豸���õ�ָ����λ��CameraConfig.xml
	//ret = IMV_SaveDeviceCfg(devHandle, "CameraConfig.xml");
	//if (IMV_OK != ret)
	//{
	//	printf("configuration of camera save failed!\n", ret);
	//	return ret;
	//}
	return IMV_OK;
}

int CameraProcesser::GetFrameHeightWidth() {
	ret = IMV_GetIntFeatureValue(devHandle, "Width", &width);
	if (IMV_OK != ret)
	{
		printf("Get Width feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	ret = IMV_GetIntFeatureValue(devHandle, "Height", &height);
	if (IMV_OK != ret)
	{
		printf("Get Height feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}
	return IMV_OK;
}

int CameraProcesser::InitCameraSetting(char* strTime, GetCameraFrameMethod getCameraFrameMethod) {
	
	this->getCameraFrameMethod = getCameraFrameMethod;

	// ��������ͼ����ļ���
	sprintf(outDir, "%simg_%s", OUT_PATH, strTime);
	if (_mkdir(outDir) == -1)
	{
		// �����ļ���ʧ��
		printf("mkdir failed! \n");
		getchar();
		return -1;
	}

	// ��ȡ���id
	int ret = GetCameraId();
	if (IMV_OK != ret)
	{
		printf("GetCameraId failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// �����豸���
	// Create Device Handle
	ret = IMV_CreateHandle(&devHandle, modeByIndex, (void*)&cameraIndex);
	if (IMV_OK != ret)
	{
		printf("Create devHandle failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// ����� 
	// Open camera 
	ret = IMV_Open(devHandle);
	if (IMV_OK != ret)
	{
		printf("Open camera failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	ret = SetCameraConfig();
	if (IMV_OK != ret)
	{
		printf("SetCameraConfig failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	ret = GetFrameHeightWidth();
	if (IMV_OK != ret)
	{
		printf("GetFrameHeightWidth failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	if (getCameraFrameMethod == FRAME_SOFT_TRIGGER)
	{
		// ע������֡�ص�����
		// Register data frame callback function
		ret = IMV_AttachGrabbing(devHandle, /*GetFrameForGrabCallbackThread*/GrabCallbackThread, this);
		if (IMV_OK != ret)
		{
			printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
			return ret;
		}
	}else if (getCameraFrameMethod == GRAB_CALLBACK)
	{
		// ע������֡�ص�����
		// Register data frame callback function
		ret = IMV_AttachGrabbing(devHandle, /*GetFrameForGrabCallbackThread*/GrabCallbackThread, this);
		if (IMV_OK != ret)
		{
			printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
			return ret;
		}
	}
}

int CameraProcesser::StartGrabbing() {
	// ��ʼ���� 
	// Start grabbing 
	ret = IMV_StartGrabbing(devHandle);
	if (IMV_OK != ret)
	{
		printf("Start grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	char fname[256];
	sprintf(fname, "%s\\realImg.avi", outDir);
	videoWriter = cv::VideoWriter(fname, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 30.0, cv::Size(width, height));
	if (!videoWriter.isOpened())
	{
		return IMV_ERROR;
	}
	return IMV_OK;
}

int CameraProcesser::StopGrabingAndReleaseSource() {
	// ֹͣ���� 
	// Stop grabbing 
	ret = IMV_StopGrabbing(devHandle);
	if (IMV_OK != ret)
	{
		printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
	}
	if (videoWriter.isOpened())
	{
		videoWriter.release();
	}
	// �ر����
	// Close camera 
	ret = IMV_Close(devHandle);
	if (IMV_OK != ret)
	{
		printf("Close camera failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	if (devHandle != NULL)
	{
		// �����豸���
		// Destroy Device Handle
		IMV_DestroyHandle(devHandle);
	}
	return IMV_OK;
}


int GetFrameForGetSingalFrameRecurrent(char* strTime) {
	CameraProcesser cameraProcesser;
	int ret = cameraProcesser.InitCameraSetting(strTime, GET_SINGAL_FRAME_RECURRENT);
	if (ret != IMV_OK)
	{
		printf("InitCameraSetting failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	ret = cameraProcesser.StartGrabbing();
	if (ret != IMV_OK)
	{
		printf("StartGrabbing failed! ErrorCode[%d]\n", ret);
		cameraProcesser.StopGrabingAndReleaseSource();
		return -1;
	}

	// �����̻߳�ȡ֡
	std::thread t(GetSingalFrameRecurrentThread, &cameraProcesser);
	if (t.joinable())
	{
		t.detach();
	}

	// ����'c'���˳��߳�
	char c = 'a';
	do
	{
		c = getchar();
	} while (c != 'c' && c != 'C');

	cameraProcesser.bExitThreadFlag = true;
	while (!cameraProcesser.bExitedThread)
	{
		// �ȴ���ȡ֡�߳��˳�
		Sleep(50);
	}

	printf("Exit Thread, preparing Release Resource... \n");
	cameraProcesser.StopGrabingAndReleaseSource();
	printf("Resource Release. \n");

	return 0;
}

int GetSingalFrameRecurrentThread(CameraProcesser *cameraProcesser) {
	
	IMV_Frame frame;
	cv::Mat matImg;
	do
	{
		// ��ȡһ֡ͼ��
		// Get a frame image
		cameraProcesser->ret = IMV_GetFrame(cameraProcesser->devHandle, &frame, 500);
		if (IMV_OK != cameraProcesser->ret)
		{
			printf("Get frame failed! ErrorCode[%d]\n", cameraProcesser->ret);
			return cameraProcesser->ret;
		}

		//nImgNo = frame.frameInfo.blockId;
		printf("Get frame blockId = %llu, %d\n", frame.frameInfo.blockId, nImgNo);

		matImg = imageConvert(cameraProcesser->devHandle, frame, gvspPixelBGR8); // ��ͼ������תΪRGB8��ʽ
		//cv::Mat matBayer((int)width, (int)height, CV_8UC3, &frame);
		//cvtColor(matBayer, matImg, cv::COLOR_BayerRG2BGR);

		if (!matImg.empty())
		{
			////�򿪽�������ļ�
			//char fname[256];
			//sprintf(fname, "%s\\realImg_%d.bmp", outDir, frame.frameInfo.blockId);
			////SaveMatToBmp(matImg.clone(), fname);
			////std::thread t(SaveMatToBmp, matImg/*.clone()*/, fname);
			////if (t.joinable())
			////{
			////	t.detach();
			////}
			//double start = GetTickCount();
			//cv::imwrite(fname, matImg);
			//double  end = GetTickCount();
			//std::cout << "imwrite GetTickCount:" << end - start << std::endl;
			cameraProcesser->videoWriter.write(matImg);
			nImgNo++;
			//printf("%s \n", fname);
		}
		//if (saveImageToBmp(devHandle, &frame, outDir))
		//{
		//	printf("Save image to bmp successfully!\n");
		//}
		//else
		//{
		//	printf("Save image to bmp failed!\n");
		//}

		// ͨ��˯��ʱ��������֡��
		// Adjust the frame rate by sleep time
		Sleep(50);

		// �ͷ�ͼ�񻺴�
			// Free image buffer
		cameraProcesser->ret = IMV_ReleaseFrame(cameraProcesser->devHandle, &frame);
		if (IMV_OK != cameraProcesser->ret)
		{
			printf("Release frame failed! ErrorCode[%d]\n", cameraProcesser->ret);
		}

	} while (!cameraProcesser->bExitThreadFlag);

	cameraProcesser->bExitedThread = true;
	return 0;
}

//void GetFrameForGrabCallbackThread(IMV_Frame* pFrame, void* pUser) {
//	CameraProcesser* cameraProcesser = (CameraProcesser*)pUser;
//	if (pFrame == NULL)
//	{
//		printf("pFrame is NULL\n");
//		return;
//	}
//
//	//nImgNo++; /*= pFrame->frameInfo.blockId;*/
//	//printf("Get frame blockId = %llu, %d\n", pFrame->frameInfo.blockId, nImgNo);
//
//	//cv::Mat matImg = imageConvert(getFrameCallBackInfo->devHandle, *pFrame, gvspPixelBGR8); // ��ͼ������תΪRGB8��ʽ
//	//if (!matImg.empty())
//	//{
//	//	nThreadNum++;
//	//	//�򿪽�������ļ�
//	//	char fname[256];
//	//	sprintf(fname, "%s\\realImg_%d.bmp", getFrameCallBackInfo->outDir, /*pFrame->frameInfo.blockId*/nImgNo);
//
//	//	//SaveMatToBmp(matImg.clone(), fname);
//	//	std::thread t(SaveMatToBmp, matImg, fname);
//	//	while (nThreadNum > MAX_THREAD)
//	//	{
//	//		Sleep(10);
//	//	}
//	//	/*if (t.joinable())
//	//	{
//	//		t.detach();
//	//	}*/
//	//	Sleep(50);
//	//	t.join();
//	//	matImg.release();
//	//	nThreadNum--;
//	//	//cv::imwrite(fname, matImg);
//	//	//printf("%s \n", fname);
//	//}
//	/*if (saveImageToBmp(getFrameCallBackInfo->devHandle, pFrame, getFrameCallBackInfo->outDir))
//	{
//		printf("Save image to bmp successfully!\n");
//	}
//	else
//	{
//		printf("Save image to bmp failed!\n");
//	}*/
//
//
//
//	cv::Mat matImg = imageConvert(cameraProcesser->devHandle, *pFrame, gvspPixelBGR8); // ��ͼ������תΪRGB8��ʽ
//		//cv::Mat matBayer((int)width, (int)height, CV_8UC3, &frame);
//		//cvtColor(matBayer, matImg, cv::COLOR_BayerRG2BGR);
//
//	if (!matImg.empty())
//	{
//		////�򿪽�������ļ�
//		//char fname[256];
//		//sprintf(fname, "%s\\realImg_%d.bmp", outDir, frame.frameInfo.blockId);
//		////SaveMatToBmp(matImg.clone(), fname);
//		////std::thread t(SaveMatToBmp, matImg/*.clone()*/, fname);
//		////if (t.joinable())
//		////{
//		////	t.detach();
//		////}
//		//double start = GetTickCount();
//		//cv::imwrite(fname, matImg);
//		//double  end = GetTickCount();
//		//std::cout << "imwrite GetTickCount:" << end - start << std::endl;
//		cameraProcesser->videoWriter.write(matImg);
//		nImgNo++;
//		//printf("%s \n", fname);
//	}
//
//	// ͨ��˯��ʱ��������֡��
//	// Adjust the frame rate by sleep time
//	Sleep(50);
//
//	return;
//}

// �ص�������ȡ֡
void GrabCallbackThread(IMV_Frame* pFrame, void* pUser) {
	CameraProcesser* cameraProcesser = (CameraProcesser*)pUser;
	if (pFrame == NULL)
	{
		printf("pFrame is NULL\n");
		return;
	}

	//nImgNo++; /*= pFrame->frameInfo.blockId;*/
	printf("Get frame blockId = %llu, %d\n", pFrame->frameInfo.blockId, nImgNo);

	//cv::Mat matImg = imageConvert(getFrameCallBackInfo->devHandle, *pFrame, gvspPixelBGR8); // ��ͼ������תΪRGB8��ʽ
	//if (!matImg.empty())
	//{
	//	nThreadNum++;
	//	//�򿪽�������ļ�
	//	char fname[256];
	//	sprintf(fname, "%s\\realImg_%d.bmp", getFrameCallBackInfo->outDir, /*pFrame->frameInfo.blockId*/nImgNo);

	//	//SaveMatToBmp(matImg.clone(), fname);
	//	std::thread t(SaveMatToBmp, matImg, fname);
	//	while (nThreadNum > MAX_THREAD)
	//	{
	//		Sleep(10);
	//	}
	//	/*if (t.joinable())
	//	{
	//		t.detach();
	//	}*/
	//	Sleep(50);
	//	t.join();
	//	matImg.release();
	//	nThreadNum--;
	//	//cv::imwrite(fname, matImg);
	//	//printf("%s \n", fname);
	//}
	/*if (saveImageToBmp(getFrameCallBackInfo->devHandle, pFrame, getFrameCallBackInfo->outDir))
	{
		printf("Save image to bmp successfully!\n");
	}
	else
	{
		printf("Save image to bmp failed!\n");
	}*/



	cv::Mat matImg = imageConvert(cameraProcesser->devHandle, *pFrame, gvspPixelBGR8); // ��ͼ������תΪRGB8��ʽ
		//cv::Mat matBayer((int)width, (int)height, CV_8UC3, &frame);
		//cvtColor(matBayer, matImg, cv::COLOR_BayerRG2BGR);

	if (!matImg.empty())
	{
		////�򿪽�������ļ�
		//char fname[256];
		//sprintf(fname, "%s\\realImg_%d.bmp", outDir, frame.frameInfo.blockId);
		////SaveMatToBmp(matImg.clone(), fname);
		////std::thread t(SaveMatToBmp, matImg/*.clone()*/, fname);
		////if (t.joinable())
		////{
		////	t.detach();
		////}
		//double start = GetTickCount();
		//cv::imwrite(fname, matImg);
		//double  end = GetTickCount();
		//std::cout << "imwrite GetTickCount:" << end - start << std::endl;
		cameraProcesser->videoWriter.write(matImg);
		nImgNo++;
		//printf("%s \n", fname);
	}

	// ͨ��˯��ʱ��������֡��
	// Adjust the frame rate by sleep time
	Sleep(50);

	return;
}

// �ص�������ȡ֡
int GetFrameForGrabCallback(char* strTime) {
	CameraProcesser cameraProcesser;
	int ret = cameraProcesser.InitCameraSetting(strTime, GRAB_CALLBACK);
	if (ret != IMV_OK)
	{
		printf("InitCameraSetting failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	ret = cameraProcesser.StartGrabbing();
	if (ret != IMV_OK)
	{
		printf("StartGrabbing failed! ErrorCode[%d]\n", ret);
		cameraProcesser.StopGrabingAndReleaseSource();
		return -1;
	}

	// ����'c'���˳��߳�
	char c = 'a';
	do
	{
		c = getchar();
	} while (c != 'c' && c != 'C');

	Sleep(50);

	printf("Exit Thread, preparing Release Resource... \n");
	cameraProcesser.StopGrabingAndReleaseSource();
	printf("Resource Release. \n");

	return 0;
}

int GetFrameForFrameSoftTrigger(char* strTime) {
	CameraProcesser cameraProcesser;
	int ret = cameraProcesser.InitCameraSetting(strTime, FRAME_SOFT_TRIGGER);
	if (ret != IMV_OK)
	{
		printf("InitCameraSetting failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	ret = cameraProcesser.StartGrabbing();
	if (ret != IMV_OK)
	{
		printf("StartGrabbing failed! ErrorCode[%d]\n", ret);
		cameraProcesser.StopGrabingAndReleaseSource();
		return -1;
	}

	// �����̻߳�ȡ֡
	std::thread t(ExecuteSoftTriggerThread, &cameraProcesser);
	if (t.joinable())
	{
		t.detach();
	}

	// ����'c'���˳��߳�
	char c = 'a';
	do
	{
		c = getchar();
	} while (c != 'c' && c != 'C');

	cameraProcesser.bExitThreadFlag = true;
	while (!cameraProcesser.bExitedThread)
	{
		// �ȴ���ȡ֡�߳��˳�
		Sleep(50);
	}

	printf("Exit Thread, preparing Release Resource... \n");
	cameraProcesser.StopGrabingAndReleaseSource();
	printf("Resource Release. \n");

	return 0;
}

int ExecuteSoftTriggerThread(CameraProcesser *cameraProcesser) {
	do
	{
		cameraProcesser->ret = IMV_ExecuteCommandFeature(cameraProcesser->devHandle, "TriggerSoftware");
		if (IMV_OK != cameraProcesser->ret)
		{
			printf("Execute TriggerSoftware failed! ErrorCode[%d]\n", cameraProcesser->ret);
			continue;
		}

	} while (!cameraProcesser->bExitThreadFlag);

	cameraProcesser->bExitedThread = true;
	return 0;
}

//int GetFrameForFrameSoftTriggerThread(IMV_Frame* pFrame, void* pUser) {
//	CameraProcesser* cameraProcesser = (CameraProcesser*)pUser;
//	if (pFrame == NULL)
//	{
//		printf("pFrame is NULL\n");
//		return;
//	}
//
//	//nImgNo++; /*= pFrame->frameInfo.blockId;*/
//	//printf("Get frame blockId = %llu, %d\n", pFrame->frameInfo.blockId, nImgNo);
//
//	//cv::Mat matImg = imageConvert(getFrameCallBackInfo->devHandle, *pFrame, gvspPixelBGR8); // ��ͼ������תΪRGB8��ʽ
//	//if (!matImg.empty())
//	//{
//	//	nThreadNum++;
//	//	//�򿪽�������ļ�
//	//	char fname[256];
//	//	sprintf(fname, "%s\\realImg_%d.bmp", getFrameCallBackInfo->outDir, /*pFrame->frameInfo.blockId*/nImgNo);
//
//	//	//SaveMatToBmp(matImg.clone(), fname);
//	//	std::thread t(SaveMatToBmp, matImg, fname);
//	//	while (nThreadNum > MAX_THREAD)
//	//	{
//	//		Sleep(10);
//	//	}
//	//	/*if (t.joinable())
//	//	{
//	//		t.detach();
//	//	}*/
//	//	Sleep(50);
//	//	t.join();
//	//	matImg.release();
//	//	nThreadNum--;
//	//	//cv::imwrite(fname, matImg);
//	//	//printf("%s \n", fname);
//	//}
//	/*if (saveImageToBmp(getFrameCallBackInfo->devHandle, pFrame, getFrameCallBackInfo->outDir))
//	{
//		printf("Save image to bmp successfully!\n");
//	}
//	else
//	{
//		printf("Save image to bmp failed!\n");
//	}*/
//
//
//
//	cv::Mat matImg = imageConvert(cameraProcesser->devHandle, *pFrame, gvspPixelBGR8); // ��ͼ������תΪRGB8��ʽ
//		//cv::Mat matBayer((int)width, (int)height, CV_8UC3, &frame);
//		//cvtColor(matBayer, matImg, cv::COLOR_BayerRG2BGR);
//
//	if (!matImg.empty())
//	{
//		////�򿪽�������ļ�
//		//char fname[256];
//		//sprintf(fname, "%s\\realImg_%d.bmp", outDir, frame.frameInfo.blockId);
//		////SaveMatToBmp(matImg.clone(), fname);
//		////std::thread t(SaveMatToBmp, matImg/*.clone()*/, fname);
//		////if (t.joinable())
//		////{
//		////	t.detach();
//		////}
//		//double start = GetTickCount();
//		//cv::imwrite(fname, matImg);
//		//double  end = GetTickCount();
//		//std::cout << "imwrite GetTickCount:" << end - start << std::endl;
//		cameraProcesser->videoWriter.write(matImg);
//		nImgNo++;
//		//printf("%s \n", fname);
//	}
//
//	// ͨ��˯��ʱ��������֡��
//	// Adjust the frame rate by sleep time
//	Sleep(50);
//
//	return;
//}