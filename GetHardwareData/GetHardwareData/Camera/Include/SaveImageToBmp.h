#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "IMVApi.h"

#include "opencv2/opencv.hpp"

// �����ڴ���1�����룬�ο����ӣ�Pragma Pack(n)�ڴ���� - ����Сʦ�ܵ����� - ֪��: https ://zhuanlan.zhihu.com/p/122962037
#pragma pack(1)
typedef struct _BitmapFileHeader {
	unsigned short	bfType;
	unsigned int	bfSize;
	unsigned short	bfReserved1;
	unsigned short	bfReserved2;
	unsigned int	bfOffBits;
} BitmapFileHeader;

typedef struct _BitmapInfoHeader {
	unsigned int	biSize;
	int				biWidth;
	int				biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned int	biCompression;
	unsigned int	biSizeImage;
	int				biXPelsPerMeter;
	int				biYPelsPerMeter;
	unsigned int	biClrUsed;
	unsigned int	biClrImportant;
} BitmapInfoHeader;

typedef struct _BitmapRGBQuad {
	unsigned char    rgbBlue;
	unsigned char    rgbGreen;
	unsigned char    rgbRed;
	unsigned char    rgbReserved;
} BitmapRGBQuad;
#pragma pack()

#define BMP_HEADER_SIZE		(sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader))

// ����bmpͼ��
// save image to bmp
bool saveImageToBmp(IMV_HANDLE devHandle, IMV_Frame *pFrame, char* outDir);

// ����Ƿ���Ҫ������ת�����ݵ��ڴ� 
// Check whether malloc buffer for saving the convert data
int mallocConvertBuffer(IMV_HANDLE devHandle);

void SaveMatToBmp(cv::Mat &matImg, char* fileName);