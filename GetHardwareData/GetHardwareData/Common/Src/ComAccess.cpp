#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include "common.h"
#include "ComAccess.h"


bool ConnectCommReadOnly(const char * ComFileName, HANDLE &COMFile)
{
	//	BYTE bSet;
	DCB dcb ;
	BOOL fRetVal ;
	COMMTIMEOUTS CommTimeOuts;
	COMFile = CreateFile(ComFileName, GENERIC_READ,//可读、可写
		0, // 不共享FILE_SHARE_READ,FILE_SHARE_WRITE,FILE_SHARE_DELETE
		NULL, // 无安全描
		OPEN_EXISTING, //打开已存在文件
		0, // 文件属性
		NULL//一个有效的句柄，已存在的设备句柄A valid handle to a template file with the GENERIC_READ access right
		);
	if (!GetCommState(COMFile,&dcb))  
		return FALSE;   
	int nError = GetLastError();
	if (INVALID_HANDLE_VALUE == COMFile/*COMFileTemp*/ ){
		char chError[256];memset(chError, 0, 256);
		int nBuffLen = 256;
		return false ;
	}
	// 设置缓冲区,输入/输出大小(字节数)
	SetupComm( /*COMFileTemp*/COMFile,4096,4096) ;
	// 指定监视事件_收到字符放入缓冲区
	SetCommMask(/*COMFileTemp*/COMFile, EV_RXCHAR ) ;


	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_115200/115200 ;
	CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
	////给定串口读与操作限时
	SetCommTimeouts(/*COMFileTemp*/COMFile, &CommTimeOuts ) ;

	//设置串口参数:波特率=CBR_115200;停止位 1个;无校验;8位
	//	//设置串口参数  
	dcb.BaudRate = CBR_115200;   // 设置波特率CBR_115200  
	dcb.fBinary = TRUE; // 设置二进制模式，此处必须设置TRUE  
	dcb.fParity = TRUE; // 支持奇偶校验  
	dcb.fOutxCtsFlow = FALSE;  // No CTS output flow control  
	dcb.fOutxDsrFlow = FALSE;  // No DSR output flow control  
	dcb.fDtrControl = DTR_CONTROL_DISABLE; // No DTR flow control  
	dcb.fDsrSensitivity = FALSE; // DSR sensitivity  
	dcb.fTXContinueOnXoff = TRUE; // XOFF continues Tx  
	dcb.fOutX = FALSE;     // No XON/XOFF out flow control  
	dcb.fInX = FALSE;        // No XON/XOFF in flow control  
	dcb.fErrorChar = FALSE;    // Disable error replacement  
	dcb.fNull = FALSE;  // Disable null stripping  
	dcb.fRtsControl = RTS_CONTROL_DISABLE;   //No RTS flow control  
	dcb.fAbortOnError = FALSE;  // 当串口发生错误，并不终止串口读写  
	dcb.ByteSize = 8;   // 数据位,范围:4-8  
	dcb.Parity = NOPARITY; // 校验模式  
	dcb.StopBits = 0;   // 1位停止位  

	//根据设备控制块配置通信设备
	fRetVal = SetCommState(/*COMFileTemp*/COMFile, &dcb ) ;
	if(!fRetVal) return FALSE;

	//刷清缓冲区
	PurgeComm( /*COMFileTemp*/COMFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	//指定串口执行扩展功能
	EscapeCommFunction( /*COMFileTemp*/COMFile, SETDTR ) ;

	return TRUE ;
}

bool ConnectCommReadWrite(const char * ComFileName, HANDLE &COMFile, OVERLAPPED &ShareEvent )
{
	//	BYTE bSet;
	DCB dcb ;
	BOOL fRetVal ;
	COMMTIMEOUTS CommTimeOuts;

	memset(&ShareEvent, 0, sizeof(ShareEvent));
	ShareEvent.hEvent= CreateEvent( NULL, FALSE, FALSE, NULL ) ;
	if (ShareEvent.hEvent == NULL) return FALSE;  

	COMFile = CreateFile(ComFileName, GENERIC_READ | GENERIC_WRITE,//可读、可写
		0, // 不共享FILE_SHARE_READ,FILE_SHARE_WRITE,FILE_SHARE_DELETE
		NULL, // 无安全描
		OPEN_EXISTING, //打开已存在文件
		FILE_FLAG_OVERLAPPED, // 文件属性
		NULL//一个有效的句柄，已存在的设备句柄A valid handle to a template file with the GENERIC_READ access right
		);

	if (!GetCommState(COMFile,&dcb))  
		return FALSE;   
	int nError = GetLastError();
	if (INVALID_HANDLE_VALUE == COMFile/*COMFileTemp*/ ){
		char chError[256];memset(chError, 0, 256);
		int nBuffLen = 256;
		return false ;
	}
	// 设置缓冲区,输入/输出大小(字节数)
	SetupComm( /*COMFileTemp*/COMFile,4096,4096) ;
	// 指定监视事件_收到字符放入缓冲区
	SetCommMask(/*COMFileTemp*/COMFile, EV_RXCHAR ) ;


	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_115200/115200 ;
	CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
	////给定串口读与操作限时
	SetCommTimeouts(/*COMFileTemp*/COMFile, &CommTimeOuts ) ;

	//设置串口参数:波特率=CBR_115200;停止位 1个;无校验;8位
	//	//设置串口参数  
	dcb.BaudRate = CBR_115200;   // 设置波特率CBR_115200  
	dcb.fBinary = TRUE; // 设置二进制模式，此处必须设置TRUE  
	dcb.fParity = TRUE; // 支持奇偶校验  
	dcb.fOutxCtsFlow = FALSE;  // No CTS output flow control  
	dcb.fOutxDsrFlow = FALSE;  // No DSR output flow control  
	dcb.fDtrControl = DTR_CONTROL_DISABLE; // No DTR flow control  
	dcb.fDsrSensitivity = FALSE; // DSR sensitivity  
	dcb.fTXContinueOnXoff = TRUE; // XOFF continues Tx  
	dcb.fOutX = FALSE;     // No XON/XOFF out flow control  
	dcb.fInX = FALSE;        // No XON/XOFF in flow control  
	dcb.fErrorChar = FALSE;    // Disable error replacement  
	dcb.fNull = FALSE;  // Disable null stripping  
	dcb.fRtsControl = RTS_CONTROL_DISABLE;   //No RTS flow control  
	dcb.fAbortOnError = FALSE;  // 当串口发生错误，并不终止串口读写  
	dcb.ByteSize = 8;   // 数据位,范围:4-8  
	dcb.Parity = NOPARITY; // 校验模式  
	dcb.StopBits = 0;   // 1位停止位  

	//根据设备控制块配置通信设备
	fRetVal = SetCommState(/*COMFileTemp*/COMFile, &dcb ) ;
	if(!fRetVal) return FALSE;

	//刷清缓冲区
	PurgeComm( /*COMFileTemp*/COMFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	//指定串口执行扩展功能
	EscapeCommFunction( /*COMFileTemp*/COMFile, SETDTR ) ;

	return TRUE ;
}


bool CloseComm(HANDLE &COMFile)
{
	//禁止串行端口所有事件
	SetCommMask(COMFile, 0) ;

	//清除数据终端就绪信号
	EscapeCommFunction( COMFile, CLRDTR ) ;

	//丢弃通信资源的输出或输入缓冲区字符并终止在通信资源上挂起的读、写操//场作 
	PurgeComm( COMFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	CloseHandle( COMFile );
	COMFile = NULL;
	return true;
}

int ReadCommBlock(HANDLE COMFile, unsigned char * pbuf, DWORD num)
{
	DWORD dwOutBytes = 0;
	ReadFile( COMFile, pbuf/*lpszBlock*/, num, &dwOutBytes, NULL);
	return ( dwOutBytes ) ;
}