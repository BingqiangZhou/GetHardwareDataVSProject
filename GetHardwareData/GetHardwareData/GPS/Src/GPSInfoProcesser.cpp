
#include "GPSInfoProcesser.h"

GPSInfoProcesser::GPSInfoProcesser() {
	delim = ",";
	GPSInfoName = "";
	nDelim = 0;
	invalidDataValue = "";
}

#pragma region ��������������Parser

// ���ַ����н�������
int GPSInfoProcesser::Decode(char* strLine) {
	SplitLineStrToStrList(strLine, delim, strList); // �������ݵ�strList
	return ParserStrListToDataStruct(strList); // �����ݶ�Ӧ���ֶα���
}

#pragma endregion

#pragma region �ַ����ָ��SplitLine
// ���ַ���pLine���ַ�delim���зָ����strList��
void GPSInfoProcesser::SplitLineStrToStrList(char* pLineStr, char* delim, std::vector<std::string> &strList) {
	if (strList.size() > 0)
	{
		strList.clear();
	}
	// �����ݳ��ֿ�ֵ��ʱ����ȡ�᲻׼ȷ
	/*char* p;
	p = strtok(pLine, delim);
	while (p) {
	strList.push_back(p);
	p = strtok(NULL, delim);
	}*/
	char* pStart = pLineStr;
	int nLen = strlen(pLineStr);
	for (size_t i = 0; i < nLen; i++)
	{
		if (pLineStr[i] == delim[0])
		{
			pLineStr[i] = '\0'; // ����,����Ϊ'\0',����Ǵ˴�Ϊһ���ַ����Ľ�β
			if (strlen(pStart) == 0)
			{
				strList.push_back(invalidDataValue);
			}
			else
			{
				strList.push_back(pStart);
			}
			pStart = pLineStr + (i + 1);
		}
	}
	strList.push_back(pStart);
}
#pragma endregion

#pragma region �����ݶ�Ӧ���ֶα�������ParserStrList

// �������б��н������ݣ�����GPS�������ͽ���Ҫ�����ֶα����Լ���д���麯�����ɣ�
int GPSInfoProcesser::ParserStrListToDataStruct(std::vector<std::string> &strList) {
	if (strList.size() == 0)
	{
		return -1;
	}
	// û�а�����֤ͷ����Ϊ���ݲ���ȷ��ֱ�ӷ���-1 
	if (-1 == strList[0].find(GPSInfoName))
	{
		strList.clear();
		return -1;
	}
	if (strList.size() != nDelim + 1)
	{
		//std::cout << "Data Format Error" << std::endl;
		strList.clear();
		return -1;
	}
	return 0;
};

#pragma endregion

#pragma region ��һ����ʽ���������ݵı��뺯��Encoder

// ��һ���ĸ�ʽ����Ҫ���õ������ݱ��뵽�ַ�����
int GPSInfoProcesser::Encode(std::vector<int> IndexList, char* delim, char* strEncoded) {
	// IndexList�б����±��Ӧ����������Ҫ�õ������ݣ�delim������֮��ļ������strEncoded��������ַ���
	if (strList.size() <= 0){
		return -1;
	}
	PushDataStructToStrFieldList();
	return EncodeStrFieldListToStr(IndexList, delim, strEncoded);
}

#pragma endregion

void GPSInfoProcesser::PushDataStructToStrFieldList() {
	strFieldList.clear();
	strFieldList = strList;
}

int GPSInfoProcesser::EncodeStrFieldListToStr(std::vector<int> IndexList, char* delim, char* strEncoded) {
	// ����±��Ƿ񳬹�vector����
	for (size_t i = 0; i < IndexList.size(); i++)
	{
		if (IndexList[i] >= strFieldList.size())
		{
			return -1;
		}
	}
	if (IndexList.size() > 0)
	{
		// ��IndexList�б�sizeΪ0ʱ��������IndexList�б����±��Ӧ������
		for (size_t i = 0; i < IndexList.size(); i++) {
			if (strFieldList[IndexList[i]].length() == 0)
			{
				strcat(strEncoded, "NULL");
			}
			else {
				strcat(strEncoded, strFieldList[IndexList[i]].c_str());
			}
			strcat(strEncoded, delim);
		}
		strcat(strEncoded, "\n");
	}
	else {
		// ��IndexList�б�sizeΪ0ʱ��������������ȫ����Ϣ
		for (size_t i = 0; i < strFieldList.size(); i++) {
			if (strFieldList[i].length() == 0)
			{
				strcat(strEncoded, "NULL");
			}
			else {
				strcat(strEncoded, strFieldList[i].c_str());
			}
			strcat(strEncoded, delim);
		}
		strcat(strEncoded, "\n");
	}
	return 0;
}