#ifndef __COM_ACCESS__
#define __COM_ACCESS__

#include <Windows.h>

#define NUM_READ 41

bool ConnectCommReadOnly(const char * ComFileName, HANDLE &COMFile);
bool ConnectCommReadWrite(const char * ComFileName, HANDLE &COMFile, OVERLAPPED &ShareEvent );
bool CloseComm(HANDLE &COMFile);
int ReadCommBlock(HANDLE COMFile, unsigned char * pbuf, DWORD num);

#endif