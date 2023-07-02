#pragma once

/// \file
/// \~chinese
/// \brief ������ʾ��
/// \example SoftTrigger.cpp
/// \~english
/// \brief soft trigger sample
/// \example SoftTrigger.cpp

//**********************************************************************
// ��DemoΪ����ʾSDK��ʹ�ã�û�и����޸����IP�Ĵ��룬������֮ǰ����ʹ
// ������ͻ����޸����IP��ַ������������������һ�¡�                 
// This Demo shows how to use GenICam API(C) to write a simple program.
// Please make sure that the camera and PC are in the same subnet before running the demo.
// If not, you can use camera client software to modify the IP address of camera to the same subnet with PC. 
// ��������ʾ�˷����豸�������豸�����������������������Ͽ����Ӳ��� 
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

// ����֡�ص�����
// Data frame callback function
void onGetFrame(IMV_Frame* pFrame, void* pUser);

int setSoftTriggerConf(IMV_HANDLE devHandle);