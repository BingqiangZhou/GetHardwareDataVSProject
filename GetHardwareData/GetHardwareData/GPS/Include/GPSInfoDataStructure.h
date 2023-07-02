#pragma once

#include <string>

// $GPGGA，语句ID，表明该语句为Global Positioning System Fix Data（GGA）GPS定位信息
struct GPGGA_GPSInfo {

#pragma region 定义GPGGA数据的字段

	float strUTCTime; // 字段1：UTC 时间，hhmmss.sss，时分秒格式
	float fLatitude; // 字段2：纬度ddmm.mmmm，度分格式（前导位数不足则补0）
	std::string strLatitudeFlag; // 字段3：纬度N（北纬）或S（南纬）
	float fLongitude; // 字段4：经度dddmm.mmmm，度分格式（前导位数不足则补0）
	std::string strLongitudeFlag; // 字段5：经度E（东经）或W（西经）
	int nGPSState;// 字段6：GPS状态，0 = 未定位，1 = 非差分定位，2 = 差分定位，3 = 无效PPS，6 = 正在估算
	int nSatellitesNum;// 字段7：正在使用的卫星数量（00 - 12）（前导位数不足则补0）
	float fHDOP;// 字段8：HDOP水平精度因子（0.5 - 99.9）
	float fAltitude;// 字段9：海拔高度（ - 9999.9 - 99999.9）
	float fGeoid;// 字段10：地球椭球面相对大地水准面的高度
	float fDifferentialTime;// 字段11：差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空）
	int nDifferentialStationId;// 字段12：差分站ID号0000 - 1023（前导位数不足则补0，如果不是差分定位将为空）
	std::string strVerificationValue;// 字段13：校验值

#pragma endregion
	GPGGA_GPSInfo() {	
#pragma region 初始化GPGGA数据的字段

	strUTCTime = NULL;
	fLatitude = NULL;
	strLatitudeFlag = "";
	fLongitude = NULL;
	strLongitudeFlag = "";
	nGPSState = NULL;
	nSatellitesNum = NULL;
	fHDOP = NULL;
	fAltitude = NULL;
	fGeoid = NULL;
	fDifferentialTime = NULL;
	nDifferentialStationId = NULL;
	strVerificationValue = "";

#pragma endregion
	}
};