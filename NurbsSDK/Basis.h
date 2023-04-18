#pragma once
#include<vector>
class Basis {

public:
	Basis() { }

/// \brief 构造单变量NURBS基函数
/// \param degree 次数.
/// \param knots 节点序列.
/// \param weights 每个控制点的权重，必须大于0.
	Basis(int degree, const std::vector<double>& knots);

	/// \brief 确定参数u所在的节点间隔的索引
	/// \param u 参数点
	int findSpan(double u) const;

	/// \brief 计算参数点u的所有非零基函数值
	/// \param i 参数u所在的节点间隔的索引
	/// \param u 参数点
	/// \return N 非零基函数值
	void getAllNurbsBasisFuns(int i, double u, std::vector<double>& N) const;
	
	/// \brief 计算参数点u的所有非零基函数值和一阶导数值
	/// \param i 参数u所在的节点间隔的索引
	/// \param u 参数点
	/// \param nd 求导次数
	/// \return N 非零基函数nd次导数值
	void getAllNurbsBasisFunsDers(int i, double u, int nd, std::vector<std::vector<double>>& ders) const;

	int getControlSize() const { return m_Knots.size() - m_Degree - 1; }
	int getKnotSize() const { return m_Knots.size(); }
	int getDegree() const { return m_Degree; }
	
	const std::vector<double>& getKnots() { return m_Knots; }

private:
	int m_Degree;					// 次数
	std::vector<double> m_Knots;  // 节点序列

	
};