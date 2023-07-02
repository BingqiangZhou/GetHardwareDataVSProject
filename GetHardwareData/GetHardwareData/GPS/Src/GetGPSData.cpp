
#include "ComAccess.h"
#include "GetGPSData.h"

#include "GPSInfoProcesser.h"

extern int nImgNo;
extern bool bExitGPSLoop; // �˳�GPS�ɼ�ѭ��
extern bool bExitedGPSThread; // �Ƿ��Ѿ��˳�GPS�ɼ�����

unsigned int __stdcall  GPSComReadAndSaveData_Thread(void * lpParameter)
{

#pragma region Ϊ��ȡGPS����������׼��

	// ��ȡ����Ľṹ��ָ��
	struct comST * cst = (struct comST *) lpParameter;
	
	// ��������GPS���ݵ�txt�ļ�������׷�ӵ���ʽ����GPS����
	char outFileName[256];
	sprintf(outFileName, "%sGPS_%s.txt", OUT_PATH, cst->strFileName);
	std::ofstream outFile(outFileName, std::ios::out | std::ios::app);

	// ��ʼ��GPGGA��Ϣ�ṹ�����������Ϊ������뵽�ļ��е�GPS��Ϣ��ʼ���ַ����ռ�
	GPGGA_GPSInfoProcesser gpsData;
	char* strEncoded = new char[256];

	// ��ʼ�����������ַ����ռ䣬����¼��ǰ�ַ�����ʼ�±�
	char *buf = (char *)malloc(4096 * 2);
	int totalnum = 0;

#pragma endregion

#pragma region ѭ����ȡ��������
	while (!bExitGPSLoop)
	{
		DWORD dwEvtMask = 0;
		//WaitCommEvent(cst->COMFile, &dwEvtMask, &(cst->ShareEvent));//�ȴ������¼�
		DWORD dwErrorFlags;
		DWORD dwLength;
		COMSTAT ComStat;
		ClearCommError(cst->COMFile, &dwErrorFlags, &ComStat);
		//dwLength = ComStat.cbInQue;
		dwLength = 4096*2; // ԶԶ����1s GPS���ݵ��ֽ���

		if ((/*dwEvtMask &*/ EV_RXCHAR) == EV_RXCHAR) {
			//		if (com_count > 0) dwLength = NUM_READ;
			DWORD numOutBytes = 0;
#pragma region ��ȡ��������
			if (ReadFile(cst->COMFile, buf + totalnum /*lpszBlock*/, dwLength, &numOutBytes, NULL/*&(cst->ShareEvent)*/) == FALSE)
			{
				// �ж��Ƿ��д���
				if (GetLastError() != ERROR_IO_PENDING)
				{
					ClearCommError(cst->COMFile, &dwErrorFlags, &ComStat);
					PurgeComm(cst->COMFile, PURGE_RXABORT | PURGE_RXCLEAR);
					totalnum = 0; numOutBytes = 0;
				}
				if (GetOverlappedResult(cst->COMFile, &(cst->ShareEvent), &numOutBytes, TRUE) == FALSE)
				{
					ClearCommError(cst->COMFile, &dwErrorFlags, &ComStat);
					PurgeComm(cst->COMFile, PURGE_RXABORT | PURGE_RXCLEAR);
					totalnum = 0; numOutBytes = 0;
				}
			}

			totalnum += numOutBytes;
			buf[totalnum] = '\0';
			//printf("%s \n", buf);
#pragma endregion
			if (totalnum < 128) {
				continue;
			}
			else
			{
				// �����н���GPS����
				char* p;
				p = strtok(buf, "\r\n");
				while (p) {
					//printf("str: %s \n", p);
					gpsData.Decode(p); // ����GPS��GPGGA����
					memset(strEncoded, 0, 256);
					gpsData.Encode(std::vector<int>{6, 1, 2, 4, 9}, " ", strEncoded); // ������Ҫ�õ������ݣ����ں������浽�ļ�
					//gpsData.Encode(std::vector<int>{}, " ", strEncoded); // �����������ݣ����ں������浽�ļ�
					if (strlen(strEncoded) > 5)
					{
						printf("strEncoded: %s", strEncoded);
						outFile << nImgNo << " " << strEncoded; // ������ݵ��ļ���
						outFile.flush(); // ˢ���ļ���������������д�뵽�ļ���
					}
					p = strtok(NULL, "\r\n");
				}
				//printf("%s \n", buf);
				buf[totalnum] = '\0';
				PurgeComm(cst->COMFile, PURGE_RXABORT | PURGE_RXCLEAR);
				totalnum = 0; numOutBytes = 0;
				/*break;*/
			}
		}
	}
#pragma endregion

#pragma region �ر��ļ��������ڴ�

	outFile.close();
	delete[] strEncoded;
	free(buf);

	bExitedGPSThread = true;

#pragma endregion
	return 0;
}