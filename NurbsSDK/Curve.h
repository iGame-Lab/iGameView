#pragma once
#include"Geo.h"
class Curve : public Geo {

public:
	/// \brief ����һ������b��������.
	/// \param degree ����.
	/// \param controlPoints ���Ƶ�.
	/// \param knots �ڵ�����.
	/// \param weights ÿ�����Ƶ��Ȩ�أ��������0.
	Curve(const int degree, const std::vector<Point>& controlPoints, const std::vector<double>& knots, const std::vector<double>& weights);

	/// \brief �������ĳһ��������������꣬u����Ϊ1
	Point getPointAtParam(std::vector<double>& u);

	/// \brief �������ĳһ������ķ��������������u����Ϊ1
	void getConnectIndex(const std::vector<double>& u, std::vector<int>& index);

	/// \brief ������߶��������ķ��������������u����Ϊ��������� x 1
	bool getPointAtParam(std::vector<std::vector<double>>& u, std::vector<Point>& points);

	/// \brief ��÷��������ֵ��u����1
	void eval(std::vector<double>& u, std::vector<double>& basisValue);

	/// \brief ��÷��������ֵ�͵�����u����1
	void evalDers(std::vector<double>& u, std::vector<std::vector<double>>& basisValue);

};