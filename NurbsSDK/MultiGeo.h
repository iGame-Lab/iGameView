#pragma once
#include"Curve.h"
#include"Surface.h"
#include"Volume.h"
#include"string"
#include<memory>
using Geometry = std::shared_ptr<Geo>;

class MultiGeo {

public:
	
	MultiGeo(){}

	MultiGeo(std::vector<Geometry>& geometry) { m_Geometry = geometry; }

	void addPatch(Geometry& geo) { m_Geometry.push_back(geo); }

	Geo& patch(unsigned int i) { return *m_Geometry[i];}

	Geometry PatchPointer(unsigned int i) { return m_Geometry[i]; }

	void setBoundaryInfo(const std::vector<std::array<int, 2>>& boundary) { m_Boundary = boundary; }

	std::vector<std::array<int, 2>>& getBoundaryInfo() { return m_Boundary; }

	int getPatchSize() { return m_Geometry.size(); }

	~MultiGeo();
public:

	std::vector<Geometry> m_Geometry;

	// 边界信息  {片号，边界号} 
	// 曲线   0：参数点u = 0的边界   1：参数点u = 1的边界
	// 曲面   0：参数点u = 0的边界   1：参数点u = 1的边界   2：参数点v = 0的边界    3：参数点v = 1的边界
	// 曲体   0：参数点u = 0的边界   1：参数点u = 1的边界   2：参数点v = 0的边界    3：参数点v = 1的边界    4：参数点w = 0的边界    5：参数点w = 1的边界
	std::vector<std::array<int, 2>> m_Boundary;
};