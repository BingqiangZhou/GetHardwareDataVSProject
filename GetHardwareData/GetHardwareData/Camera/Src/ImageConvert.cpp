/// \file
/// \~chinese
/// \brief 图像转化示例
/// \example ImageConvert.cpp
/// \~english
/// \brief Image convert sample
/// \example ImageConvert.cpp

//**********************************************************************
// 本Demo为简单演示SDK的使用，没有附加修改相机IP的代码，在运行之前，请使
// 用相机客户端修改相机IP地址的网段与主机的网段一致。                 
// This Demo shows how to use GenICam API(C) to write a simple program.
// Please make sure that the camera and PC are in the same subnet before running the demo.
// If not, you can use camera client software to modify the IP address of camera to the same subnet with PC. 
//**********************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include "IMVApi.h"

#include "ImageConvert.h"

cv::Mat matConvertedImg;

//// 图片转化
//// Image convert
//void imageConvert(IMV_HANDLE devHandle, IMV_Frame frame, IMV_EPixelType convertFormat)
//{
//	IMV_PixelConvertParam stPixelConvertParam;
//	unsigned char*			pDstBuf = NULL;
//	unsigned int			nDstBufSize = 0;
//	int						ret = IMV_OK;
//	FILE*					hFile = NULL;
//	const char*				pFileName = NULL;
//	const char*				pConvertFormatStr = NULL;
//
//	int						nCVMatType = CV_8UC1; // 记录输出的Mat数据格式
//
//	switch (convertFormat)
//	{
//		case gvspPixelRGB8:
//			nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 3;
//			//pFileName = (const char*)"convertRGB8.bin";
//			pConvertFormatStr = (const char*)"RGB8";
//			nCVMatType = CV_8UC3;
//			break;
//
//		case gvspPixelBGR8:
//			nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 3;
//			//pFileName = (const char*)"convertBGR8.bin";
//			pConvertFormatStr = (const char*)"BGR8";
//			nCVMatType = CV_8UC3;
//			break;
//		case gvspPixelBGRA8:
//			nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 4;
//			//pFileName = (const char*)"convertBGRA8.bin";
//			pConvertFormatStr = (const char*)"BGRA8";
//			nCVMatType = CV_8UC4;
//			break;
//		case gvspPixelMono8:
//		default:
//			nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height;
//			//pFileName = (const char*)"convertMono8.bin";
//			pConvertFormatStr = (const char*)"Mono8";
//			nCVMatType = CV_8UC1;
//			break;
//	}
//
//	pDstBuf = (unsigned char*)malloc(nDstBufSize);
//	if (NULL == pDstBuf)
//	{
//		printf("malloc pDstBuf failed!\n");
//		return;
//	}
//
//	// 图像转换成BGR8
//	// convert image to BGR8
//	memset(&stPixelConvertParam, 0, sizeof(stPixelConvertParam));
//	stPixelConvertParam.nWidth = frame.frameInfo.width;
//	stPixelConvertParam.nHeight = frame.frameInfo.height;
//	stPixelConvertParam.ePixelFormat = frame.frameInfo.pixelFormat;
//	stPixelConvertParam.pSrcData = frame.pData;
//	stPixelConvertParam.nSrcDataLen = frame.frameInfo.size;
//	stPixelConvertParam.nPaddingX = frame.frameInfo.paddingX;
//	stPixelConvertParam.nPaddingY = frame.frameInfo.paddingY;
//	stPixelConvertParam.eBayerDemosaic = demosaicNearestNeighbor;
//	stPixelConvertParam.eDstPixelFormat = convertFormat;
//	stPixelConvertParam.pDstBuf = pDstBuf;
//	stPixelConvertParam.nDstBufSize = nDstBufSize;
//
//	ret = IMV_PixelConvert(devHandle, &stPixelConvertParam);
//	if (IMV_OK == ret)
//	{
//		printf("image convert to %s successfully! nDstDataLen (%u)\n", 
//			pConvertFormatStr, stPixelConvertParam.nDstBufSize);
//
//		//hFile = fopen(pFileName, "wb");
//		//if (hFile != NULL)
//		//{
//		//	fwrite((void*)pDstBuf, 1, stPixelConvertParam.nDstBufSize, hFile);
//		//	fclose(hFile);
//
//		//}
//		//else
//		//{
//		//	// 如果打开失败，请用管理权限执行
//		//	// If opefailed, Run as Administrator
//		//	printf("Open file (%s) failed!\n", pFileName);
//		//}
//
//		// 转换为Mat对象，并写入文件
//		if (matConvertedImg.empty())
//		{
//			matConvertedImg = cv::Mat(frame.frameInfo.height, frame.frameInfo.width, nCVMatType);
//		}
//
//		memcpy(matConvertedImg.data, (uint8_t*)(pDstBuf), matConvertedImg.rows * matConvertedImg.cols * matConvertedImg.channels());
//
//		//cv::Mat	image = cv::Mat(frame.frameInfo.height, frame.frameInfo.width, nCVMatType, (uint8_t*)(pDstBuf));
//
//		////打开结果保存文件
//		//char fname[256];
//		//char rbuf[256];
//		//SYSTEMTIME time;
//		//GetLocalTime(&time);
//		//sprintf(fname, "rgb_%d_%d_%d_%d_%d_%d_%d.bmp", \
//		//	time.wYear, time.wMonth, time.wDay, time.wHour, \
//		//	time.wMinute, time.wSecond, time.wMilliseconds);
//		//cv::imwrite(fname, image);
//	}
//	else
//	{
//		printf("image convert to %s failed! ErrorCode[%d]\n", pConvertFormatStr, ret);
//	}
//
//	if (pDstBuf)
//	{
//		free(pDstBuf);
//		pDstBuf = NULL;
//	}
//
//	return;
//}

