
#include "GPSInfoProcesser.h"

GPSInfoProcesser::GPSInfoProcesser() {
	delim = ",";
	GPSInfoName = "";
	nDelim = 0;
	invalidDataValue = "";
}

#pragma region 解析器解析函数Parser

// 从字符串中解析数据
int GPSInfoProcesser::Decode(char* strLine) {
	SplitLineStrToStrList(strLine, delim, strList); // 解析数据到strList
	return ParserStrListToDataStruct(strList); // 将数据对应到字段变量
}

#pragma endregion

#pragma region 字符串分割函数SplitLine
// 将字符串pLine按字符delim进行分割，存入strList中
void GPSInfoProcesser::SplitLineStrToStrList(char* pLineStr, char* delim, std::vector<std::string> &strList) {
	if (strList.size() > 0)
	{
		strList.clear();
	}
	// 当数据出现空值的时候，提取会不准确
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
			pLineStr[i] = '\0'; // 将“,”改为'\0',即标记此处为一个字符串的结尾
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

#pragma region 将数据对应到字段变量函数ParserStrList

// 从数据列表中解析数据（其他GPS数据类型仅需要定义字段变量以及重写该虚函数即可）
int GPSInfoProcesser::ParserStrListToDataStruct(std::vector<std::string> &strList) {
	if (strList.size() == 0)
	{
		return -1;
	}
	// 没有包含验证头，认为数据不正确，直接返回-1 
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

#pragma region 以一定格式输出相关数据的编码函数Encoder

// 以一定的格式将需要的用到的数据编码到字符串中
int GPSInfoProcesser::Encode(std::vector<int> IndexList, char* delim, char* strEncoded) {
	// IndexList列表中下标对应的数据是需要用到的数据，delim是数据之间的间隔符，strEncoded是输出的字符串
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
	// 检测下标是否超过vector长度
	for (size_t i = 0; i < IndexList.size(); i++)
	{
		if (IndexList[i] >= strFieldList.size())
		{
			return -1;
		}
	}
	if (IndexList.size() > 0)
	{
		// 当IndexList列表size为0时，仅编码IndexList列表中下标对应的数据
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
		// 当IndexList列表size为0时，则编码解析到的全部信息
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