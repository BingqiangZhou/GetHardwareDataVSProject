/// \file
/// \~chinese
/// \brief 软触发示例
/// \example SoftTrigger.cpp
/// \~english
/// \brief soft trigger sample
/// \example SoftTrigger.cpp

//**********************************************************************
// 本Demo为简单演示SDK的使用，没有附加修改相机IP的代码，在运行之前，请使
// 用相机客户端修改相机IP地址的网段与主机的网段一致。                 
// This Demo shows how to use GenICam API(C) to write a simple program.
// Please make sure that the camera and PC are in the same subnet before running the demo.
// If not, you can use camera client software to modify the IP address of camera to the same subnet with PC. 
// 本工程演示了发现设备，连接设备，设置软触发，软触发，断开连接操作 
// This program shows how to discover and connect device, set software trigger config, execute software trigger and disconnect device 
//**********************************************************************
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <Windows.h>
//#include <process.h>
//#include "IMVApi.h"

#include "ImageConvert.h"
#include "SoftTrigger.h"

#include "opencv2/opencv.hpp"

//bool g_isExitThread = false;

extern cv::Mat matConvertedImg;

unsigned __stdcall executeSoftTriggerProc(void* pUserData)
{
	int ret = IMV_OK;
	IMV_HANDLE devHandle = (IMV_HANDLE)pUserData;
	if (NULL == devHandle)
	{
		return 0;
	}

	while (!ISEXITTHREAD)
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
		Sleep(50);

		////打开结果保存文件
		char fname[256];
		char rbuf[256];
		SYSTEMTIME time;
		GetLocalTime(&time);
		sprintf(fname, "rgb_%d_%d_%d_%d_%d_%d_%d.bmp", \
			time.wYear, time.wMonth, time.wDay, time.wHour, \
			time.wMinute, time.wSecond, time.wMilliseconds);
		cv::imwrite(fname, matConvertedImg);
	}

	return 0;
}

// 数据帧回调函数
// Data frame callback function
void onGetFrame(IMV_Frame* pFrame, void* pUser)
{
	if (pFrame == NULL)
	{
		printf("pFrame is NULL\n");
		return;
	}

	IMV_HANDLE* devHandle = (IMV_HANDLE*)pUser;

	imageConvert(*devHandle, *pFrame, gvspPixelBGR8); // 将图像数据转为RGB8格式

	//cv::Mat image = cv::Mat(pFrame->frameInfo.height, pFrame->frameInfo.width, CV_8U, (uint8_t*)((pFrame->pData)));


	printf("Get frame blockId = %llu\n", pFrame->frameInfo.blockId);

	return;
}

int setSoftTriggerConf(IMV_HANDLE devHandle)
{
	int ret = IMV_OK;

	// 设置触发源为软触发 
	// Set trigger source to Software 
	ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerSource", "Software");
	if (IMV_OK != ret)
	{
		printf("Set triggerSource value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 设置触发器 
	// Set trigger selector to FrameStart 
	ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerSelector", "FrameStart");
	if (IMV_OK != ret)
	{
		printf("Set triggerSelector value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	// 设置触发模式 
	// Set trigger mode to On 
	ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerMode", "On");
	if (IMV_OK != ret)
	{
		printf("Set triggerMode value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	return ret;
}

//int main()
//{
//	int ret = IMV_OK;
//	unsigned int cameraIndex = 0;
//	IMV_HANDLE devHandle = NULL;
//	HANDLE threadHandle = NULL;
//
//	// 发现设备 
//	// discover camera 
//	IMV_DeviceList deviceInfoList;
//	ret = IMV_EnumDevices(&deviceInfoList, interfaceTypeAll);
//	if (IMV_OK != ret)
//	{
//		printf("Enumeration devices failed! ErrorCode[%d]\n", ret);
//		getchar();
//		return -1;
//	}
//
//	if (deviceInfoList.nDevNum < 1)
//	{
//		printf("no camera\n");
//		getchar();
//		return -1;
//	}
//
//	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址） 
//	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
//	displayDeviceInfo(deviceInfoList);
//
//	// 选择需要连接的相机 
//	// Select one camera to connect to  
//	cameraIndex = selectDevice(deviceInfoList.nDevNum);
//
//	do
//	{
//		// 创建设备句柄
//		// Create Device Handle
//		ret = IMV_CreateHandle(&devHandle, modeByIndex, (void*)&cameraIndex);
//		if (IMV_OK != ret)
//		{
//			printf("Create devHandle failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 打开相机 
//		// Open camera 
//		ret = IMV_Open(devHandle);
//		if (IMV_OK != ret)
//		{
//			printf("Open camera failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 设置软触发配置 
//		// Set software trigger config 
//		ret = setSoftTriggerConf(devHandle);
//		if (IMV_OK != ret)
//		{
//			break;
//		}
//
//		// 注册数据帧回调函数
//		// Register data frame callback function
//		ret = IMV_AttachGrabbing(devHandle, onGetFrame, NULL);
//		if (IMV_OK != ret)
//		{
//			printf("Attach grabbing failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 开始拉流 
//		// Start grabbing 
//		ret = IMV_StartGrabbing(devHandle);
//		if (IMV_OK != ret)
//		{
//			printf("Start grabbing failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 创建软触发线程 
//		// Create soft trigger thread 
//		threadHandle = (HANDLE)_beginthreadex(NULL,
//			0,
//			executeSoftTriggerProc,
//			(void*)devHandle,
//			CREATE_SUSPENDED,
//			NULL);
//
//		if (!threadHandle)
//		{
//			printf("Failed to create soft trigger thread!\n");
//			break;
//		}
//
//		ResumeThread(threadHandle);
//
//		// 启动拉流线程 
//		// Start grabbing thread 
//		g_isExitThread = false;
//
//		// 取图2秒
//		// get frame 2 seconds
//		Sleep(2000);
//
//		// 退出拉流线程 
//		// Stop grabbing thread 
//		g_isExitThread = true;
//
//		WaitForSingleObject(threadHandle, INFINITE);
//		CloseHandle(threadHandle);
//
//		// 停止拉流 
//		// Stop grabbing 
//		ret = IMV_StopGrabbing(devHandle);
//		if (IMV_OK != ret)
//		{
//			printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 关闭相机
//		// Close camera 
//		ret = IMV_Close(devHandle);
//		if (IMV_OK != ret)
//		{
//			printf("Close camera failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//	} while (false);
//
//	if (devHandle != NULL)
//	{
//		// 销毁设备句柄
//		// Destroy Device Handle
//		IMV_DestroyHandle(devHandle);
//	}
//
//	printf("Press enter key to exit...\n");
//	getchar();
//
//	return 0;
//}
