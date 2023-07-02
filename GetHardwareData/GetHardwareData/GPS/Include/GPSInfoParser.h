#pragma once

#include <vector>
#include <string.h>

// GPS���ݽ������ṹ��
struct GPSInfoParser {

#pragma region ����GPS���ݽ�������Ҫʹ�õ��ı���

	char* GPSInfoName; // ��Ҫ������GPS��������Ҳ������ͷ��,����һ��GPS�����е�GPGGA��GPGSA��GPGSV��GPRMC��GPVTG
	//	$GPGGA, , , , , , 0, 00, 25.5, , , , , , *7A
	//	$GPGSA, A, 1, , , , , , , , , , , , , 25.5, 25.5, 25.5, 1 * 1F
	//	$GPGSV, 1, 1, 00, 0 * 65
	//	$GPRMC, , V, , , , , , , , , , M, V * 2A
	//	$GPVTG, , , , , , , , , M * 33
	int nFields; // GPS�������ж��ٸ��ֶΣ������жϽ����Ƿ���ȷ
	std::vector<char*> strList; // �����Ľ���б�
	char* delim; // GPS�������õ��ļ����

#pragma endregion

	GPSInfoParser() {
		delim = ",";
		GPSInfoName = "";
		nFields = 0;
	}

#pragma region ��������������Parser

	// ���ַ����н�������
	int Parser(char* strLine) {
		SplitLine(strLine, delim, strList); // �������ݵ�strList
		return ParserStrList(strList); // �����ݶ�Ӧ���ֶα���
	}

#pragma endregion

#pragma region ��һ����ʽ���������ݵı��뺯��Encoder

	// ��һ���ĸ�ʽ����Ҫ���õ������ݱ��뵽�ַ�����
	int Encoder(std::vector<int> IndexList, char* delim, char* strEncoded) {
		// IndexList�б����±��Ӧ����������Ҫ�õ������ݣ�delim������֮��ļ������strEncoded��������ַ���

		// ����±��Ƿ񳬹�vector����
		for (size_t i = 0; i < IndexList.size(); i++)
		{
			if (IndexList[i] >= strList.size())
			{
				return -1;
			}
		}
		if (IndexList.size() > 0)
		{
			// ��IndexList�б�sizeΪ0ʱ��������IndexList�б����±��Ӧ������
			for (size_t i = 0; i < IndexList.size(); i++) {
				strcat(strEncoded, strList[IndexList[i]]);
				strcat(strEncoded, delim);
			}
			strcat(strEncoded, "\n");
		}
		else {
			// ��IndexList�б�sizeΪ0ʱ��������������ȫ����Ϣ
			for (size_t i = 0; i < strList.size(); i++) {
				strcat(strEncoded, strList[i]);
				strcat(strEncoded, delim);
			}
			strcat(strEncoded, "\n");
		}
		return 0;
	}

#pragma endregion

#pragma region �ַ����ָ��SplitLine
	// ���ַ���pLine���ַ�delim���зָ����strList��
	void SplitLine(char* pLine, char* delim, std::vector<char*> &strList) {
		if (strList.size() > 0)
		{
			strList.clear();
		}
		// �����ݲ����ֿ�ֵ��ʱ����ȡ�᲻׼ȷ
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
				pLine[i] = '\0'; // ����,����Ϊ'\0',����Ǵ˴�Ϊһ���ַ����Ľ�β
				strList.push_back(pStart);
				pStart = pLine + (i + 1);
				if (pStart[0] == delim[0]) // ����������","����Ҫ����һ��
				{
					pStart++;
				}
			}
		}
	}
#pragma endregion

#pragma region �����ݶ�Ӧ���ֶα�������ParserStrList

	// �������б��н������ݣ�����GPS�������ͽ���Ҫ�����ֶα����Լ���д���麯�����ɣ�
	virtual int ParserStrList(std::vector<char*> &strList) {
		if (strList.size() == 0)
		{
			return -1;
		}
		int i = 0;
		// �����Ƿ��������ͷ��������ڣ���sizeӦ�ü�1(nFields + i)������"GPGGA"��Ϣԭ�����ֶ���13�������Ǵ�������ͷ��size+1��Ϊ14 
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

// $GPGGA�����ID�����������ΪGlobal Positioning System Fix Data��GGA��GPS��λ��Ϣ
struct GPGGA_GPSInfoParser : GPSInfoParser {

#pragma region ����GPGGA���ݵ��ֶ�

	char* strUTCTime; // �ֶ�1��UTC ʱ�䣬hhmmss.sss��ʱ�����ʽ
	char* strLatitude; // �ֶ�2��γ��ddmm.mmmm���ȷָ�ʽ��ǰ��λ��������0��
	char* strLatitudeFlag; // �ֶ�3��γ��N����γ����S����γ��
	char* strLongitude; // �ֶ�4������dddmm.mmmm���ȷָ�ʽ��ǰ��λ��������0��
	char* strLongitudeFlag; // �ֶ�5������E����������W��������
	char* strGPSState;// �ֶ�6��GPS״̬��0 = δ��λ��1 = �ǲ�ֶ�λ��2 = ��ֶ�λ��3 = ��ЧPPS��6 = ���ڹ���
	char* strSatellitesNum;// �ֶ�7������ʹ�õ�����������00 - 12����ǰ��λ��������0��
	char* strHDOP;// �ֶ�8��HDOPˮƽ�������ӣ�0.5 - 99.9��
	char* strAltitude;// �ֶ�9�����θ߶ȣ� - 9999.9 - 99999.9��
	char* strGeoid;// �ֶ�10��������������Դ��ˮ׼��ĸ߶�
	char* strDifferentialTime;// �ֶ�11�����ʱ�䣨�����һ�ν��յ�����źſ�ʼ��������������ǲ�ֶ�λ��Ϊ�գ�
	char* strDifferentialStationId;// �ֶ�12�����վID��0000 - 1023��ǰ��λ��������0��������ǲ�ֶ�λ��Ϊ�գ�
	char* strVerificationValue;// �ֶ�13��У��ֵ

#pragma endregion

	GPGGA_GPSInfoParser() {

#pragma region ��ʼ��GPGGA���ݵ��ֶ�

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

#pragma region ��ʼ����������Ҫ�õ��ı�������������ͷ���ֶ������ַ��������м�����

		this->GPSInfoName = "GPGGA";
		this->nFields = 13;
		this->delim = ",";

#pragma endregion
	}

	// �������б��н������ݣ�����GPS�������ͽ���Ҫ�����ֶα����Լ���д���麯�����ɣ�
	virtual int ParserStrList(std::vector<char*> &strList) {

#pragma region ���ø��ṹ�壬��ȡ���������б��еĿ�ʼλ��

		int i = GPSInfoParser::ParserStrList(strList);
		if (i < 0)
		{
			return i;
		}

#pragma endregion

#pragma region �������õ������ݶ�Ӧ���ֶ�

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