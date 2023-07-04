#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include "IMVApi.h"

#include <direct.h>
#include <iostream>
#include <thread>

#include <opencv2/opencv.hpp>

// 获取相机图像方式
enum GetCameraFrameMethod { 
	GET_SINGAL_FRAME_RECURRENT,  // 循环获取单帧，调用接口IMV_GetFrame
	GRAB_CALLBACK, // 回调函数获取帧
	FRAME_SOFT_TRIGGER  // 软触发获取帧
};

// 相机数据处理类
class CameraProcesser
{
private:
	int ret;
	unsigned int cameraIndex; // 相机id,如果是多个相机，则需要手动选择
	//int64_t nImgNo;
	IMV_HANDLE devHandle;	// 相机句柄
	bool bExitThreadFlag;	// 退出获取图像线程标记
	bool bExitedThread;		// 获取图像线程是否已经结束标记
	char outDir[256];		// 输出文件到outDir文件夹
	int64_t width;			// 图像宽度
	int64_t height;			// 图像宽度
	cv::VideoWriter videoWriter;	// 图像保存为avi格式视频,对应的VideoWriter对象
	GetCameraFrameMethod getCameraFrameMethod; // 获取图像帧的方式，包括：循环获取单帧GET_SINGAL_FRAME_RECURRENT、回调函数获取帧GRAB_CALLBACK、 软触发获取帧FRAME_SOFT_TRIGGER

	std::ofstream outFile;

public:
	CameraProcesser();
	~CameraProcesser();

	int InitCameraSetting(char* strTime, GetCameraFrameMethod getCameraFrameMethod); // 初始化相机设置
	int StartGrabbing(); // 开始抓图
	int StopGrabingAndReleaseSource(); // 结束抓图 并 释放资源

	friend int GetFrameForGetSingalFrameRecurrent(char* strTime);// 循环获取单帧
	friend int GetSingalFrameRecurrentThread(CameraProcesser* cameraProcesser); // 循环获取单帧线程

	friend int GetFrameForGrabCallback(char* strTime); // 回调函数获取帧
	//friend void GetFrameForGrabCallbackThread(IMV_Frame* pFrame, void* pUser); // 回调函数获取帧的回调函数

	friend int GetFrameForFrameSoftTrigger(char* strTime); // 软触发获取帧
	friend int ExecuteSoftTriggerThread(CameraProcesser* cameraProcesser);  // 循环执行软触发线程
	//friend int GetFrameForFrameSoftTriggerThread(IMV_Frame* pFrame, void* pUser); // 与GetFrameForGrabCallbackThread功能一致

	friend void GrabCallbackThread(IMV_Frame* pFrame, void* pUser); // GetFrameForGrabCallbackThread与GetFrameForGrabCallbackThread功能一致，统一为同一个

private:

	int GetCameraId(); // 获取相机id，如果是多个相机，则需要手动选择
	int SetCameraConfig(); // 设置相机配置，包括曝光，白平衡，图像格式，触发模式等
	int GetFrameHeightWidth(); // 获取图像帧高宽

};