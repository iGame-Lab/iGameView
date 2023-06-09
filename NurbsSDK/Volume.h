#pragma once
#include"Geo.h"
class Volume : public Geo {

public:
	/// \brief 构造一条有理b样条曲线.
	/// \param degree 次数.
	/// \param controlPoints 控制点.
	/// \param knots 节点序列.
	/// \param weights 每个控制点的权重，必须大于0.
	Volume(const int udegree, const int vdegree, const int wdegree, const std::vector<Point>& controlPoints,
		const std::vector<double>& uknots, const std::vector<double>& vknots, const std::vector<double>& wknots, const std::vector<double>& weights);

	/// \brief 获得体某一参数点的物理坐标，u长度为3
	Point getPointAtParam(std::vector<double>& u);

	/// \brief 获得体某一参数点的非零基函数索引，u长度为3
	void getConnectIndex(const std::vector<double>& u, std::vector<int>& index);

	/// \brief 获得体多个参数点的非零基函数索引，u长度为参数点个数 x 3
	bool getPointAtParam(std::vector<std::vector<double>>& u, std::vector<Point>& points);

	/// \brief 获得非零基函数值，u长度3
	void eval(std::vector<double>& u, std::vector<double>& basisValue);

	/// \brief 获得非零基函数值和导数，u长度3
	void evalDers(std::vector<double>& u, std::vector<std::vector<double>>& basisValue);




};