// 图片转化
// Image convert
cv::Mat imageConvert(IMV_HANDLE devHandle, IMV_Frame frame, IMV_EPixelType convertFormat)
{
	IMV_PixelConvertParam stPixelConvertParam;
	unsigned char*			pDstBuf = NULL;
	unsigned int			nDstBufSize = 0;
	int						ret = IMV_OK;
	FILE*					hFile = NULL;
	const char*				pFileName = NULL;
	const char*				pConvertFormatStr = NULL;

	int						nCVMatType = CV_8UC1; // 记录输出的Mat数据格式

	switch (convertFormat)
	{
	case gvspPixelRGB8:
		nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 3;
		//pFileName = (const char*)"convertRGB8.bin";
		pConvertFormatStr = (const char*)"RGB8";
		nCVMatType = CV_8UC3;
		break;

	case gvspPixelBGR8:
		nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 3;
		//pFileName = (const char*)"convertBGR8.bin";
		pConvertFormatStr = (const char*)"BGR8";
		nCVMatType = CV_8UC3;
		break;
	case gvspPixelBGRA8:
		nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 4;
		//pFileName = (const char*)"convertBGRA8.bin";
		pConvertFormatStr = (const char*)"BGRA8";
		nCVMatType = CV_8UC4;
		break;
	case gvspPixelMono8:
	default:
		nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height;
		//pFileName = (const char*)"convertMono8.bin";
		pConvertFormatStr = (const char*)"Mono8";
		nCVMatType = CV_8UC1;
		break;
	}

	cv::Mat matImg;
	pDstBuf = (unsigned char*)malloc(nDstBufSize);
	if (NULL == pDstBuf)
	{
		printf("malloc pDstBuf failed!\n");
		return matImg;
	}
	matImg = cv::Mat(frame.frameInfo.height, frame.frameInfo.width, nCVMatType);
	/*if (matImg.empty())
	{
		matImg = cv::Mat(frame.frameInfo.height, frame.frameInfo.width, nCVMatType);
	}*/

	// 图像转换成BGR8
	// convert image to BGR8
	memset(&stPixelConvertParam, 0, sizeof(stPixelConvertParam));
	stPixelConvertParam.nWidth = frame.frameInfo.width;
	stPixelConvertParam.nHeight = frame.frameInfo.height;
	stPixelConvertParam.ePixelFormat = frame.frameInfo.pixelFormat;
	stPixelConvertParam.pSrcData = frame.pData;
	stPixelConvertParam.nSrcDataLen = frame.frameInfo.size;
	stPixelConvertParam.nPaddingX = frame.frameInfo.paddingX;
	stPixelConvertParam.nPaddingY = frame.frameInfo.paddingY;
	stPixelConvertParam.eBayerDemosaic = demosaicNearestNeighbor;
	stPixelConvertParam.eDstPixelFormat = convertFormat;
	stPixelConvertParam.pDstBuf = pDstBuf;
	stPixelConvertParam.nDstBufSize = nDstBufSize;

	ret = IMV_PixelConvert(devHandle, &stPixelConvertParam);
	if (IMV_OK == ret)
	{
		/*printf("image convert to %s successfully! nDstDataLen (%u)\n",
			pConvertFormatStr, stPixelConvertParam.nDstBufSize);*/

		//hFile = fopen(pFileName, "wb");
		//if (hFile != NULL)
		//{
		//	fwrite((void*)pDstBuf, 1, stPixelConvertParam.nDstBufSize, hFile);
		//	fclose(hFile);

		//}
		//else
		//{
		//	// 如果打开失败，请用管理权限执行
		//	// If opefailed, Run as Administrator
		//	printf("Open file (%s) failed!\n", pFileName);
		//}

		// 转换为Mat对象，并写入文件

		memcpy(matImg.data, (uint8_t*)(pDstBuf), matImg.rows * matImg.cols * matImg.channels());

		//cv::Mat	image = cv::Mat(frame.frameInfo.height, frame.frameInfo.width, nCVMatType, (uint8_t*)(pDstBuf));

		////打开结果保存文件
		//char fname[256];
		//char rbuf[256];
		//SYSTEMTIME time;
		//GetLocalTime(&time);
		//sprintf(fname, "rgb_%d_%d_%d_%d_%d_%d_%d.bmp", \
		//	time.wYear, time.wMonth, time.wDay, time.wHour, \
		//	time.wMinute, time.wSecond, time.wMilliseconds);
		//cv::imwrite(fname, image);
	}
	else
	{
		printf("image convert to %s failed! ErrorCode[%d]\n", pConvertFormatStr, ret);
	}

	if (pDstBuf)
	{
		free(pDstBuf);
		pDstBuf = NULL;
	}

	return matImg;
}


