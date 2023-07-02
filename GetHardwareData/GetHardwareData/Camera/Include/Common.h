#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "IMVApi.h"

// ***********��ʼ�� �ⲿ�ִ�����SDK��������޹أ�������ʾ�豸�б� ***********
// ***********BEGIN: These functions are not related to API call and used to display device info***********
void displayDeviceInfo(IMV_DeviceList deviceInfoList);

char* trim(char* pStr);

int isInputValid(char* pInpuStr);

unsigned int selectDevice(unsigned int cameraCnt);

// ***********������ �ⲿ�ִ�����SDK��������޹أ�������ʾ�豸�б� ***********
// ***********END: These functions are not related to API call and used to display device info***********