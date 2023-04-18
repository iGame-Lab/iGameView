#pragma once
#include"Geo.h"

class Surface : public Geo {

public:
	/// \brief ����һ������b��������.
	/// \param degree ����.
	/// \param controlPoints ���Ƶ�.
	/// \param knots �ڵ�����.
	/// \param weights ÿ�����Ƶ��Ȩ�أ��������0.
	Surface(const int udegree, const int vdegree, const std::vector<Point>& controlPoints,
		const std::vector<double>& uknots, const std::vector<double>& vknots, const std::vector<double>& weights);


	/// \brief �������ĳһ��������������꣬u����Ϊ2
	Point getPointAtParam(std::vector<double>& u);

	/// \brief �������ĳһ������ķ��������������u����Ϊ2
	void getConnectIndex(const std::vector<double>& u, std::vector<int>& index);

	/// \brief ���������������ķ��������������u����Ϊ��������� x 2
	bool getPointAtParam(std::vector<std::vector<double>>& u, std::vector<Point>& points);

	/// \brief ��÷��������ֵ��u����2
	void eval(std::vector<double>& u, std::vector<double>& basisValue);

	/// \brief ��÷��������ֵ�͵�����u����2
	void evalDers(std::vector<double>& u, std::vector<std::vector<double>>& basisValue);

};