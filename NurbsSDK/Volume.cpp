#include "Volume.h"

Volume::Volume(const int udegree, const int vdegree, const int wdegree, const std::vector<Point>& controlPoints, const std::vector<double>& uknots, const std::vector<double>& vknots, const std::vector<double>& wknots, const std::vector<double>& weights)
{
	m_Basis.resize(3);
	m_ControlPoints = controlPoints;
	m_Weights = weights;
	m_Basis[0] = Basis(udegree, uknots);
	m_Basis[1] = Basis(vdegree, vknots);
	m_Basis[2] = Basis(wdegree, wknots);
	type = Gtype::Volume;
}

Point Volume::getPointAtParam(std::vector<double>& u)
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

void Volume::getConnectIndex(const std::vector<double>& u, std::vector<int>& index)
{
	int p = m_Basis[0].getDegree(), q = m_Basis[1].getDegree(), r = m_Basis[2].getDegree();
	index.resize((p + 1) * (q + 1)*(r+1));
	int uspan = m_Basis[0].findSpan(u[0]), vspan = m_Basis[1].findSpan(u[1]), wspan = m_Basis[2].findSpan(u[2]);

	for (int k = 0; k <= r; ++k) 
	{
		for (int j = 0; j <= q; ++j)
		{
			for (int i = 0; i <= p; ++i)
			{
				index[k * (p+1) * (q + 1) + j * (p + 1) + i] = 	
					(wspan - r + k) * (m_Basis[0].getKnotSize() - p - 1) * (m_Basis[1].getKnotSize() - q - 1)
					+ (vspan - q + j) * (m_Basis[0].getKnotSize() - p - 1) + (uspan - p + i);
			}
		}
	}
}

bool Volume::getPointAtParam(std::vector<std::vector<double>>& u, std::vector<Point>& points)
{
	points.resize(u.size());
	for (int i = 0; i < points.size(); ++i)
	{
		points[i] = getPointAtParam(u[i]);
	}
	return true;
}

void Volume::eval(std::vector<double>& u, std::vector<double>& basisValue)
{
	double tol = 1e-15;
	int p = m_Basis[0].getDegree(), q = m_Basis[1].getDegree(), r = m_Basis[2].getDegree();
	int uPt = m_Basis[0].getKnotSize() - 1 - p - 1, vPt = m_Basis[1].getKnotSize() - 1 - q - 1, wPt = m_Basis[2].getKnotSize() - 1 - r - 1;
	int uspan = m_Basis[0].findSpan(u[0]), vspan = m_Basis[1].findSpan(u[1]), wspan = m_Basis[2].findSpan(u[2]);
	auto weight = m_Weights;

	auto uknots = m_Basis[0].getKnots(), vknots = m_Basis[1].getKnots(), wknots = m_Basis[2].getKnots();
	std::vector <double> Nu(p + 1), Nv(q + 1), Nw(r + 1);

	if (fabs(u[0] - uknots.back()) < tol)
		u[0] = uknots.back() - tol;
	if (fabs(u[1] - vknots.back()) < tol)
		u[1] = vknots.back() - tol;
	if (fabs(u[2] - wknots.back()) < tol)
		u[2] = wknots.back() - tol;
	m_Basis[0].getAllNurbsBasisFuns(uspan, u[0], Nu);
	m_Basis[1].getAllNurbsBasisFuns(vspan, u[1], Nv);
	m_Basis[2].getAllNurbsBasisFuns(wspan, u[2], Nw);

	double w = 0.0;
	int uind, vind, wind, ind;
	for (int k = 0; k <= r; ++k) {
		wind = wspan - r + k;
		for (int j = 0; j <= q; ++j)
		{
			vind = vspan - q + j;
			for (int i = 0; i <= p; ++i)
			{
				uind = uspan - p + i;
				ind =  wind * (vPt + 1) * (uPt + 1) + vind * (uPt + 1) + uind;
				w += Nu[i] * Nv[j] * Nw[k] * weight[ind];
			}
		}
	}

	basisValue.resize((p + 1) * (q + 1) * (r + 1));
	int cnt = 0;
	for (int k = 0; k <= r; ++k) {
		wind = wspan - r + k;
		for (int j = 0; j <= q; ++j)
		{
			vind = vspan - q + j;
			for (int i = 0; i <= p; ++i)
			{
				uind = uspan - p + i;
				ind = wind * (vPt + 1) * (uPt + 1) + vind * (uPt + 1) + uind;
				basisValue[cnt++] = Nu[i] * Nv[j] * Nw[k] * weight[ind] / w;
			}
		}
	}
}

