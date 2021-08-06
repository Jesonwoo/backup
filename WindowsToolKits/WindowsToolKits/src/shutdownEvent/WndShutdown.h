#pragma once
#include<Windows.h>
#include "common/typedefs.h"
/*
    �����ڴ��ںͿ���̨�Ĺػ��¼�������
    ˼��:ͨ������һ�����ش��ڣ������ش��ڵ���Ϣ�ص������м����ػ��¼���������CleanFunc(args)
    tip:��ͨ�û�Ȩ�޼��ɣ�������̳���5s�����Window�ػ���������ʾ: xxx������ֹ�ػ�
*/
extern "C" {
    typedef void(_stdcall* CleanFunc)(void* args);
    bool CORE_EXPORT SetShutdownProc(CleanFunc func, void* args);
    
}


/*
    ���÷���Ĺػ��¼�������
    ˼�룺�ڷ���Ŀ����������м����ػ��¼�.
    tip:һ���������3����ʱ�䴦��
*/
extern "C" {
    bool CORE_EXPORT SetServiceShutdownProc(LPCWSTR ServiceName, CleanFunc func, void* args);
}