#pragma once
#include<array>
#include<vector>
#include"Basis.h"

// 前三项为坐标    其它项则根据需求填充，如：位移，误差等  便于绘制
using Point = std::vector<double>;
class Geo {

public:


	Geo(){}

	//根据某个参数点坐标计算物理坐标点
	//u  参数点  
	virtual Point getPointAtParam(std::vector<double>& u) = 0;

	//计算参数点的非零基函数的索引号
	//u  参数点  
	//index  索引号序列  
	virtual void getConnectIndex(const std::vector<double>& u, std::vector<int>& index) = 0;

	//根据一组参数点坐标计算一组物理坐标点
	//u  一组参数点 
	//points  一组物理点  
	virtual bool getPointAtParam(std::vector<std::vector<double>>& u, std::vector<Point>& points) = 0;

	//根据某个参数点坐标计算非零基函数
	//u  参数点
	//basisValue 非零基函数序列
	virtual void eval(std::vector<double>& u, std::vector<double>& basisValue) = 0;

	//根据某个参数点坐标计算非零基函数和一阶导数
	//u  参数点
	//basisValue[0] 非零基函数序列  {N_{i},N_{i+1},...,N_{i+p}}
	//basisValue[1] 一阶导数 {dN_{i}dxi, dN_{i}deta, dN_{i}dzeta, ....,dN_{i+p}dxi, dN_{i+p}deta, dN_{i+p}dzeta }
	virtual void evalDers(std::vector<double>& u, std::vector<std::vector<double>>& basisValue) = 0;

	enum Gtype
	{
		Curve, Surface, Volume
	};


	virtual ~Geo() { m_ControlPoints.clear(); m_Basis.clear(); m_Weights.clear(); }

	Gtype type;
public:
	std::vector<Point> m_ControlPoints;
	std::vector<Basis> m_Basis;   // 基函数
	std::vector<double> m_Weights; // 权值
};