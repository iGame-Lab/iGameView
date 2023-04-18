#pragma once
#include"MultiGeo.h"
//��������Ŀ��Ƶ������Զ�����NURBS����ģ��

class AutoGeoCreate {

public:

	// ��������
	static bool CreateModel(MultiGeo& geo, int uCptNum, int uDegreeNum);


	// ��������
	static bool CreateModel(MultiGeo& geo, int uCptNum, int vCptNum, int uDegreeNum, int vDegreeNum );

	// ��������
	static bool CreateModel(MultiGeo& geo, int uCptNum, int vCptNum, int wCptNum, int uDegreeNum, int vDegreeNum, int wDegreeNum);
};

