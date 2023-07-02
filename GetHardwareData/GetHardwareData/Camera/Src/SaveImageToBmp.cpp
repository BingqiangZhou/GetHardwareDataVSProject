#include "SaveImageToBmp.h"

unsigned char* g_pConvertBuf = NULL;
BitmapRGBQuad g_colorTable[256];

extern int nImgNo;

// 检查是否需要申请存放转码数据的内存 
// Check whether malloc buffer for saving the convert data
int mallocConvertBuffer(IMV_HANDLE devHandle)
{
	int ret = IMV_OK;
	uint64_t pixelFormatVal = 0;
	int64_t widthVal = 0;
	int64_t heightVal = 0;

	ret = IMV_GetEnumFeatureValue(devHandle, "PixelFormat", &pixelFormatVal);
	if (IMV_OK != ret)
	{
		printf("Get PixelFormat feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	for (int i = 0; i < 256; i++)
	{
		g_colorTable[i].rgbRed = (unsigned char)i;
		g_colorTable[i].rgbBlue = (unsigned char)i;
		g_colorTable[i].rgbGreen = (unsigned char)i;
		g_colorTable[i].rgbReserved = (unsigned char)0;
	}

	if ((pixelFormatVal == (uint64_t)gvspPixelMono8)
		|| (pixelFormatVal == (uint64_t)gvspPixelBGR8))
	{
		// mono8和BGR8裸数据不需要转码
		// mono8 and BGR8 raw data is not need to convert
		return IMV_OK;
	}

	ret = IMV_GetIntFeatureValue(devHandle, "Width", &widthVal);
	if (IMV_OK != ret)
	{
		printf("Get Width feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	ret = IMV_GetIntFeatureValue(devHandle, "Height", &heightVal);
	if (IMV_OK != ret)
	{
		printf("Get Height feature value failed! ErrorCode[%d]\n", ret);
		return ret;
	}

	g_pConvertBuf = (unsigned char*)malloc(sizeof(unsigned char) * (int)widthVal * (int)heightVal * 3);
	if (g_pConvertBuf == NULL)
	{
		printf("Malloc g_pConvertBuf failed!\n");
		return IMV_NO_MEMORY;
	}

	return IMV_OK;
}



bool saveImageToBmp(IMV_HANDLE devHandle, IMV_Frame *pFrame, char* outDir)
{
	int						ret = IMV_OK;
	IMV_PixelConvertParam stPixelConvertParam;
	unsigned char*			pImageData = NULL;
	IMV_EPixelType		pixelFormat = gvspPixelMono8;
	unsigned int			imageSize = 0;
	unsigned int			uRgbQuadLen = 0;
	char					fileName[256] = { 0 };
	BitmapFileHeader		bmpFileHeader = { 0 };
	BitmapInfoHeader		bmpInfoHeader = { 0 };

	// mono8和BGR8裸数据不需要转码
	// mono8 and BGR8 raw data is not need to convert
	if ((pFrame->frameInfo.pixelFormat != gvspPixelMono8)
		&& (pFrame->frameInfo.pixelFormat != gvspPixelBGR8))
	{
		if (g_pConvertBuf == NULL)
		{
			printf("g_pConvertBuf is NULL\n");
			return false;
		}

		// 图像转换成BGR8
		// convert image to BGR8
		memset(&stPixelConvertParam, 0, sizeof(stPixelConvertParam));
		stPixelConvertParam.nWidth = pFrame->frameInfo.width;
		stPixelConvertParam.nHeight = pFrame->frameInfo.height;
		stPixelConvertParam.ePixelFormat = pFrame->frameInfo.pixelFormat;
		stPixelConvertParam.pSrcData = pFrame->pData;
		stPixelConvertParam.nSrcDataLen = pFrame->frameInfo.size;
		stPixelConvertParam.nPaddingX = pFrame->frameInfo.paddingX;
		stPixelConvertParam.nPaddingY = pFrame->frameInfo.paddingY;
		stPixelConvertParam.eBayerDemosaic = demosaicNearestNeighbor;
		stPixelConvertParam.eDstPixelFormat = gvspPixelBGR8;
		stPixelConvertParam.pDstBuf = g_pConvertBuf;
		stPixelConvertParam.nDstBufSize = pFrame->frameInfo.width * pFrame->frameInfo.height * 3;

		ret = IMV_PixelConvert(devHandle, &stPixelConvertParam);
		if (IMV_OK != ret)
		{
			printf("image convert to BGR failed! ErrorCode[%d]\n", ret);
			return false;
		}

		pImageData = g_pConvertBuf;
		pixelFormat = gvspPixelBGR8;
	}
	else
	{
		pImageData = pFrame->pData;
		pixelFormat = pFrame->frameInfo.pixelFormat;
	}

	//nImgNo++;
	sprintf(fileName, "%s\\realImg_%d.bmp", outDir, /*pFrame->frameInfo.blockId*/nImgNo);

	if (pixelFormat == gvspPixelMono8)
	{
		uRgbQuadLen = (unsigned int)sizeof(g_colorTable);
		imageSize = pFrame->frameInfo.width * pFrame->frameInfo.height;
	}
	else
	{
		uRgbQuadLen = 0;
		imageSize = pFrame->frameInfo.width * pFrame->frameInfo.height * 3;
	}

	// 打开BMP文件
	// open BMP file
	FILE* hFile = fopen(fileName, "wb");
	if (hFile == NULL)
	{
		// 如果打开失败，请用管理权限执行
		// If opefailed, Run as Administrator
		printf("Open file (%s) failed!\n", fileName);
		return false;
	}

	// 设置BMP文件头
	// set BMP file header

	// 文件头类型 'BM'(42 4D)
	// file header type 'BM'(42 4D)
	bmpFileHeader.bfType = 0x4D42;

	// 文件大小
	// file size
	bmpFileHeader.bfSize = BMP_HEADER_SIZE + uRgbQuadLen + imageSize;

	// 位图像素数据的起始位置
	// start position of bitmap pixel data
	bmpFileHeader.bfOffBits = BMP_HEADER_SIZE + uRgbQuadLen;

	// 设置BMP信息头
	// set BMP info header

	// 信息头所占字节数
	// the number of header bytes
	bmpInfoHeader.biSize = (unsigned int)sizeof(bmpInfoHeader);

	// 位图宽度
	// bmp width
	bmpInfoHeader.biWidth = (int)pFrame->frameInfo.width;

	// RGB数据保存为bmp，上下会颠倒，需要设置height为负值
	// bmp height
	bmpInfoHeader.biHeight = -(int)pFrame->frameInfo.height;

	// 位图平面数
	// the number of bitmap planes
	bmpInfoHeader.biPlanes = 1;

	// 像素位数
	// the number of pixels
	bmpInfoHeader.biBitCount = (pixelFormat == gvspPixelMono8) ? 8 : 24;

	// 图像大小
	// the size of image
	bmpInfoHeader.biSizeImage = imageSize;

	// 写入BMP文件头
	// write BMP file header
	fwrite((void*)&bmpFileHeader, 1, sizeof(bmpFileHeader), hFile);

	// 写入BMP信息头
	// write BMP info header
	fwrite((void*)&bmpInfoHeader, 1, sizeof(bmpInfoHeader), hFile);

	if (pixelFormat == gvspPixelMono8)
	{
		// 黑白图像写入颜色表
		// write color table of mono8 image
		fwrite((void*)&g_colorTable, 1, uRgbQuadLen, hFile);
	}

	// 写入图像数据
	// write image data
	fwrite((void*)pImageData, 1, imageSize, hFile);

	// 关闭BMP文件
	// close BMP file
	fclose(hFile);

	return true;
}

void SaveMatToBmp(cv::Mat &matImg, char* fileName) {
	cv::imwrite(fileName, matImg);
	matImg.release();
	std::cout << "SaveMatToBmp end" << std::endl;
	return;
}