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

extern int nImgNo = 0;
extern bool bExitGPSLoop = false; // 退出GPS采集循环
extern bool bExitedGPSThread = false; // 是否已经退出GPS采集流程

extern std::string strGPSInfo = ""; // 记录GPS信息

int main() {
	
	std::cout << "Hello World!\n";

#pragma region 获取当前时间，并构建时间字符串

	char strTime[256];
	SYSTEMTIME time;
	GetLocalTime(&time);
	sprintf(strTime, "%d_%d_%d_%d_%d_%d_%d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

#pragma endregion

#pragma region 读取GPS串口数据，进行解析并写入文件，文件名格式类似于GPS_2022_8_28_18_12_35_114.txt(GPS_strTime.txt)
	
	// 从 文件中读取串口号
	std::string strComName = "COM1";
	try
	{
		std::ifstream readFileStream;
		readFileStream.open("ComConfig.txt");
		readFileStream >> strComName;
		readFileStream.close();
	}
	catch (const std::exception& e)
	{
		printf("COM config file is not exist. : %s", e.what());
		return -1;
	}

	comST st_COM;
	st_COM.strFileName = strTime;
	if (!ConnectCommReadOnly(/*IComFileName*/strComName.c_str(), st_COM.COMFile)) {
		printf("main::Inertial COM connect failed.");
		return -1;
	}
	else {
		HANDLE InertialComRead_handle = (HANDLE)_beginthreadex(NULL, 0, GPSComReadAndSaveData_Thread, (void*)&st_COM, 0, NULL); // 开启一个处理GPS数据的线程
	}

#pragma endregion

#pragma region 开启相机保存图像为bmp格式，以realImg_ ImgNo.bmp为命名方式，保存在类似img_2022_8_28_18_12_35_114（img_strTime）的文件夹中

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