void Volume::evalDers(std::vector<double>& u, std::vector<std::vector<double>>& basisValue)
{
	double tol = 1e-15;
	int p = m_Basis[0].getDegree(), q = m_Basis[1].getDegree(), r = m_Basis[2].getDegree();
	int uPt = m_Basis[0].getKnotSize() - 1 - p - 1, vPt = m_Basis[1].getKnotSize() - 1 - q - 1, wPt = m_Basis[2].getKnotSize() - 1 - r - 1;
	int uspan = m_Basis[0].findSpan(u[0]), vspan = m_Basis[1].findSpan(u[1]), wspan = m_Basis[2].findSpan(u[2]);
	auto weight = m_Weights;

	auto uknots = m_Basis[0].getKnots(), vknots = m_Basis[1].getKnots(), wknots = m_Basis[2].getKnots();
	std::vector <double> Nu(p + 1), Nv(q + 1), Nw(r + 1);
	std::vector<std::vector<double>> Nu_ders(uPt + 1, std::vector<double>(p + 1)),
		Nv_ders(vPt + 1, std::vector<double>(q + 1)), Nw_ders(wPt + 1, std::vector<double>(r + 1));

	if (fabs(u[0] - uknots.back()) < tol)
		u[0] = uknots.back() - tol;
	if (fabs(u[1] - vknots.back()) < tol)
		u[1] = vknots.back() - tol;
	if (fabs(u[2] - wknots.back()) < tol)
		u[2] = wknots.back() - tol;

	m_Basis[0].getAllNurbsBasisFuns(uspan, u[0], Nu);
	m_Basis[1].getAllNurbsBasisFuns(vspan, u[1], Nv);
	m_Basis[2].getAllNurbsBasisFuns(wspan, u[2], Nw);
	m_Basis[0].getAllNurbsBasisFunsDers(uspan, u[0], uPt, Nu_ders);
	m_Basis[1].getAllNurbsBasisFunsDers(vspan, u[1], vPt, Nv_ders);
	m_Basis[2].getAllNurbsBasisFunsDers(wspan, u[2], wPt, Nw_ders);

	double w = 0.0, dNdxi = 0.0, dNdeta = 0.0, dNdzeta = 0.0;
	int uind, vind, wind, ind;
	for (int k = 0; k <= r; ++k) {
		wind = wspan - r + k;
		for (int j = 0; j <= q; ++j)
		{
			vind = vspan - q + j;
			for (int i = 0; i <= p; ++i)
			{
				uind = uspan - p + i;
				ind = wind * (vPt + 1) * (uPt + 1) + vind * (uPt + 1) + uind;
				w += Nu[i] * Nv[j] * Nw[k] * weight[ind];
				dNdxi += Nu_ders[1][i] * Nv[j] * Nw[k] * weight[ind];
				dNdeta += Nu[i] * Nv_ders[1][j] * Nw[k] * weight[ind];
				dNdzeta += Nu[i] * Nv[j] * Nw_ders[1][k] * weight[ind];
			}
		}
	}
	basisValue.resize(2);
	basisValue[0].resize((p + 1) * (q + 1) * (r + 1));
	basisValue[1].resize(3 * (p + 1) * (q + 1) * (r + 1));
	int cnt = 0;
	double fac;
	for (int k = 0; k <= r; ++k) {
		wind = wspan - r + k;
		for (int j = 0; j <= q; ++j)
		{
			vind = vspan - q + j;
			for (int i = 0; i <= p; ++i)
			{
				uind = uspan - p + i;
				ind = wind * (vPt + 1) * (uPt + 1) + vind * (uPt + 1) + uind;
				fac = weight[ind] / (w * w);

				basisValue[0][cnt] = Nu[i] * Nv[j] * Nw[k] * weight[ind] / w;
				basisValue[1][3 * cnt] = (Nu_ders[1][i] * Nv[j] * Nw[k] * w - Nu[i] * Nv[j] * Nw[k] * dNdxi) * fac;
				basisValue[1][3 * cnt + 1] = (Nu[i] * Nv_ders[1][j] * Nw[k] * w - Nu[i] * Nv[j] * Nw[k] * dNdeta) * fac;
				basisValue[1][3 * cnt + 2] = (Nu[i] * Nv[j] * Nw_ders[1][k] * w - Nu[i] * Nv[j] * Nw[k] * dNdzeta) * fac;
				cnt++;
			}
		}
	}
}
