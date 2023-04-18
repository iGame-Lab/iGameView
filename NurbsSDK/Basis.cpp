#include "Basis.h"


Basis::Basis(int degree, const std::vector<double>& knots)
{
	m_Degree = degree;
	m_Knots = knots;
}


int Basis::findSpan(double u) const
{
	int n = m_Knots.size() - 1 - m_Degree - 1;
	int low = m_Degree, high = n + 1;
	int middle = (low + high) / 2;

	if (u >= m_Knots[n + 1])  return n;
	if (u < m_Knots[m_Degree]) return m_Degree;

	while (u < m_Knots[middle] || u >= m_Knots[middle + 1])
	{
		if (u < m_Knots[middle])
			high = middle;
		else
			low = middle;
		middle = (low + high) / 2;
	}
	return middle;
}

void Basis::getAllNurbsBasisFuns(int i, double u, std::vector<double>& N) const
{
	int j, r;    // N_{j,r}
	double saved, temp;
	std::vector<double> left(m_Degree + 1), right(m_Degree + 1);

	N[0] = 1.0;
	for (j = 1; j <= m_Degree; j++)
	{
		left[j] = u - m_Knots[i + 1 - j];
		right[j] = m_Knots[i + j] - u;
		saved = 0.0;
		for (r = 0; r < j; r++)
		{
			temp = N[r] / (right[r + 1] + left[j - r]);
			N[r] = saved + right[r + 1] * temp;
			saved = left[j - r] * temp;
		}
		N[j] = saved;
	}
}

void Basis::getAllNurbsBasisFunsDers(int i, double u,int nd, std::vector<std::vector<double>>& ders) const
{
	int j, r, k, s1, s2, rk, pk, j1, j2;
	double saved, temp, d;
	std::vector<double> left(m_Degree + 1), right(m_Degree + 1);
	std::vector<std::vector<double>> ndu(m_Degree + 1, std::vector<double>(m_Degree + 1));
	std::vector<std::vector<double>> a(m_Degree + 1, std::vector<double>(m_Degree + 1));

	ndu[0][0] = 1.0;
	for (j = 1; j <= m_Degree; j++)
	{
		left[j] = u - m_Knots[i + 1 - j];
		right[j] = m_Knots[i + j] - u;
		saved = 0.0;
		for (r = 0; r < j; r++)
		{
			ndu[j][r] = right[r + 1] + left[j - r];
			temp = ndu[r][j - 1] / ndu[j][r];
			ndu[r][j] = saved + right[r + 1] * temp;
			saved = left[j - r] * temp;
		}
		ndu[j][j] = saved;
	}

	for (j = 0; j <= m_Degree; j++)
		ders[0][j] = ndu[j][m_Degree];

	for (r = 0; r <= m_Degree; r++)
	{
		s1 = 0; s2 = 1;
		a[0][0] = 1.0;

		for (k = 1; k <= nd; k++)
		{
			d = 0.0;
			rk = r - k;
			pk = m_Degree - k;
			if (r >= k)
			{
				a[s2][0] = a[s1][0] / ndu[pk + 1][rk];
				d = a[s2][0] * ndu[rk][pk];
			}
			if (rk >= -1)  j1 = 1;
			else          j1 = -rk;
			if (r - 1 <= pk) j2 = k - 1;
			else          j2 = m_Degree - r;

			for (j = j1; j <= j2; j++)
			{
				a[s2][j] = (a[s1][j] - a[s1][j - 1]) / ndu[pk + 1][rk + j];
				d += a[s2][j] * ndu[rk + j][pk];
			}

			if (r <= pk)
			{
				a[s2][k] = -a[s1][k - 1] / ndu[pk + 1][r];
				d += a[s2][k] * ndu[r][pk];
			}
			ders[k][r] = d;
			j = s1; s1 = s2; s2 = j;
		}
	}

	r = m_Degree;
	for (k = 1; k <= nd; k++)
	{
		for (j = 0; j <= m_Degree; j++)
			ders[k][j] *= r;
		r *= (m_Degree - k);
	}
}
