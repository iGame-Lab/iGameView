#pragma once
#include<array>
#include<vector>
#include"Basis.h"

// ǰ����Ϊ����    �����������������䣬�磺λ�ƣ�����  ���ڻ���
using Point = std::vector<double>;
class Geo {

public:


	Geo(){}

	//����ĳ������������������������
	//u  ������  
	virtual Point getPointAtParam(std::vector<double>& u) = 0;

	//���������ķ����������������
	//u  ������  
	//index  ����������  
	virtual void getConnectIndex(const std::vector<double>& u, std::vector<int>& index) = 0;

	//����һ��������������һ�����������
	//u  һ������� 
	//points  һ�������  
	virtual bool getPointAtParam(std::vector<std::vector<double>>& u, std::vector<Point>& points) = 0;

	//����ĳ�����������������������
	//u  ������
	//basisValue �������������
	virtual void eval(std::vector<double>& u, std::vector<double>& basisValue) = 0;

	//����ĳ�������������������������һ�׵���
	//u  ������
	//basisValue[0] �������������  {N_{i},N_{i+1},...,N_{i+p}}
	//basisValue[1] һ�׵��� {dN_{i}dxi, dN_{i}deta, dN_{i}dzeta, ....,dN_{i+p}dxi, dN_{i+p}deta, dN_{i+p}dzeta }
	virtual void evalDers(std::vector<double>& u, std::vector<std::vector<double>>& basisValue) = 0;

	enum Gtype
	{
		Curve, Surface, Volume
	};


	virtual ~Geo() { m_ControlPoints.clear(); m_Basis.clear(); m_Weights.clear(); }

	Gtype type;
public:
	std::vector<Point> m_ControlPoints;
	std::vector<Basis> m_Basis;   // ������
	std::vector<double> m_Weights; // Ȩֵ
};