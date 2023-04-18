#pragma once
#include"MultiGeo.h"
//根据输入的控制点数量自动生成NURBS几何模型

class AutoGeoCreate {

public:

	// 构造曲线
	static bool CreateModel(MultiGeo& geo, int uCptNum, int uDegreeNum);


	// 构造曲面
	static bool CreateModel(MultiGeo& geo, int uCptNum, int vCptNum, int uDegreeNum, int vDegreeNum );

	// 构造曲体
	static bool CreateModel(MultiGeo& geo, int uCptNum, int vCptNum, int wCptNum, int uDegreeNum, int vDegreeNum, int wDegreeNum);
};

