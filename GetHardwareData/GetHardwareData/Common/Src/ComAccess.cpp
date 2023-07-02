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
	COMFile = CreateFile(ComFileName, GENERIC_READ,//�ɶ�����д
		0, // ������FILE_SHARE_READ,FILE_SHARE_WRITE,FILE_SHARE_DELETE
		NULL, // �ް�ȫ��
		OPEN_EXISTING, //���Ѵ����ļ�
		0, // �ļ�����
		NULL//һ����Ч�ľ�����Ѵ��ڵ��豸���A valid handle to a template file with the GENERIC_READ access right
		);
	if (!GetCommState(COMFile,&dcb))  
		return FALSE;   
	int nError = GetLastError();
	if (INVALID_HANDLE_VALUE == COMFile/*COMFileTemp*/ ){
		char chError[256];memset(chError, 0, 256);
		int nBuffLen = 256;
		return false ;
	}
	// ���û�����,����/�����С(�ֽ���)
	SetupComm( /*COMFileTemp*/COMFile,4096,4096) ;
	// ָ�������¼�_�յ��ַ����뻺����
	SetCommMask(/*COMFileTemp*/COMFile, EV_RXCHAR ) ;


	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_115200/115200 ;
	CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
	////�������ڶ��������ʱ
	SetCommTimeouts(/*COMFileTemp*/COMFile, &CommTimeOuts ) ;

	//���ô��ڲ���:������=CBR_115200;ֹͣλ 1��;��У��;8λ
	//	//���ô��ڲ���  
	dcb.BaudRate = CBR_115200;   // ���ò�����CBR_115200  
	dcb.fBinary = TRUE; // ���ö�����ģʽ���˴���������TRUE  
	dcb.fParity = TRUE; // ֧����żУ��  
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
	dcb.fAbortOnError = FALSE;  // �����ڷ������󣬲�����ֹ���ڶ�д  
	dcb.ByteSize = 8;   // ����λ,��Χ:4-8  
	dcb.Parity = NOPARITY; // У��ģʽ  
	dcb.StopBits = 0;   // 1λֹͣλ  

	//�����豸���ƿ�����ͨ���豸
	fRetVal = SetCommState(/*COMFileTemp*/COMFile, &dcb ) ;
	if(!fRetVal) return FALSE;

	//ˢ�建����
	PurgeComm( /*COMFileTemp*/COMFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	//ָ������ִ����չ����
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

	COMFile = CreateFile(ComFileName, GENERIC_READ | GENERIC_WRITE,//�ɶ�����д
		0, // ������FILE_SHARE_READ,FILE_SHARE_WRITE,FILE_SHARE_DELETE
		NULL, // �ް�ȫ��
		OPEN_EXISTING, //���Ѵ����ļ�
		FILE_FLAG_OVERLAPPED, // �ļ�����
		NULL//һ����Ч�ľ�����Ѵ��ڵ��豸���A valid handle to a template file with the GENERIC_READ access right
		);

	if (!GetCommState(COMFile,&dcb))  
		return FALSE;   
	int nError = GetLastError();
	if (INVALID_HANDLE_VALUE == COMFile/*COMFileTemp*/ ){
		char chError[256];memset(chError, 0, 256);
		int nBuffLen = 256;
		return false ;
	}
	// ���û�����,����/�����С(�ֽ���)
	SetupComm( /*COMFileTemp*/COMFile,4096,4096) ;
	// ָ�������¼�_�յ��ַ����뻺����
	SetCommMask(/*COMFileTemp*/COMFile, EV_RXCHAR ) ;


	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_115200/115200 ;
	CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
	////�������ڶ��������ʱ
	SetCommTimeouts(/*COMFileTemp*/COMFile, &CommTimeOuts ) ;

	//���ô��ڲ���:������=CBR_115200;ֹͣλ 1��;��У��;8λ
	//	//���ô��ڲ���  
	dcb.BaudRate = CBR_115200;   // ���ò�����CBR_115200  
	dcb.fBinary = TRUE; // ���ö�����ģʽ���˴���������TRUE  
	dcb.fParity = TRUE; // ֧����żУ��  
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
	dcb.fAbortOnError = FALSE;  // �����ڷ������󣬲�����ֹ���ڶ�д  
	dcb.ByteSize = 8;   // ����λ,��Χ:4-8  
	dcb.Parity = NOPARITY; // У��ģʽ  
	dcb.StopBits = 0;   // 1λֹͣλ  

	//�����豸���ƿ�����ͨ���豸
	fRetVal = SetCommState(/*COMFileTemp*/COMFile, &dcb ) ;
	if(!fRetVal) return FALSE;

	//ˢ�建����
	PurgeComm( /*COMFileTemp*/COMFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	//ָ������ִ����չ����
	EscapeCommFunction( /*COMFileTemp*/COMFile, SETDTR ) ;

	return TRUE ;
}


bool CloseComm(HANDLE &COMFile)
{
	//��ֹ���ж˿������¼�
	SetCommMask(COMFile, 0) ;

	//��������ն˾����ź�
	EscapeCommFunction( COMFile, CLRDTR ) ;

	//����ͨ����Դ����������뻺�����ַ�����ֹ��ͨ����Դ�Ϲ���Ķ���д��//���� 
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