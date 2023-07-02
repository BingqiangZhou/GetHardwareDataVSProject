
#include "GPSInfoProcesser.h"
#include <algorithm>
#include <string>


GPGGA_GPSInfoProcesser::GPGGA_GPSInfoProcesser() {

#pragma region ��ʼ����������Ҫ�õ��ı�������������ͷ���ֶ������ַ��������м�����

	GPSInfoName = "GPGGA";
	nDelim = 14;
	delim = ",";
	invalidDataValue = "-999999.000000";

#pragma endregion
}

// �������б��н������ݣ�����GPS�������ͽ���Ҫ�����ֶα����Լ���д���麯�����ɣ�
int GPGGA_GPSInfoProcesser::ParserStrListToDataStruct(std::vector<std::string> &strList) {

#pragma region ���ø��ṹ�壬��ȡ���������б��еĿ�ʼλ��

	int i = GPSInfoProcesser::ParserStrListToDataStruct(strList);
	if (i < 0)
	{
		return i;
	}

#pragma endregion

#pragma region �������õ������ݶ�Ӧ���ֶ�

	gpgga.strUTCTime = std::atof(strList[1].c_str());
	gpgga.fLatitude = std::atof(strList[2].c_str());
	gpgga.strLatitudeFlag = strList[3];
	gpgga.fLongitude = std::atof(strList[4].c_str());
	gpgga.strLongitudeFlag = strList[5];
	gpgga.nGPSState = std::atoi(strList[6].c_str());
	gpgga.nSatellitesNum = std::atoi(strList[7].c_str());
	gpgga.fHDOP = std::atof(strList[8].c_str());
	gpgga.fAltitude = std::atof(strList[9].c_str());
	// ����һ��'M'�ַ������ǿ��ַ�
	gpgga.fGeoid = std::atof(strList[11].c_str());
	// ����һ��'M'�ַ������ǿ��ַ�
	gpgga.fDifferentialTime = std::atof(strList[13].c_str());
	if (strList[14].length() > 1)
	{
		int nIndex = strList[14].find("*");
		std::string p = strList[14].substr(0, std::max(nIndex, 0)); // �Ǻ�֮ǰ��ֵ�����硰*4E�������ǲ�ֶ�λΪ��;

		// �����ֵ֤Ϊ�գ���˵���Ǻ�֮ǰû���ַ������硰*4E��
		// ��ʱ���ǲ�ֶ�λ��strDifferentialTimeҲӦ��Ϊ�գ���strDifferentialStationIdҲ��Ϊ��
		// ����ֵ֤����Ϊ �Ǻ�֮���ֵ

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