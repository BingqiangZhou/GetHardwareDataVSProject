#pragma once

#include <vector>
#include <string.h>

#include "GPSInfoDataStructure.h"

// GPS数据处理父类
class GPSInfoProcesser {

public:
#pragma region 定义GPS数据解析器需要使用到的变量

	char* GPSInfoName; // 需要解析的GPS数据名（也是数据头）,例如一下GPS数据中的GPGGA、GPGSA、GPGSV、GPRMC、GPVTG
					   //	$GPGGA, , , , , , 0, 00, 25.5, , , , , , *7A
					   //	$GPGSA, A, 1, , , , , , , , , , , , , 25.5, 25.5, 25.5, 1 * 1F
					   //	$GPGSV, 1, 1, 00, 0 * 65
					   //	$GPRMC, , V, , , , , , , , , , M, V * 2A
					   //	$GPVTG, , , , , , , , , M * 33
	std::vector<std::string> strList; // 解析的结果列表
	std::vector<std::string> strFieldList; // 数据字段的列表
	char* delim; // GPS数据中用到的间隔符
	int nDelim; // GPS数据中有多少个间隔符，用于判断解析是否正确
	std::string invalidDataValue; // 解析出来无效值的替代值

#pragma endregion

	GPSInfoProcesser();

#pragma region 解码器相关方法

	int Decode(char* strLine);
	void SplitLineStrToStrList(char* pLineStr, char* delim, std::vector<std::string> &strList);
	virtual int ParserStrListToDataStruct(std::vector<std::string> &strList);

#pragma endregion

#pragma region 编码器相关方法

	int Encode(std::vector<int> IndexList, char* delim, char* strEncoded);
	virtual void PushDataStructToStrFieldList();
	int EncodeStrFieldListToStr(std::vector<int> IndexList, char* delim, char* strEncoded);

#pragma endregion
};


// $GPGGA，语句ID，表明该语句为Global Positioning System Fix Data（GGA）GPS定位信息
class GPGGA_GPSInfoProcesser : public GPSInfoProcesser {

public:
#pragma region 定义GPGGA数据的字段

	GPGGA_GPSInfo gpgga;

#pragma endregion

	GPGGA_GPSInfoProcesser();

	// 从数据列表中解析数据（其他GPS数据类型仅需要定义字段变量以及重写该虚函数即可）
	virtual int ParserStrListToDataStruct(std::vector<std::string> &strList);
	virtual void PushDataStructToStrFieldList();

};