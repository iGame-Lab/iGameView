#pragma once
#include<vector>
class Basis {

public:
	Basis() { }

/// \brief ���쵥����NURBS������
/// \param degree ����.
/// \param knots �ڵ�����.
/// \param weights ÿ�����Ƶ��Ȩ�أ��������0.
	Basis(int degree, const std::vector<double>& knots);

	/// \brief ȷ������u���ڵĽڵ���������
	/// \param u ������
	int findSpan(double u) const;

	/// \brief ���������u�����з��������ֵ
	/// \param i ����u���ڵĽڵ���������
	/// \param u ������
	/// \return N ���������ֵ
	void getAllNurbsBasisFuns(int i, double u, std::vector<double>& N) const;
	
	/// \brief ���������u�����з��������ֵ��һ�׵���ֵ
	/// \param i ����u���ڵĽڵ���������
	/// \param u ������
	/// \param nd �󵼴���
	/// \return N ���������nd�ε���ֵ
	void getAllNurbsBasisFunsDers(int i, double u, int nd, std::vector<std::vector<double>>& ders) const;

	int getControlSize() const { return m_Knots.size() - m_Degree - 1; }
	int getKnotSize() const { return m_Knots.size(); }
	int getDegree() const { return m_Degree; }
	
	const std::vector<double>& getKnots() { return m_Knots; }

private:
	int m_Degree;					// ����
	std::vector<double> m_Knots;  // �ڵ�����

	
};