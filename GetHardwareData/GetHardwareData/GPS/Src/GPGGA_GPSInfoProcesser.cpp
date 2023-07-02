
#include "GPSInfoProcesser.h"
#include <algorithm>
#include <string>


GPGGA_GPSInfoProcesser::GPGGA_GPSInfoProcesser() {

#pragma region 初始化解析器需要用到的变量，包括数据头，字段数，字符串解析中间间隔符

	GPSInfoName = "GPGGA";
	nDelim = 14;
	delim = ",";
	invalidDataValue = "-999999.000000";

#pragma endregion
}

// 从数据列表中解析数据（其他GPS数据类型仅需要定义字段变量以及重写该虚函数即可）
int GPGGA_GPSInfoProcesser::ParserStrListToDataStruct(std::vector<std::string> &strList) {

#pragma region 调用父结构体，获取解析数据列表中的开始位置

	int i = GPSInfoProcesser::ParserStrListToDataStruct(strList);
	if (i < 0)
	{
		return i;
	}

#pragma endregion

#pragma region 将解析得到的数据对应到字段

	gpgga.strUTCTime = std::atof(strList[1].c_str());
	gpgga.fLatitude = std::atof(strList[2].c_str());
	gpgga.strLatitudeFlag = strList[3];
	gpgga.fLongitude = std::atof(strList[4].c_str());
	gpgga.strLongitudeFlag = strList[5];
	gpgga.nGPSState = std::atoi(strList[6].c_str());
	gpgga.nSatellitesNum = std::atoi(strList[7].c_str());
	gpgga.fHDOP = std::atof(strList[8].c_str());
	gpgga.fAltitude = std::atof(strList[9].c_str());
	// 跳过一个'M'字符或者是空字符
	gpgga.fGeoid = std::atof(strList[11].c_str());
	// 跳过一个'M'字符或者是空字符
	gpgga.fDifferentialTime = std::atof(strList[13].c_str());
	if (strList[14].length() > 1)
	{
		int nIndex = strList[14].find("*");
		std::string p = strList[14].substr(0, std::max(nIndex, 0)); // 星号之前的值，例如“*4E”，不是差分定位为空;

		// 如果验证值为空，则说明星号之前没有字符，例如“*4E”
		// 此时不是差分定位，strDifferentialTime也应该为空，将strDifferentialStationId也置为空
		// 将验证值设置为 星号之后的值

		gpgga.strVerificationValue = strList[14].substr(nIndex + 1);
		gpgga.nDifferentialStationId = std::atoi(p.c_str());
	}
	else
	{
		gpgga.strVerificationValue = "";
		gpgga.nDifferentialStationId = 0;
	}

#pragma endregion
	return 0;
};

void GPGGA_GPSInfoProcesser::PushDataStructToStrFieldList() {
	strFieldList.clear();
	strFieldList.resize(14);

	strFieldList[0] = GPSInfoName;
	strFieldList[1] = std::to_string(gpgga.strUTCTime);
	strFieldList[2] = std::to_string(gpgga.fLatitude);
	strFieldList[3] = gpgga.strLatitudeFlag;
	strFieldList[4] = std::to_string(gpgga.fLongitude);
	strFieldList[5] = gpgga.strLongitudeFlag;
	strFieldList[6] = std::to_string(gpgga.nGPSState);
	strFieldList[7] = std::to_string(gpgga.nSatellitesNum);
	strFieldList[8] = std::to_string(gpgga.fHDOP);
	strFieldList[9] = std::to_string(gpgga.fAltitude);
	strFieldList[10] = std::to_string(gpgga.fGeoid);
	strFieldList[11] = std::to_string(gpgga.fDifferentialTime);
	strFieldList[12] = std::to_string(gpgga.nDifferentialStationId);
	strFieldList[13] = gpgga.strVerificationValue;

}