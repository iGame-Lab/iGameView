#include "Curve.h"


Curve::Curve(const int degree, const std::vector<Point>& controlPoints, const std::vector<double>& knots, const std::vector<double>& weights)
{
	m_ControlPoints = controlPoints;
	m_Basis = {Basis(degree, knots)};
	m_Weights = weights;
	type = Gtype::Curve;
}

Point Curve::getPointAtParam(std::vector<double>& u)
{
	std::vector<double> basisValue;
	std::vector<int> index;
	eval(u, basisValue);
	getConnectIndex(u, index);

	Point point{ 0,0,0 };
	for (int i = 0; i < basisValue.size(); ++i)
	{
		point[0] += basisValue[i] * m_ControlPoints[index[i]][0];
		point[1] += basisValue[i] * m_ControlPoints[index[i]][1];
		point[2] += basisValue[i] * m_ControlPoints[index[i]][2];
	}
	return point;
}

void Curve::getConnectIndex(const std::vector<double>& u, std::vector<int>& index)
{
	int p = m_Basis[0].getDegree();
	index.resize(p + 1);
	int uspan = m_Basis[0].findSpan(u[0]);
	for (int i = 0; i <= p; i++)
	{
		index[i] = uspan - p + i;
	}
}

bool Curve::getPointAtParam(std::vector<std::vector<double>>& u, std::vector<Point>& points)
{
	points.resize(u.size());
	for (int i = 0; i < points.size(); ++i)
	{
		points[i] = getPointAtParam(u[i]);
	}
	return true;
}

void Curve::eval(std::vector<double>& u, std::vector<double>& basisValue)
{
	double tol = 1e-15;
	int p = m_Basis[0].getDegree();
	int uPt = m_Basis[0].getKnotSize() - 1 - p - 1;
	int uspan = m_Basis[0].findSpan(u[0]);
	auto weight = m_Weights;
	auto u_knots = m_Basis[0].getKnots();
	std::vector <double> Nu(p + 1);

	if (fabs(u[0] - u_knots.back()) < tol)
		u[0] = u_knots.back() - tol;
	m_Basis[0].getAllNurbsBasisFuns(uspan, u[0], Nu);

	int i, k;
	double w = 0.0;

	basisValue.resize(p + 1);

	for (i = 0; i <= p; i++)
	{
		k = uspan - p + i;
		w += Nu[i] * weight[k];
	}
	for (i = 0; i <= p; i++)
	{
		k = uspan - p + i;
		basisValue[i] = Nu[i] * weight[k] / w;
	}
}

void Curve::evalDers(std::vector<double>& u, std::vector<std::vector<double>>& basisValue)
{
	double tol = 1e-15;
	int p = m_Basis[0].getDegree();
	int uPt = m_Basis[0].getKnotSize() - 1 - p - 1;
	int uspan = m_Basis[0].findSpan(u[0]);
	auto weight = m_Weights;
	auto  u_knots = m_Basis[0].getKnots();
	std::vector <double> Nu(p + 1);
	std::vector<std::vector<double>> Nu_ders(uPt + 1, std::vector<double>(p + 1));

	if (fabs(u[0] - u_knots[uPt - 1]) < tol)
		u[0] = u_knots.back() - tol;
	basisValue.resize(2);
	basisValue[0].resize(p + 1);
	basisValue[1].resize(p + 1);

	m_Basis[0].getAllNurbsBasisFuns(uspan, u[0], Nu);
	m_Basis[0].getAllNurbsBasisFunsDers(uspan, u[0], uPt, Nu_ders);

	int uind;
	double w = 0.0, fac, dNdxi = 0.0;
	for (int i = 0; i <= p; i++)
	{
		uind = uspan - p + i;
		w += Nu[i] * weight[uind];
		dNdxi += Nu_ders[1][i] * weight[uind];
	}
	for (int i = 0; i <= p; i++)
	{
		uind = uspan - p + i;
		basisValue[0][i] = Nu[i] * weight[uind] / w;
		fac = weight[i] / (w * w);
		basisValue[1][i] = (Nu_ders[1][i] * w - Nu[i] * dNdxi) * fac;
	}
}
