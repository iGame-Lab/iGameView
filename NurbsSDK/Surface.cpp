#include "Surface.h"

Surface::Surface(const int udegree, const int vdegree, const std::vector<Point>& controlPoints, const std::vector<double>& uknots, const std::vector<double>& vknots, const std::vector<double>& weights)
{
	m_Basis.resize(2);
	m_ControlPoints = controlPoints;
	m_Weights = weights;
	m_Basis[0] = Basis(udegree, uknots);
	m_Basis[1] = Basis(vdegree, vknots);
	type = Gtype::Surface;
}

Point Surface::getPointAtParam(std::vector<double>& u)
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

void Surface::getConnectIndex(const std::vector<double>& u, std::vector<int>& index)
{
	int p = m_Basis[0].getDegree(), q = m_Basis[1].getDegree();
	index.resize((p + 1) * (q + 1));
	int uspan = m_Basis[0].findSpan(u[0]), vspan = m_Basis[1].findSpan(u[1]);

	for (int j = 0; j <= q; ++j)
	{
		for (int i = 0; i <= p; ++i)
		{
			index[j * (p + 1) + i] = (uspan - p + i) + (vspan - q + j) * (m_Basis[0].getKnotSize() - p - 1);
		}
	}
}

bool Surface::getPointAtParam(std::vector<std::vector<double>>& u, std::vector<Point>& points)
{
	points.resize(u.size());
	for (int i = 0; i < points.size(); ++i)
	{
		points[i] = getPointAtParam(u[i]);
	}
	return true;
}

void Surface::eval(std::vector<double>& u, std::vector<double>& basisValue)
{
	double tol = 1e-15;
	int p = m_Basis[0].getDegree(), q = m_Basis[1].getDegree();
	int uPt = m_Basis[0].getKnotSize() - 1 - p - 1, vPt = m_Basis[1].getKnotSize() - 1 - q - 1;
	int uspan = m_Basis[0].findSpan(u[0]), vspan = m_Basis[1].findSpan(u[1]);
	auto weight = m_Weights;
	
	auto uknots = m_Basis[0].getKnots(), vknots = m_Basis[1].getKnots();
	std::vector <double> Nu(p + 1), Nv(q + 1);

	if (fabs(u[0] - uknots.back()) < tol)
		u[0] = uknots.back() - tol;
	if (fabs(u[1] - vknots.back()) < tol)
		u[1] = vknots.back() - tol;
	m_Basis[0].getAllNurbsBasisFuns(uspan, u[0], Nu);
	m_Basis[1].getAllNurbsBasisFuns(vspan, u[1], Nv);

	double w = 0.0;
	int uind, vind, ind;
	for (int j = 0; j <= q; ++j)
	{
		vind = vspan - q + j;
		for (int i = 0; i <= p; ++i)
		{
			uind = uspan - p + i;
			ind = vind * (uPt + 1) + uind;
			w += Nu[i] * Nv[j] * weight[ind];
		}
	}

	basisValue.resize((p + 1) * (q + 1));
	int cnt = 0;
	for (int j = 0; j <= q; ++j)
	{
		vind = vspan - q + j;
		for (int i = 0; i <= p; ++i)
		{
			uind = uspan - p + i;
			ind = vind * (uPt + 1) + uind;
			basisValue[cnt++] = Nu[i] * Nv[j] * weight[ind] / w;
		}
	}
}

void Surface::evalDers(std::vector<double>& u, std::vector<std::vector<double>>& basisValue)
{
	double tol = 1e-15;
	int p = m_Basis[0].getDegree(), q = m_Basis[1].getDegree();
	int uPt = m_Basis[0].getKnotSize() - 1 - p - 1, vPt = m_Basis[1].getKnotSize() - 1 - q - 1;
	int uspan = m_Basis[0].findSpan(u[0]), vspan = m_Basis[1].findSpan(u[1]);
	auto weight = m_Weights;

	auto uknots = m_Basis[0].getKnots(), vknots = m_Basis[1].getKnots();
	std::vector <double> Nu(p + 1), Nv(q + 1);
	std::vector<std::vector<double>> Nu_ders(uPt + 1, std::vector<double>(p + 1)),
		Nv_ders(vPt + 1, std::vector<double>(q + 1));

	if (fabs(u[0] - uknots.back()) < tol)
		u[0] = uknots.back() - tol;
	if (fabs(u[1] - vknots.back()) < tol)
		u[1] = vknots.back() - tol;
	m_Basis[0].getAllNurbsBasisFuns(uspan, u[0], Nu);
	m_Basis[1].getAllNurbsBasisFuns(vspan, u[1], Nv);
	m_Basis[0].getAllNurbsBasisFunsDers(uspan, u[0], uPt, Nu_ders);
	m_Basis[1].getAllNurbsBasisFunsDers(vspan, u[1], vPt, Nv_ders);

	double w = 0.0, dNdxi = 0.0, dNdeta = 0.0;
	int uind, vind, ind;
	for (int j = 0; j <= q; ++j)
	{
		vind = vspan - q + j;
		for (int i = 0; i <= p; ++i)
		{
			uind = uspan - p + i;
			ind = vind * (uPt + 1) + uind;
			w += Nu[i] * Nv[j] * weight[ind];
			dNdxi += Nu_ders[1][i] * Nv[j] * weight[ind];
			dNdeta += Nu[i] * Nv_ders[1][j] * weight[ind];
		}
	}
	basisValue.resize(2);
	basisValue[0].resize((p + 1) * (q + 1));
	basisValue[1].resize(2 * (p + 1) * (q + 1));
	int cnt = 0;
	double fac;
	for (int j = 0; j <= q; ++j)
	{
		vind = vspan - q + j;
		for (int i = 0; i <= p; ++i)
		{
			uind = uspan - p + i;
			ind = vind * (uPt + 1) + uind;
			fac = weight[ind] / (w * w);

			basisValue[0][cnt] = Nu[i] * Nv[j] * weight[ind] / w;
			basisValue[1][2 * cnt] = (Nu_ders[1][i] * Nv[j] * w - Nu[i] * Nv[j] * dNdxi) * fac;
			basisValue[1][2 * cnt + 1] = (Nu[i] * Nv_ders[1][j] * w - Nu[i] * Nv[j] * dNdeta) * fac;
			cnt++;
		}
	}

}
