#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "IMVApi.h"

#include "opencv2/opencv.hpp"

// 这里内存以1来对齐，参考链接：Pragma Pack(n)内存分配 - 果果小师弟的文章 - 知乎: https ://zhuanlan.zhihu.com/p/122962037
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

// 保存bmp图像
// save image to bmp
bool saveImageToBmp(IMV_HANDLE devHandle, IMV_Frame *pFrame, char* outDir);

// 检查是否需要申请存放转码数据的内存 
// Check whether malloc buffer for saving the convert data
int mallocConvertBuffer(IMV_HANDLE devHandle);

void SaveMatToBmp(cv::Mat &matImg, char* fileName);