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
	// 发现设备 
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

	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址） 
	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
	displayDeviceInfo(deviceInfoList);

	// 选择需要连接的相机 
	// Select one camera to connect to  
	if (deviceInfoList.nDevNum > 1)
	{
		cameraIndex = selectDevice(deviceInfoList.nDevNum);
	}
	return IMV_OK;
}

int CameraProcesser::SetCameraConfig() {
	
	//设置图像格式
	ret = IMV_SetEnumFeatureSymbol(devHandle, "PixelFormat", /*"Mono8"*/"BayerRG12");
	if (IMV_OK != ret)
	{
		printf("Get feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 设置自动白平衡
	ret = IMV_SetEnumFeatureSymbol(devHandle, "BalanceWhiteAuto", "Continuous");
	if (IMV_OK != ret)
	{
		printf("Get feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 设置帧率
	//ret = IMV_SetDoubleFeatureValue(devHandle, "AcquisitionFrameRate", 36.0);
	//if (IMV_OK != ret)
	//{
	//	printf("Get feature value failed! ErrorCode[%d]\n", ret);
	//	return ret;
	//}

	switch (getCameraFrameMethod)
	{
		// 当前GET_SINGAL_FRAME_RECURRENT 与 GRAB_CALLBACK 方式 设置相同，都放在默认设置中
	//case GET_SINGAL_FRAME_RECURRENT:
	//	break;
	//case GRAB_CALLBACK:
	//	break;
	case FRAME_SOFT_TRIGGER:
		// 设置触发模式 
		// Set trigger mode to On 
		ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerMode", "On");
		if (IMV_OK != ret)
		{
			printf("Set triggerMode value failed! ErrorCode[%d]\n", ret);
			return ret;
		}
		break;
	default:
		// 设置触发模式 
		// Set trigger mode to On 
		ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerMode", "Off");
		if (IMV_OK != ret)
		{
			printf("Set triggerMode value failed! ErrorCode[%d]\n", ret);
			return ret;
		}
		// 设置自动曝光，先注释掉（在设置`TriggerMode`属性为`On`之后，`ExposureAuto`属性不存在，因此设置`ExposureAuto`属性会报错）
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
	//// 保存设备配置到指定的位置CameraConfig_before.xml
	//ret = IMV_SaveDeviceCfg(devHandle, "CameraConfig_before.xml");
	//if (IMV_OK != ret)
	//{
	//	printf("configuration of camera save failed!\n", ret);
	//	return ret;
	//}

	//// 下载设备描述XML文件到CameraConfig.zip
	//ret = IMV_DownLoadGenICamXML(devHandle, "CameraConfig.zip");
	//if (IMV_OK != ret)
	//{
	//	printf("configuration of camera save failed!\n", ret);
	//	return ret;
	//}

	//// 保存设备配置到指定的位置CameraConfig.xml
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

	// 创建保存图像的文件夹
	sprintf(outDir, "%simg_%s", OUT_PATH, strTime);
	if (_mkdir(outDir) == -1)
	{
		// 创建文件夹失败
		printf("mkdir failed! \n");
		getchar();
		return -1;
	}

	// 获取相机id
	int ret = GetCameraId();
	if (IMV_OK != ret)
	{
		printf("GetCameraId failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 创建设备句柄
	// Create Device Handle
	ret = IMV_CreateHandle(&devHandle, modeByIndex, (void*)&cameraIndex);
	if (IMV_OK != ret)
	{
		printf("Create devHandle failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 打开相机 
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
		// 注册数据帧回调函数
		// Register data frame callback function
		ret = IMV_AttachGrabbing(devHandle, /*GetFrameForGrabCallbackThread*/GrabCallbackThread, this);
		if (IMV_OK != ret)
		{
			printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
			return ret;
		}
	}else if (getCameraFrameMethod == GRAB_CALLBACK)
	{
		// 注册数据帧回调函数
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
	// 开始拉流 
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
	// 停止拉流 
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
	// 关闭相机
	// Close camera 
	ret = IMV_Close(devHandle);
	if (IMV_OK != ret)
	{
		printf("Close camera failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	if (devHandle != NULL)
	{
		// 销毁设备句柄
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

	// 开启线程获取帧
	std::thread t(GetSingalFrameRecurrentThread, &cameraProcesser);
	if (t.joinable())
	{
		t.detach();
	}

	// 输入'c'，退出线程
	char c = 'a';
	do
	{
		c = getchar();
	} while (c != 'c' && c != 'C');

	cameraProcesser.bExitThreadFlag = true;
	while (!cameraProcesser.bExitedThread)
	{
		// 等待获取帧线程退出
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
		// 获取一帧图像
		// Get a frame image
		cameraProcesser->ret = IMV_GetFrame(cameraProcesser->devHandle, &frame, 500);
		if (IMV_OK != cameraProcesser->ret)
		{
			printf("Get frame failed! ErrorCode[%d]\n", cameraProcesser->ret);
			return cameraProcesser->ret;
		}

		//nImgNo = frame.frameInfo.blockId;
		printf("Get frame blockId = %llu, %d\n", frame.frameInfo.blockId, nImgNo);

		matImg = imageConvert(cameraProcesser->devHandle, frame, gvspPixelBGR8); // 将图像数据转为RGB8格式
		//cv::Mat matBayer((int)width, (int)height, CV_8UC3, &frame);
		//cvtColor(matBayer, matImg, cv::COLOR_BayerRG2BGR);

		if (!matImg.empty())
		{
			////打开结果保存文件
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

		// 通过睡眠时间来调节帧率
		// Adjust the frame rate by sleep time
		Sleep(50);

		// 释放图像缓存
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
//	//cv::Mat matImg = imageConvert(getFrameCallBackInfo->devHandle, *pFrame, gvspPixelBGR8); // 将图像数据转为RGB8格式
//	//if (!matImg.empty())
//	//{
//	//	nThreadNum++;
//	//	//打开结果保存文件
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
//	cv::Mat matImg = imageConvert(cameraProcesser->devHandle, *pFrame, gvspPixelBGR8); // 将图像数据转为RGB8格式
//		//cv::Mat matBayer((int)width, (int)height, CV_8UC3, &frame);
//		//cvtColor(matBayer, matImg, cv::COLOR_BayerRG2BGR);
//
//	if (!matImg.empty())
//	{
//		////打开结果保存文件
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
//	// 通过睡眠时间来调节帧率
//	// Adjust the frame rate by sleep time
//	Sleep(50);
//
//	return;
//}

// 回调函数获取帧
void GrabCallbackThread(IMV_Frame* pFrame, void* pUser) {
	CameraProcesser* cameraProcesser = (CameraProcesser*)pUser;
	if (pFrame == NULL)
	{
		printf("pFrame is NULL\n");
		return;
	}

	//nImgNo++; /*= pFrame->frameInfo.blockId;*/
	printf("Get frame blockId = %llu, %d\n", pFrame->frameInfo.blockId, nImgNo);

	//cv::Mat matImg = imageConvert(getFrameCallBackInfo->devHandle, *pFrame, gvspPixelBGR8); // 将图像数据转为RGB8格式
	//if (!matImg.empty())
	//{
	//	nThreadNum++;
	//	//打开结果保存文件
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



	cv::Mat matImg = imageConvert(cameraProcesser->devHandle, *pFrame, gvspPixelBGR8); // 将图像数据转为RGB8格式
		//cv::Mat matBayer((int)width, (int)height, CV_8UC3, &frame);
		//cvtColor(matBayer, matImg, cv::COLOR_BayerRG2BGR);

	if (!matImg.empty())
	{
		////打开结果保存文件
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

	// 通过睡眠时间来调节帧率
	// Adjust the frame rate by sleep time
	Sleep(50);

	return;
}

// 回调函数获取帧
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

	// 输入'c'，退出线程
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

	// 开启线程获取帧
	std::thread t(ExecuteSoftTriggerThread, &cameraProcesser);
	if (t.joinable())
	{
		t.detach();
	}

	// 输入'c'，退出线程
	char c = 'a';
	do
	{
		c = getchar();
	} while (c != 'c' && c != 'C');

	cameraProcesser.bExitThreadFlag = true;
	while (!cameraProcesser.bExitedThread)
	{
		// 等待获取帧线程退出
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
//	//cv::Mat matImg = imageConvert(getFrameCallBackInfo->devHandle, *pFrame, gvspPixelBGR8); // 将图像数据转为RGB8格式
//	//if (!matImg.empty())
//	//{
//	//	nThreadNum++;
//	//	//打开结果保存文件
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
//	cv::Mat matImg = imageConvert(cameraProcesser->devHandle, *pFrame, gvspPixelBGR8); // 将图像数据转为RGB8格式
//		//cv::Mat matBayer((int)width, (int)height, CV_8UC3, &frame);
//		//cvtColor(matBayer, matImg, cv::COLOR_BayerRG2BGR);
//
//	if (!matImg.empty())
//	{
//		////打开结果保存文件
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
//	// 通过睡眠时间来调节帧率
//	// Adjust the frame rate by sleep time
//	Sleep(50);
//
//	return;
//}