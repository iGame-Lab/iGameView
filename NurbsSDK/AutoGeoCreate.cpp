#include "AutoGeoCreate.h"
#include<assert.h>
// 
bool AutoGeoCreate::CreateModel(MultiGeo& geo, int uCptNum, int uDegreeNum)
{
    if (uDegreeNum + 1 > uCptNum) return false;
    std::vector<Point> controlPoints(uCptNum);
    int uKnotSize = uCptNum + uDegreeNum + 1;
    std::vector<double> knots(uKnotSize), weights(uCptNum);
    for (int i = 0; i <= uDegreeNum; ++i) knots[i] = 0, knots[uKnotSize - 1 - i] = 1;

    double uKnotSpan = 1.0 / (uKnotSize - 2 * uDegreeNum - 1), uCptSpan = 1.0 / (uCptNum - 1);

    for (int i = uDegreeNum + 1; i < uKnotSize - uDegreeNum - 1; ++i)
    {
        knots[i] = knots[i - 1] + uKnotSpan;
    }

    for (int i = 0; i < uCptNum; ++i)
    {
        controlPoints[i] = { uCptSpan * i, 0, 0 };
        weights[i] = 1;
    }

    std::vector<Geometry> curve = { std::make_shared<Curve>(uDegreeNum,controlPoints, knots, weights)};
    geo = MultiGeo{ curve };
    geo.setBoundaryInfo({ {0,0},{0,1} });
    return true;
}


bool AutoGeoCreate::CreateModel(MultiGeo& geo, int uCptNum, int vCptNum, int uDegreeNum, int vDegreeNum)
{
    if (uDegreeNum + 1 > uCptNum || vDegreeNum + 1 > vCptNum) return false;
    std::vector<Point> controlPoints(uCptNum * vCptNum);
    int uKnotSize = uCptNum + uDegreeNum + 1, vKnotSize = vCptNum + vDegreeNum + 1;
    std::vector<double> uKnots(uKnotSize), vKnots(vKnotSize), weights(uCptNum * vCptNum);
    for (int i = 0; i <= uDegreeNum; ++i) uKnots[i] = 0, uKnots[uKnotSize - 1 - i] = 1;
    for (int i = 0; i <= vDegreeNum; ++i) vKnots[i] = 0, vKnots[vKnotSize - 1 - i] = 1;

    double uKnotSpan = 1.0 / (uKnotSize - 2 * uDegreeNum - 1), uCptSpan = 1.0 / (uCptNum - 1);
    double vKnotSpan = 1.0 / (vKnotSize - 2 * vDegreeNum - 1), vCptSpan = 1.0 / (vCptNum - 1);

    for (int i = uDegreeNum + 1; i < uKnotSize - uDegreeNum - 1; ++i)
    {
        uKnots[i] = uKnots[i - 1] + uKnotSpan;
    }

    for (int i = vDegreeNum + 1; i < vKnotSize - vDegreeNum - 1; ++i)
    {
        vKnots[i] = vKnots[i - 1] + vKnotSpan;
    }
    for (int j = 0; j < vCptNum; ++j) {
        for (int i = 0; i < uCptNum; ++i)
        {
            controlPoints[j * uCptNum + i] = { uCptSpan * i, vCptSpan * j, 0 };
            weights[j * uCptNum + i] = 1;
        }
    }

    std::vector<Geometry> surface = { std::make_shared<Surface>(uDegreeNum,vDegreeNum,controlPoints, uKnots,vKnots, weights) };
    geo = MultiGeo{ surface };
    geo.setBoundaryInfo({ {0,0},{0,1},{0,2},{0,3} });
    return true;
}



bool AutoGeoCreate::CreateModel(MultiGeo& geo, int uCptNum, int vCptNum, int wCptNum, int uDegreeNum, int vDegreeNum, int wDegreeNum)
{
    if (uDegreeNum + 1 > uCptNum || vDegreeNum + 1 > vCptNum || wDegreeNum + 1 > wCptNum) return false;
    std::vector<Point> controlPoints(uCptNum * vCptNum * wCptNum);
    int uKnotSize = uCptNum + uDegreeNum + 1, vKnotSize = vCptNum + vDegreeNum + 1, wKnotSize = wCptNum + wDegreeNum + 1;
    std::vector<double> uKnots(uKnotSize), vKnots(vKnotSize), wKnots(wKnotSize), weights(uCptNum * vCptNum * wCptNum);
    for (int i = 0; i <= uDegreeNum; ++i) uKnots[i] = 0, uKnots[uKnotSize - 1 - i] = 1;
    for (int i = 0; i <= vDegreeNum; ++i) vKnots[i] = 0, vKnots[vKnotSize - 1 - i] = 1;
    for (int i = 0; i <= wDegreeNum; ++i) wKnots[i] = 0, wKnots[wKnotSize - 1 - i] = 1;


    double uKnotSpan = 1.0 / (uKnotSize - 2 * uDegreeNum - 1), uCptSpan = 1.0 / (uCptNum - 1);
    double vKnotSpan = 1.0 / (vKnotSize - 2 * vDegreeNum - 1), vCptSpan = 1.0 / (vCptNum - 1);
    double wKnotSpan = 1.0 / (wKnotSize - 2 * wDegreeNum - 1), wCptSpan = 1.0 / (wCptNum - 1);

    for (int i = uDegreeNum + 1; i < uKnotSize - uDegreeNum - 1; ++i)
    {
        uKnots[i] = uKnots[i - 1] + uKnotSpan;
    }

    for (int i = vDegreeNum + 1; i < vKnotSize - vDegreeNum - 1; ++i)
    {
        vKnots[i] = vKnots[i - 1] + vKnotSpan;
    }

    for (int i = wDegreeNum + 1; i < wKnotSize - wDegreeNum - 1; ++i)
    {
        wKnots[i] = wKnots[i - 1] + wKnotSpan;
    }

    for (int k = 0; k < wCptNum; ++k) {
        for (int j = 0; j < vCptNum; ++j) {
            for (int i = 0; i < uCptNum; ++i)
            {
                controlPoints[k * uCptNum * vCptNum + j * uCptNum + i] = { uCptSpan * i, vCptSpan * j, wCptSpan * k };
                weights[k * uCptNum * vCptNum + j * uCptNum + i] = 1;
            }
        }
    }

    std::vector<Geometry> volume = { std::make_shared<Volume>(uDegreeNum,vDegreeNum,wDegreeNum,controlPoints, uKnots,vKnots,wKnots, weights) };
    geo = MultiGeo{ volume };
    geo.setBoundaryInfo({ {0,0},{0,1},{0,2},{0,3},{0,4},{0,5} });
    return true;
}
