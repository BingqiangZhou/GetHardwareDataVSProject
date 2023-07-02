#pragma once

#include <vector>
#include <string.h>

#include "GPSInfoDataStructure.h"

// GPS���ݴ�������
class GPSInfoProcesser {

public:
#pragma region ����GPS���ݽ�������Ҫʹ�õ��ı���

	char* GPSInfoName; // ��Ҫ������GPS��������Ҳ������ͷ��,����һ��GPS�����е�GPGGA��GPGSA��GPGSV��GPRMC��GPVTG
					   //	$GPGGA, , , , , , 0, 00, 25.5, , , , , , *7A
					   //	$GPGSA, A, 1, , , , , , , , , , , , , 25.5, 25.5, 25.5, 1 * 1F
					   //	$GPGSV, 1, 1, 00, 0 * 65
					   //	$GPRMC, , V, , , , , , , , , , M, V * 2A
					   //	$GPVTG, , , , , , , , , M * 33
	std::vector<std::string> strList; // �����Ľ���б�
	std::vector<std::string> strFieldList; // �����ֶε��б�
	char* delim; // GPS�������õ��ļ����
	int nDelim; // GPS�������ж��ٸ�������������жϽ����Ƿ���ȷ
	std::string invalidDataValue; // ����������Чֵ�����ֵ

#pragma endregion

	GPSInfoProcesser();

#pragma region ��������ط���

	int Decode(char* strLine);
	void SplitLineStrToStrList(char* pLineStr, char* delim, std::vector<std::string> &strList);
	virtual int ParserStrListToDataStruct(std::vector<std::string> &strList);

#pragma endregion

#pragma region ��������ط���

	int Encode(std::vector<int> IndexList, char* delim, char* strEncoded);
	virtual void PushDataStructToStrFieldList();
	int EncodeStrFieldListToStr(std::vector<int> IndexList, char* delim, char* strEncoded);

#pragma endregion
};


// $GPGGA�����ID�����������ΪGlobal Positioning System Fix Data��GGA��GPS��λ��Ϣ
class GPGGA_GPSInfoProcesser : public GPSInfoProcesser {

public:
#pragma region ����GPGGA���ݵ��ֶ�

	GPGGA_GPSInfo gpgga;

#pragma endregion

	GPGGA_GPSInfoProcesser();

	// �������б��н������ݣ�����GPS�������ͽ���Ҫ�����ֶα����Լ���д���麯�����ɣ�
	virtual int ParserStrListToDataStruct(std::vector<std::string> &strList);
	virtual void PushDataStructToStrFieldList();

};