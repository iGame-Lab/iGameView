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

	// �߽���Ϣ  {Ƭ�ţ��߽��} 
	// ����   0��������u = 0�ı߽�   1��������u = 1�ı߽�
	// ����   0��������u = 0�ı߽�   1��������u = 1�ı߽�   2��������v = 0�ı߽�    3��������v = 1�ı߽�
	// ����   0��������u = 0�ı߽�   1��������u = 1�ı߽�   2��������v = 0�ı߽�    3��������v = 1�ı߽�    4��������w = 0�ı߽�    5��������w = 1�ı߽�
	std::vector<std::array<int, 2>> m_Boundary;
};