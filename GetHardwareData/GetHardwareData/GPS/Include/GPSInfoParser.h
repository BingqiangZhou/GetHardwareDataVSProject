#pragma once

#include <vector>
#include <string.h>

// GPS数据解析父结构体
struct GPSInfoParser {

#pragma region 定义GPS数据解析器需要使用到的变量

	char* GPSInfoName; // 需要解析的GPS数据名（也是数据头）,例如一下GPS数据中的GPGGA、GPGSA、GPGSV、GPRMC、GPVTG
	//	$GPGGA, , , , , , 0, 00, 25.5, , , , , , *7A
	//	$GPGSA, A, 1, , , , , , , , , , , , , 25.5, 25.5, 25.5, 1 * 1F
	//	$GPGSV, 1, 1, 00, 0 * 65
	//	$GPRMC, , V, , , , , , , , , , M, V * 2A
	//	$GPVTG, , , , , , , , , M * 33
	int nFields; // GPS数据中有多少个字段，用于判断解析是否正确
	std::vector<char*> strList; // 解析的结果列表
	char* delim; // GPS数据中用到的间隔符

#pragma endregion

	GPSInfoParser() {
		delim = ",";
		GPSInfoName = "";
		nFields = 0;
	}

#pragma region 解析器解析函数Parser

	// 从字符串中解析数据
	int Parser(char* strLine) {
		SplitLine(strLine, delim, strList); // 解析数据到strList
		return ParserStrList(strList); // 将数据对应到字段变量
	}

#pragma endregion

#pragma region 以一定格式输出相关数据的编码函数Encoder

	// 以一定的格式将需要的用到的数据编码到字符串中
	int Encoder(std::vector<int> IndexList, char* delim, char* strEncoded) {
		// IndexList列表中下标对应的数据是需要用到的数据，delim是数据之间的间隔符，strEncoded是输出的字符串

		// 检测下标是否超过vector长度
		for (size_t i = 0; i < IndexList.size(); i++)
		{
			if (IndexList[i] >= strList.size())
			{
				return -1;
			}
		}
		if (IndexList.size() > 0)
		{
			// 当IndexList列表size为0时，仅编码IndexList列表中下标对应的数据
			for (size_t i = 0; i < IndexList.size(); i++) {
				strcat(strEncoded, strList[IndexList[i]]);
				strcat(strEncoded, delim);
			}
			strcat(strEncoded, "\n");
		}
		else {
			// 当IndexList列表size为0时，则编码解析到的全部信息
			for (size_t i = 0; i < strList.size(); i++) {
				strcat(strEncoded, strList[i]);
				strcat(strEncoded, delim);
			}
			strcat(strEncoded, "\n");
		}
		return 0;
	}

#pragma endregion

#pragma region 字符串分割函数SplitLine
	// 将字符串pLine按字符delim进行分割，存入strList中
	void SplitLine(char* pLine, char* delim, std::vector<char*> &strList) {
		if (strList.size() > 0)
		{
			strList.clear();
		}
		// 当数据不出现空值的时候，提取会不准确
		/*char* p;
		p = strtok(pLine, delim);
		while (p) {
			strList.push_back(p);
			p = strtok(NULL, delim);
		}*/
		char* pStart = pLine;
		int nLen = strlen(pLine);
		for (size_t i = 0; i < nLen; i++)
		{
			if (pLine[i] == delim[0])
			{
				pLine[i] = '\0'; // 将“,”改为'\0',即标记此处为一个字符串的结尾
				strList.push_back(pStart);
				pStart = pLine + (i + 1);
				if (pStart[0] == delim[0]) // 出现连续的","则需要跳过一个
				{
					pStart++;
				}
			}
		}
	}
#pragma endregion

#pragma region 将数据对应到字段变量函数ParserStrList