// ***********开始： 这部分处理与SDK操作相机无关，用于显示设备列表 ***********
// ***********BEGIN: These functions are not related to API call and used to display device info***********

//// 选择图像转换目标格式
//// Select target format for image convert 
//static IMV_EPixelType selectConvertFormat(void)
//{
//	char inputStr[256];
//	char* pTrimStr;
//	int inputIndex = -1;
//	int ret = -1;
//	int convertFormatyCnt = 4;
//	IMV_EPixelType convertFormat = gvspPixelMono8;
//	char* find = NULL;
//
//	printf("\n--------------------------------------------\n");
//	printf("\t0.Convert to mono8\n");
//	printf("\t1.Convert to RGB24\n");
//	printf("\t2.Convert to BGR24\n");
//	printf("\t3.Convert to BGRA32\n");
//	printf("--------------------------------------------\n");
//
//	printf("\nPlease select the convert format index: ");
//	while (1)
//	{
//		memset(inputStr, 0, sizeof(inputStr));
//		fgets(inputStr, sizeof(inputStr), stdin);
//		// 清空输入缓存
//		// clear flush
//		fflush(stdin);
//
//		// fgets比gets多吃一个换行符号，取出换行符号 
//		// fgets eats one more line feed symbol than gets, and takes out the line feed symbol
//		find = strchr(inputStr, '\n');
//		if (find) { *find = '\0'; }
//
//		pTrimStr = trim(inputStr);
//		ret = isInputValid(pTrimStr);
//		if (ret == 0)
//		{
//			inputIndex = atoi(pTrimStr);
//			if ((inputIndex >= 0) && (inputIndex < (int)convertFormatyCnt))
//			{
//				break;
//			}
//		}
//
//		printf("Input invalid! Please select the convert format index: ");
//	}
//
//	switch (inputIndex)
//	{
//		case 0: convertFormat = gvspPixelMono8; break;
//		case 1: convertFormat = gvspPixelRGB8; break;
//		case 2: convertFormat = gvspPixelBGR8; break;
//		case 3: convertFormat = gvspPixelBGRA8; break;
//		default: convertFormat = gvspPixelMono8;break;
//	}
//
//	return convertFormat;
//}

