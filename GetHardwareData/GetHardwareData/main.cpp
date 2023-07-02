// GetHardwareData.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <direct.h>

#include "Camera/Include/Common.h"
#include "Camera/Include/ImageConvert.h"
#include "Camera/Include/SoftTrigger.h"
#include "Camera/Include/SaveImageToBmp.h"

#include "Common/Include/ComAccess.h"
#include "GPS/Include/GetGPSData.h"

#include <thread>

#include "CameraProcesser.h"

extern cv::Mat matConvertedImg;
extern int nImgNo = 0;
extern bool bExitGPSLoop = false; // 退出GPS采集循环
extern bool bExitedGPSThread = false; // 是否已经退出GPS采集流程

#define MAX_THREAD 8

struct GetFrameCallBackInfo
{
	char* outDir;
	IMV_HANDLE devHandle;
	cv::VideoWriter videoWriter;
};

int Camera_NotUseThreadToStartSoftTirgger()
{
	std::cout << "Hello World!\n";

	int ret = IMV_OK;
	unsigned int cameraIndex = 0;
	IMV_HANDLE devHandle = NULL;
	HANDLE threadHandle = NULL;

	// 发现设备 
	// discover camera 
	IMV_DeviceList deviceInfoList;
	ret = IMV_EnumDevices(&deviceInfoList, interfaceTypeAll);
	if (IMV_OK != ret)
	{
		printf("Enumeration devices failed! ErrorCode[%d]\n", ret);
		getchar();
		return -1;
	}

	if (deviceInfoList.nDevNum < 1)
	{
		printf("no camera\n");
		getchar();
		return -1;
	}

	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址） 
	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
	displayDeviceInfo(deviceInfoList);

	// 选择需要连接的相机 
	// Select one camera to connect to  
	cameraIndex = selectDevice(deviceInfoList.nDevNum);


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

	//// 保存设备配置到指定的位置CameraConfig_before.xml
	//ret = IMV_SaveDeviceCfg(devHandle, "CameraConfig_before.xml");
	//if (IMV_OK != ret)
	//{
	//	printf("configuration of camera save failed!\n", ret);
	//	return ret;
	//}

	//// 设置自动曝光，先注释掉（在设置`TriggerMode`属性为`On`之后，`ExposureAuto`属性不存在，因此设置`ExposureAuto`属性会报错）
	//ret = IMV_SetEnumFeatureSymbol(devHandle, "ExposureAuto", "Continuous");
	////IMV_EnumEntryList pEnumEntryList;
	////IMV_GetEnumFeatureEntrys(devHandle, "ExposureAuto", &pEnumEntryList);
	////ret = IMV_SetEnumFeatureValue(devHandle, "ExposureAuto", 2);
	//if (IMV_OK != ret)
	//{
	//	printf("Get feature value failed! ErrorCode[%d]\n", ret);
	//	return ret;
	//}

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

	// 设置软触发配置 
	// Set software trigger config 
	ret = setSoftTriggerConf(devHandle);
	if (IMV_OK != ret)
	{
		return ret;
	}

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

	// 注册数据帧回调函数
	// Register data frame callback function
	ret = IMV_AttachGrabbing(devHandle, onGetFrame, &devHandle); // 传入相机句柄
	if (IMV_OK != ret)
	{
		printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 开始拉流 
	// Start grabbing 
	ret = IMV_StartGrabbing(devHandle);
	if (IMV_OK != ret)
	{
		printf("Start grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	do
	{
		// 执行软触发
		// Execute soft trigger 
		ret = IMV_ExecuteCommandFeature(devHandle, "TriggerSoftware");
		if (IMV_OK != ret)
		{
			printf("Execute TriggerSoftware failed! ErrorCode[%d]\n", ret);
			continue;

		}

		// 通过睡眠时间来调节帧率
		// Adjust the frame rate by sleep time
		Sleep(10);

		if (!matConvertedImg.empty())
		{
			//打开结果保存文件
			char fname[256];
			SYSTEMTIME time;
			GetLocalTime(&time);
			sprintf(fname, "rgb_%d_%d_%d_%d_%d_%d_%d.bmp", \
				time.wYear, time.wMonth, time.wDay, time.wHour, \
				time.wMinute, time.wSecond, time.wMilliseconds);
			cv::imwrite(fname, matConvertedImg);
		}

	} while (true);

	matConvertedImg.release();

	// 停止拉流 
// Stop grabbing 
	ret = IMV_StopGrabbing(devHandle);
	if (IMV_OK != ret)
	{
		printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
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

	printf("Press enter key to exit...\n");
	getchar();

	return 0;
}

// 软触发使用线程的情况
int Camera_UseThreadToStartSoftTirgger()
{
	std::cout << "Hello World!\n";

	int ret = IMV_OK;
	unsigned int cameraIndex = 0;
	IMV_HANDLE devHandle = NULL;
	HANDLE threadHandle = NULL;

	// 发现设备 
	// discover camera 
	IMV_DeviceList deviceInfoList;
	ret = IMV_EnumDevices(&deviceInfoList, interfaceTypeAll);
	if (IMV_OK != ret)
	{
		printf("Enumeration devices failed! ErrorCode[%d]\n", ret);
		getchar();
		return -1;
	}

	if (deviceInfoList.nDevNum < 1)
	{
		printf("no camera\n");
		getchar();
		return -1;
	}

	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址） 
	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
	displayDeviceInfo(deviceInfoList);

	// 选择需要连接的相机 
	// Select one camera to connect to  
	cameraIndex = selectDevice(deviceInfoList.nDevNum);

	do
	{
		// 创建设备句柄
		// Create Device Handle
		ret = IMV_CreateHandle(&devHandle, modeByIndex, (void*)&cameraIndex);
		if (IMV_OK != ret)
		{
			printf("Create devHandle failed! ErrorCode[%d]\n", ret);
			break;
		}

		// 打开相机 
		// Open camera 
		ret = IMV_Open(devHandle);
		if (IMV_OK != ret)
		{
			printf("Open camera failed! ErrorCode[%d]\n", ret);
			break;
		}

		//// 保存设备配置到指定的位置CameraConfig_before.xml
		//ret = IMV_SaveDeviceCfg(devHandle, "CameraConfig_before.xml");
		//if (IMV_OK != ret)
		//{
		//	printf("configuration of camera save failed!\n", ret);
		//	return ret;
		//}

		//// 设置自动曝光，先注释掉（在设置`TriggerMode`属性为`On`之后，`ExposureAuto`属性不存在，因此设置`ExposureAuto`属性会报错）
		//ret = IMV_SetEnumFeatureSymbol(devHandle, "ExposureAuto", "Continuous");
		////IMV_EnumEntryList pEnumEntryList;
		////IMV_GetEnumFeatureEntrys(devHandle, "ExposureAuto", &pEnumEntryList);
		////ret = IMV_SetEnumFeatureValue(devHandle, "ExposureAuto", 2);
		//if (IMV_OK != ret)
		//{
		//	printf("Get feature value failed! ErrorCode[%d]\n", ret);
		//	return ret;
		//}

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

		// 设置软触发配置 
		// Set software trigger config 
		ret = setSoftTriggerConf(devHandle);
		if (IMV_OK != ret)
		{
			break;
		}

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

		// 注册数据帧回调函数
		// Register data frame callback function
		ret = IMV_AttachGrabbing(devHandle, onGetFrame, &devHandle); // 传入相机句柄
		if (IMV_OK != ret)
		{
			printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
			break;
		}

		// 开始拉流 
		// Start grabbing 
		ret = IMV_StartGrabbing(devHandle);
		if (IMV_OK != ret)
		{
			printf("Start grabbing failed! ErrorCode[%d]\n", ret);
			break;
		}

		// 创建软触发线程 
		// Create soft trigger thread 
		threadHandle = (HANDLE)_beginthreadex(NULL,
			0,
			executeSoftTriggerProc,
			(void*)devHandle,
			CREATE_SUSPENDED,
			NULL);

		if (!threadHandle)
		{
			printf("Failed to create soft trigger thread!\n");
			break;
		}

		ResumeThread(threadHandle);

		// 启动拉流线程 
		// Start grabbing thread 
		bool g_isExitThread = false;

		// 取图2秒
		// get frame 2 seconds
		Sleep(2000);

		// 退出拉流线程 
		// Stop grabbing thread 
		g_isExitThread = true;

		WaitForSingleObject(threadHandle, INFINITE);
		CloseHandle(threadHandle);

		// 停止拉流 
		// Stop grabbing 
		ret = IMV_StopGrabbing(devHandle);
		if (IMV_OK != ret)
		{
			printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
			break;
		}

		// 关闭相机
		// Close camera 
		ret = IMV_Close(devHandle);
		if (IMV_OK != ret)
		{
			printf("Close camera failed! ErrorCode[%d]\n", ret);
			break;
		}
	} while (false);

	if (devHandle != NULL)
	{
		// 销毁设备句柄
		// Destroy Device Handle
		IMV_DestroyHandle(devHandle);
	}

	printf("Press enter key to exit...\n");
	getchar();

	return 0;
}


// 直接获取单帧

// 软触发使用线程的情况
int Camera_GetSingalFrame(char* strTime)
{
	// 创建保存图像的文件夹

	char outDir[256];
	sprintf(outDir, "%simg_%s", OUT_PATH, strTime);
	if (_mkdir(outDir) == -1)
	{
		// 创建文件夹失败
		printf("mkdir failed! \n");
		getchar();
		return -1;
	}

	int ret = IMV_OK;
	unsigned int cameraIndex = 0;
	IMV_HANDLE devHandle = NULL;
	HANDLE threadHandle = NULL;

	// 发现设备 
	// discover camera 
	IMV_DeviceList deviceInfoList;
	ret = IMV_EnumDevices(&deviceInfoList, interfaceTypeAll);
	if (IMV_OK != ret)
	{
		printf("Enumeration devices failed! ErrorCode[%d]\n", ret);
		getchar();
		return -1;
	}

	if (deviceInfoList.nDevNum < 1)
	{
		printf("no camera\n");
		getchar();
		return -1;
	}

	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址） 
	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
	displayDeviceInfo(deviceInfoList);

	// 选择需要连接的相机 
	// Select one camera to connect to  
	//cameraIndex = selectDevice(deviceInfoList.nDevNum);
	cameraIndex = 0;

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

	//// 保存设备配置到指定的位置CameraConfig_before.xml
	//ret = IMV_SaveDeviceCfg(devHandle, "CameraConfig_before.xml");
	//if (IMV_OK != ret)
	//{
	//	printf("configuration of camera save failed!\n", ret);
	//	return ret;
	//}

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
	ret = IMV_SetDoubleFeatureValue(devHandle, "AcquisitionFrameRate", 36.0);
	if (IMV_OK != ret)
	{
		printf("Get feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

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

	// 检查是否需要申请存放转码数据的内存 
		// Check whether malloc buffer for saving the convert data
	ret = mallocConvertBuffer(devHandle);
	if (IMV_OK != ret)
	{
		printf("mallocConvertBuffer failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 开始拉流 
	// Start grabbing 
	ret = IMV_StartGrabbing(devHandle);
	if (IMV_OK != ret)
	{
		printf("Start grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	int64_t width = 0;
	int64_t height = 0;

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

	IMV_Frame frame;
	cv::Mat matImg;
	char fname[256];
	sprintf(fname, "%s\\realImg.avi", outDir);
	cv::VideoWriter videoWriter = cv::VideoWriter(fname, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 30.0 ,cv::Size(width, height));
	
	do
	{
		// 获取一帧图像
		// Get a frame image
		ret = IMV_GetFrame(devHandle, &frame, 500);
		if (IMV_OK != ret)
		{
			printf("Get frame failed! ErrorCode[%d]\n", ret);
			return ret;
		}

		//nImgNo = frame.frameInfo.blockId;
		printf("Get frame blockId = %llu\n", frame.frameInfo.blockId);

		matImg = imageConvert(devHandle, frame, gvspPixelBGR8); // 将图像数据转为RGB8格式
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
			videoWriter.write(matImg);
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
		ret = IMV_ReleaseFrame(devHandle, &frame);
		if (IMV_OK != ret)
		{
			printf("Release frame failed! ErrorCode[%d]\n", ret);
		}
		
	} while (nImgNo < 1000);

	videoWriter.release();
	matConvertedImg.release();

	// 停止拉流 
// Stop grabbing 
	ret = IMV_StopGrabbing(devHandle);
	if (IMV_OK != ret)
	{
		printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
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

	printf("Press enter key to exit...\n");
	getchar();

	return 0;
}

// 数据帧回调函数
// Data frame callback function
static void onGetFrame(IMV_Frame* pFrame, void* pUser)
{
	GetFrameCallBackInfo* getFrameCallBackInfo = (GetFrameCallBackInfo*)pUser;
	if (pFrame == NULL)
	{
		printf("pFrame is NULL\n");
		return;
	}

	//nImgNo++; /*= pFrame->frameInfo.blockId;*/
	//printf("Get frame blockId = %llu, %d\n", pFrame->frameInfo.blockId, nImgNo);

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

	

	cv::Mat matImg = imageConvert(getFrameCallBackInfo->devHandle, *pFrame, gvspPixelBGR8); // 将图像数据转为RGB8格式
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
		getFrameCallBackInfo->videoWriter.write(matImg);
		nImgNo++;
		//printf("%s \n", fname);
	}

	// 通过睡眠时间来调节帧率
	// Adjust the frame rate by sleep time
	Sleep(50);

	return;
}


// 软触发使用线程的情况
int Camera_GrabCallback(char* strTime)
{
	// 创建保存图像的文件夹

	char outDir[256];
	sprintf(outDir, "%simg_%s", OUT_PATH, strTime);
	if (_mkdir(outDir) == -1)
	{
		// 创建文件夹失败
		printf("mkdir failed! \n");
		getchar();
		return -1;
	}

	int ret = IMV_OK;
	unsigned int cameraIndex = 0;
	IMV_HANDLE devHandle = NULL;
	HANDLE threadHandle = NULL;

	// 发现设备 
	// discover camera 
	IMV_DeviceList deviceInfoList;
	ret = IMV_EnumDevices(&deviceInfoList, interfaceTypeAll);
	if (IMV_OK != ret)
	{
		printf("Enumeration devices failed! ErrorCode[%d]\n", ret);
		getchar();
		return -1;
	}

	if (deviceInfoList.nDevNum < 1)
	{
		printf("no camera\n");
		getchar();
		return -1;
	}

	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址） 
	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
	displayDeviceInfo(deviceInfoList);

	// 选择需要连接的相机 
	// Select one camera to connect to  
	//cameraIndex = selectDevice(deviceInfoList.nDevNum);
	cameraIndex = 0;

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

	//// 保存设备配置到指定的位置CameraConfig_before.xml
	//ret = IMV_SaveDeviceCfg(devHandle, "CameraConfig_before.xml");
	//if (IMV_OK != ret)
	//{
	//	printf("configuration of camera save failed!\n", ret);
	//	return ret;
	//}

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
	ret = IMV_SetDoubleFeatureValue(devHandle, "AcquisitionFrameRate", 10.0);
	if (IMV_OK != ret)
	{
		printf("Get feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

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

	int64_t width = 0;
	int64_t height = 0;

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

	IMV_Frame frame;
	cv::Mat matImg;
	char fname[256];
	sprintf(fname, "%s\\realImg.avi", outDir);
	cv::VideoWriter videoWriter = cv::VideoWriter(fname, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 30.0, cv::Size(width, height));

	GetFrameCallBackInfo getFrameCallBackInfo;
	getFrameCallBackInfo.devHandle = devHandle;
	getFrameCallBackInfo.outDir = outDir;
	getFrameCallBackInfo.videoWriter = videoWriter;

	// 注册数据帧回调函数
		// Register data frame callback function
	ret = IMV_AttachGrabbing(devHandle, onGetFrame, &getFrameCallBackInfo);
	if (IMV_OK != ret)
	{
		printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 检查是否需要申请存放转码数据的内存 
		// Check whether malloc buffer for saving the convert data
	ret = mallocConvertBuffer(devHandle);
	if (IMV_OK != ret)
	{
		printf("mallocConvertBuffer failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 开始拉流 
	// Start grabbing 
	ret = IMV_StartGrabbing(devHandle);
	if (IMV_OK != ret)
	{
		printf("Start grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	char c = 'a';
	do
	{
		c = getchar();
	} while (c != 'c' && c != 'C');

	// 停止拉流 
// Stop grabbing 
	ret = IMV_StopGrabbing(devHandle);
	if (IMV_OK != ret)
	{
		printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	videoWriter.release();
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

	printf("Press enter key to exit...\n");
	getchar();

	return 0;
}

int main() {
	
	std::cout << "Hello World!\n";

#pragma region 获取当前时间，并构建时间字符串

	char strTime[256];
	SYSTEMTIME time;
	GetLocalTime(&time);
	sprintf(strTime, "%d_%d_%d_%d_%d_%d_%d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

#pragma endregion

#pragma region 读取GPS串口数据，进行解析并写入文件，文件名格式类似于GPS_2022_8_28_18_12_35_114.txt(GPS_strTime.txt)
	
	comST st_COM;
	st_COM.strFileName = strTime;
	if (!ConnectCommReadOnly(/*IComFileName*/"COM5", st_COM.COMFile)) {
		printf("main::Inertial COM connect failed.");
	}
	else {
		HANDLE InertialComRead_handle = (HANDLE)_beginthreadex(NULL, 0, GPSComReadAndSaveData_Thread, (void*)&st_COM, 0, NULL); // 开启一个处理GPS数据的线程
	}

#pragma endregion

#pragma region 开启相机保存图像为bmp格式，以realImg_ ImgNo.bmp为命名方式，保存在类似img_2022_8_28_18_12_35_114（img_strTime）的文件夹中

	//Camera_GetSingalFrame(strTime); // 调用IMV_GetFrame接口一次读取单帧
	//Camera_GrabCallback(strTime);
	//Camera_UseThreadToStartSoftTirgger(); // 软触发使用线程的情况
	// Camera_NotUseThreadToStartSoftTirgger(); // 软触发不使用线程，再while循环中调用软触发，并且不在回调函数中保存图像

	GetFrameForGetSingalFrameRecurrent(strTime);
	//GetFrameForGrabCallback(strTime);
	//GetFrameForFrameSoftTrigger(strTime);

	// 退出GPS线程，关闭串口
	bExitGPSLoop = true;
	while (!bExitedGPSThread)
	{
		Sleep(50);
	}
	CloseComm(st_COM.COMFile);

#pragma endregion
}