	// 从数据列表中解析数据（其他GPS数据类型仅需要定义字段变量以及重写该虚函数即可）
	virtual int ParserStrList(std::vector<char*> &strList) {
		if (strList.size() == 0)
		{
			return -1;
		}
		int i = 0;
		// 查找是否存在数据头，如果存在，则size应该加1(nFields + i)，例如"GPGGA"信息原本的字段是13个，但是存在数据头，size+1变为14 
		if (-1 == std::string(strList[i]).find(GPSInfoName, strlen(GPSInfoName)))
		{
			i++;
		}
		if (strList.size() != nFields + i)
		{
			//std::cout << "Data Format Error" << std::endl;
			strList.clear();
			return -1;
		}
		return i;
	};

#pragma endregion
};

// $GPGGA，语句ID，表明该语句为Global Positioning System Fix Data（GGA）GPS定位信息
struct GPGGA_GPSInfoParser : GPSInfoParser {

#pragma region 定义GPGGA数据的字段

	char* strUTCTime; // 字段1：UTC 时间，hhmmss.sss，时分秒格式
	char* strLatitude; // 字段2：纬度ddmm.mmmm，度分格式（前导位数不足则补0）
	char* strLatitudeFlag; // 字段3：纬度N（北纬）或S（南纬）
	char* strLongitude; // 字段4：经度dddmm.mmmm，度分格式（前导位数不足则补0）
	char* strLongitudeFlag; // 字段5：经度E（东经）或W（西经）
	char* strGPSState;// 字段6：GPS状态，0 = 未定位，1 = 非差分定位，2 = 差分定位，3 = 无效PPS，6 = 正在估算
	char* strSatellitesNum;// 字段7：正在使用的卫星数量（00 - 12）（前导位数不足则补0）
	char* strHDOP;// 字段8：HDOP水平精度因子（0.5 - 99.9）
	char* strAltitude;// 字段9：海拔高度（ - 9999.9 - 99999.9）
	char* strGeoid;// 字段10：地球椭球面相对大地水准面的高度
	char* strDifferentialTime;// 字段11：差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空）
	char* strDifferentialStationId;// 字段12：差分站ID号0000 - 1023（前导位数不足则补0，如果不是差分定位将为空）
	char* strVerificationValue;// 字段13：校验值

#pragma endregion

	GPGGA_GPSInfoParser() {

#pragma region 初始化GPGGA数据的字段

		strUTCTime = NULL;
		strLatitude = NULL;
		strLatitudeFlag = NULL;
		strLongitude = NULL;
		strLongitudeFlag = NULL;
		strGPSState = NULL;
		strSatellitesNum = NULL;
		strHDOP = NULL;
		strAltitude = NULL;
		strGeoid = NULL;
		strDifferentialTime = NULL;
		strDifferentialStationId = NULL;
		strVerificationValue = NULL;

#pragma endregion

#pragma region 初始化解析器需要用到的变量，包括数据头，字段数，字符串解析中间间隔符

		this->GPSInfoName = "GPGGA";
		this->nFields = 13;
		this->delim = ",";

#pragma endregion
	}

	// 从数据列表中解析数据（其他GPS数据类型仅需要定义字段变量以及重写该虚函数即可）
	virtual int ParserStrList(std::vector<char*> &strList) {

#pragma region 调用父结构体，获取解析数据列表中的开始位置

		int i = GPSInfoParser::ParserStrList(strList);
		if (i < 0)
		{
			return i;
		}

#pragma endregion

#pragma region 将解析得到的数据对应到字段

		strUTCTime = strList[i++];
		strLatitude = strList[i++];
		strLatitudeFlag = strList[i++];
		strLongitude = strList[i++];
		strLongitudeFlag = strList[i++];
		strGPSState = strList[i++];
		strSatellitesNum = strList[i++];
		strHDOP = strList[i++];
		strAltitude = strList[i++];
		strGeoid = strList[i++];
		strDifferentialTime = strList[i++];
		strDifferentialStationId = strList[i++];
		strVerificationValue = strList[i++];

#pragma endregion
		return 0;
	};
};