#pragma once

#include <string>

// $GPGGA�����ID�����������ΪGlobal Positioning System Fix Data��GGA��GPS��λ��Ϣ
struct GPGGA_GPSInfo {

#pragma region ����GPGGA���ݵ��ֶ�

	float strUTCTime; // �ֶ�1��UTC ʱ�䣬hhmmss.sss��ʱ�����ʽ
	float fLatitude; // �ֶ�2��γ��ddmm.mmmm���ȷָ�ʽ��ǰ��λ��������0��
	std::string strLatitudeFlag; // �ֶ�3��γ��N����γ����S����γ��
	float fLongitude; // �ֶ�4������dddmm.mmmm���ȷָ�ʽ��ǰ��λ��������0��
	std::string strLongitudeFlag; // �ֶ�5������E����������W��������
	int nGPSState;// �ֶ�6��GPS״̬��0 = δ��λ��1 = �ǲ�ֶ�λ��2 = ��ֶ�λ��3 = ��ЧPPS��6 = ���ڹ���
	int nSatellitesNum;// �ֶ�7������ʹ�õ�����������00 - 12����ǰ��λ��������0��
	float fHDOP;// �ֶ�8��HDOPˮƽ�������ӣ�0.5 - 99.9��
	float fAltitude;// �ֶ�9�����θ߶ȣ� - 9999.9 - 99999.9��
	float fGeoid;// �ֶ�10��������������Դ��ˮ׼��ĸ߶�
	float fDifferentialTime;// �ֶ�11�����ʱ�䣨�����һ�ν��յ�����źſ�ʼ��������������ǲ�ֶ�λ��Ϊ�գ�
	int nDifferentialStationId;// �ֶ�12�����վID��0000 - 1023��ǰ��λ��������0��������ǲ�ֶ�λ��Ϊ�գ�
	std::string strVerificationValue;// �ֶ�13��У��ֵ

#pragma endregion
	GPGGA_GPSInfo() {	
#pragma region ��ʼ��GPGGA���ݵ��ֶ�

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