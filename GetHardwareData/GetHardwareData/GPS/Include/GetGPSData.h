#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include <iostream>
#include <vector>

#include <fstream>

struct comST {
	HANDLE COMFile;
	OVERLAPPED ShareEvent;
	char* strFileName;
};

unsigned int __stdcall  GPSComReadAndSaveData_Thread(void * lpParameter);