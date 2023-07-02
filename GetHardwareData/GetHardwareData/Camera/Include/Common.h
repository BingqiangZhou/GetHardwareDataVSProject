#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "IMVApi.h"

// ***********开始： 这部分处理与SDK操作相机无关，用于显示设备列表 ***********
// ***********BEGIN: These functions are not related to API call and used to display device info***********
void displayDeviceInfo(IMV_DeviceList deviceInfoList);

char* trim(char* pStr);

int isInputValid(char* pInpuStr);

unsigned int selectDevice(unsigned int cameraCnt);

// ***********结束： 这部分处理与SDK操作相机无关，用于显示设备列表 ***********
// ***********END: These functions are not related to API call and used to display device info***********