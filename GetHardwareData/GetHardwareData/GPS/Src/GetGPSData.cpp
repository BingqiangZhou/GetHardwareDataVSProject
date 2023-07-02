
#include "ComAccess.h"
#include "GetGPSData.h"

#include "GPSInfoProcesser.h"

extern int nImgNo;
extern bool bExitGPSLoop; // 退出GPS采集循环
extern bool bExitedGPSThread; // 是否已经退出GPS采集流程

unsigned int __stdcall  GPSComReadAndSaveData_Thread(void * lpParameter)
{

#pragma region 为读取GPS串口数据作准备

	// 获取传入的结构体指针
	struct comST * cst = (struct comST *) lpParameter;
	
	// 创建保存GPS数据的txt文件，并以追加的形式加入GPS数据
	char outFileName[256];
	sprintf(outFileName, "%sGPS_%s.txt", OUT_PATH, cst->strFileName);
	std::ofstream outFile(outFileName, std::ios::out | std::ios::app);

	// 初始化GPGGA信息结构体解析器，并为最后输入到文件中的GPS信息初始化字符串空间
	GPGGA_GPSInfoProcesser gpsData;
	char* strEncoded = new char[256];

	// 初始化串口数据字符串空间，并记录当前字符串开始下标
	char *buf = (char *)malloc(4096 * 2);
	int totalnum = 0;

#pragma endregion

#pragma region 循环读取串口数据
	while (!bExitGPSLoop)
	{
		DWORD dwEvtMask = 0;
		//WaitCommEvent(cst->COMFile, &dwEvtMask, &(cst->ShareEvent));//等待串口事件
		DWORD dwErrorFlags;
		DWORD dwLength;
		COMSTAT ComStat;
		ClearCommError(cst->COMFile, &dwErrorFlags, &ComStat);
		//dwLength = ComStat.cbInQue;
		dwLength = 4096*2; // 远远大于1s GPS数据的字节数

		if ((/*dwEvtMask &*/ EV_RXCHAR) == EV_RXCHAR) {
			//		if (com_count > 0) dwLength = NUM_READ;
			DWORD numOutBytes = 0;
#pragma region 读取串口数据
			if (ReadFile(cst->COMFile, buf + totalnum /*lpszBlock*/, dwLength, &numOutBytes, NULL/*&(cst->ShareEvent)*/) == FALSE)
			{
				// 判断是否有错误
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
				// 根据行解析GPS数据
				char* p;
				p = strtok(buf, "\r\n");
				while (p) {
					//printf("str: %s \n", p);
					gpsData.Decode(p); // 解析GPS中GPGGA数据
					memset(strEncoded, 0, 256);
					gpsData.Encode(std::vector<int>{6, 1, 2, 4, 9}, " ", strEncoded); // 编码需要用到的数据，用于后续保存到文件
					//gpsData.Encode(std::vector<int>{}, " ", strEncoded); // 编码所有数据，用于后续保存到文件
					if (strlen(strEncoded) > 5)
					{
						printf("strEncoded: %s", strEncoded);
						outFile << nImgNo << " " << strEncoded; // 输出数据到文件中
						outFile.flush(); // 刷新文件缓冲区，将数据写入到文件中
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

#pragma region 关闭文件，清理内存

	outFile.close();
	delete[] strEncoded;
	free(buf);

	bExitedGPSThread = true;

#pragma endregion
	return 0;
}
