#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include "IMVApi.h"

#include <direct.h>
#include <iostream>
#include <thread>

#include <opencv2/opencv.hpp>

// ��ȡ���ͼ��ʽ
enum GetCameraFrameMethod { 
	GET_SINGAL_FRAME_RECURRENT,  // ѭ����ȡ��֡�����ýӿ�IMV_GetFrame
	GRAB_CALLBACK, // �ص�������ȡ֡
	FRAME_SOFT_TRIGGER  // ��������ȡ֡
};

// ������ݴ�����
class CameraProcesser
{
private:
	int ret;
	unsigned int cameraIndex; // ���id,����Ƕ�����������Ҫ�ֶ�ѡ��
	//int64_t nImgNo;
	IMV_HANDLE devHandle;	// ������
	bool bExitThreadFlag;	// �˳���ȡͼ���̱߳��
	bool bExitedThread;		// ��ȡͼ���߳��Ƿ��Ѿ��������
	char outDir[256];		// ����ļ���outDir�ļ���
	int64_t width;			// ͼ�����
	int64_t height;			// ͼ�����
	cv::VideoWriter videoWriter;	// ͼ�񱣴�Ϊavi��ʽ��Ƶ,��Ӧ��VideoWriter����
	GetCameraFrameMethod getCameraFrameMethod; // ��ȡͼ��֡�ķ�ʽ��������ѭ����ȡ��֡GET_SINGAL_FRAME_RECURRENT���ص�������ȡ֡GRAB_CALLBACK�� ��������ȡ֡FRAME_SOFT_TRIGGER

public:
	CameraProcesser();
	~CameraProcesser();

	int InitCameraSetting(char* strTime, GetCameraFrameMethod getCameraFrameMethod); // ��ʼ���������
	int StartGrabbing(); // ��ʼץͼ
	int StopGrabingAndReleaseSource(); // ����ץͼ �� �ͷ���Դ

	friend int GetFrameForGetSingalFrameRecurrent(char* strTime);// ѭ����ȡ��֡
	friend int GetSingalFrameRecurrentThread(CameraProcesser* cameraProcesser); // ѭ����ȡ��֡�߳�

	friend int GetFrameForGrabCallback(char* strTime); // �ص�������ȡ֡
	//friend void GetFrameForGrabCallbackThread(IMV_Frame* pFrame, void* pUser); // �ص�������ȡ֡�Ļص�����

	friend int GetFrameForFrameSoftTrigger(char* strTime); // ��������ȡ֡
	friend int ExecuteSoftTriggerThread(CameraProcesser* cameraProcesser);  // ѭ��ִ���������߳�
	//friend int GetFrameForFrameSoftTriggerThread(IMV_Frame* pFrame, void* pUser); // ��GetFrameForGrabCallbackThread����һ��

	friend void GrabCallbackThread(IMV_Frame* pFrame, void* pUser); // GetFrameForGrabCallbackThread��GetFrameForGrabCallbackThread����һ�£�ͳһΪͬһ��

private:

	int GetCameraId(); // ��ȡ���id������Ƕ�����������Ҫ�ֶ�ѡ��
	int SetCameraConfig(); // ����������ã������ع⣬��ƽ�⣬ͼ���ʽ������ģʽ��
	int GetFrameHeightWidth(); // ��ȡͼ��֡�߿�

};