// ***********结束： 这部分处理与SDK操作相机无关，用于显示设备列表 ***********
// ***********END: These functions are not related to API call and used to display device info***********

//int main()
//{
//	int ret = IMV_OK;
//	unsigned int cameraIndex = 0;
//	IMV_HANDLE devHandle = NULL;
//	IMV_Frame frame;
//	IMV_EPixelType convertFormat = gvspPixelMono8;
//
//	// 发现设备 
//	// discover camera 
//	IMV_DeviceList deviceInfoList;
//	ret = IMV_EnumDevices(&deviceInfoList, interfaceTypeAll);
//	if (IMV_OK != ret)
//	{
//		printf("Enumeration devices failed! ErrorCode[%d]\n", ret);
//		getchar();
//		return -1;
//	}
//
//	if (deviceInfoList.nDevNum < 1)
//	{
//		printf("no camera\n");
//		getchar();
//		return -1;
//	}
//
//	// 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地址） 
//	// Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address) 
//	displayDeviceInfo(deviceInfoList);
//
//	// 选择需要连接的相机 
//	// Select one camera to connect to  
//	cameraIndex = selectDevice(deviceInfoList.nDevNum);
//
//	do
//	{
//		// 创建设备句柄
//		// Create Device Handle
//		ret = IMV_CreateHandle(&devHandle, modeByIndex, (void*)&cameraIndex);
//		if (IMV_OK != ret)
//		{
//			printf("Create devHandle failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 打开相机 
//		// Open camera 
//		ret = IMV_Open(devHandle);
//		if (IMV_OK != ret)
//		{
//			printf("Open camera failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 开始拉流 
//		// Start grabbing 
//		ret = IMV_StartGrabbing(devHandle);
//		if (IMV_OK != ret)
//		{
//			printf("Start grabbing failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 获取一帧图像
//		// Get a frame image
//		ret = IMV_GetFrame(devHandle, &frame, 500);
//		if (IMV_OK != ret)
//		{
//			printf("Get frame failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 选择图像转换目标格式
//		// Select target format for image convert 
//		convertFormat = selectConvertFormat();
//
//		printf("BlockId (%llu) pixelFormat (%d), Start image convert...\n",
//			frame.frameInfo.blockId, frame.frameInfo.pixelFormat);
//		// 图片转化
//		// Image convert
//		imageConvert(devHandle, frame, convertFormat);
//
//		// 释放图像缓存
//		// Free image buffer
//		ret = IMV_ReleaseFrame(devHandle, &frame);
//		if (IMV_OK != ret)
//		{
//			printf("Release frame failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 停止拉流 
//		// Stop grabbing 
//		ret = IMV_StopGrabbing(devHandle);
//		if (IMV_OK != ret)
//		{
//			printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//
//		// 关闭相机
//		// Close camera 
//		ret = IMV_Close(devHandle);
//		if (IMV_OK != ret)
//		{
//			printf("Close camera failed! ErrorCode[%d]\n", ret);
//			break;
//		}
//	} while (false);
//
//	if (devHandle != NULL)
//	{
//		// 销毁设备句柄
//		// Destroy Device Handle
//		IMV_DestroyHandle(devHandle);
//	}
//
//	printf("Press enter key to exit...\n");
//	getchar();
//
//	return 0;
//}
