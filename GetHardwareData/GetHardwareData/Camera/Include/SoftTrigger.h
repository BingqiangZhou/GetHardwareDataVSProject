#pragma once

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "IMVApi.h"

//bool g_isExitThread = false;

#define ISEXITTHREAD false

unsigned __stdcall executeSoftTriggerProc(void* pUserData);

// 数据帧回调函数
// Data frame callback function
void onGetFrame(IMV_Frame* pFrame, void* pUser);

int setSoftTriggerConf(IMV_HANDLE devHandle);