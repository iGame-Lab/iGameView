#include "FileUtil.h"
#include"iostream"
FileUtil::FileUtil(const std::string filename, MultiGeo& geometry)
{
	auto loc = filename.find_last_of('.');
	std::string form = filename.substr(loc+1);
	if (form == "xml") readFileXml(filename, geometry);
	else
	{
		printf("error/n");
	}
}

void FileUtil::readFileXml(const std::string filename, MultiGeo& geometry)
{
	const char* filepath = (char*)filename.data();
	TiXmlDocument doc(filepath);
	bool loadOkay = doc.LoadFile();

	if (!loadOkay) {
		printf("Could not load test file %s. Error='%s'. Exiting.\n", filepath, doc.ErrorDesc());
		exit(1);
	}

	TiXmlElement* root = doc.RootElement();

	TiXmlElement* item = root->FirstChildElement();
	const char* type = item->ValueTStr().c_str();
	std::string s_type = type;


	for (TiXmlNode* item = root->FirstChild(type); item; item = item->NextSibling(type)) {

		int num = s_type == "curve" ? 1 : (s_type == "surface" ? 2 : (s_type == "volume" ? 3 : 0));
		if (num == 0) { printf("xml tag error\n"); exit(0); }


		std::vector<int> degree(num), cpt(num);
		std::vector<double> weights;
		std::vector<std::vector<double>> knots(num);
		std::vector<Point> points;

		std::regex pattern(" ");
		std::string value = item->FirstChild("degree")->ToElement()->GetText();
		std::vector<std::string> s_degree(std::sregex_token_iterator(value.begin(), value.end(), pattern, -1), std::sregex_token_iterator());
		assert(static_cast<int>(s_degree.size()) == num);
		for (int i = 0; i < num; ++i)
			degree[i] = std::stoi(s_degree[i]);

		int cptNum = 1;
		value = item->FirstChild("number")->ToElement()->GetText();
		std::vector<std::string> s_number(std::sregex_token_iterator(value.begin(), value.end(), pattern, -1), std::sregex_token_iterator());
		assert(static_cast<int>(s_number.size()) == num);
		for (int i = 0; i < num; ++i)
			cpt[i] = std::stoi(s_number[i]), cptNum *= cpt[i];

		TiXmlNode* knot_item = nullptr;
		for (int i = 0; i < num; ++i)
		{
			knot_item = knot_item ? knot_item->NextSibling("knots") : item->FirstChild("knots");
			value = knot_item->ToElement()->GetText();
			std::vector<std::string> s_knot(std::sregex_token_iterator(value.begin(), value.end(), pattern, -1), std::sregex_token_iterator());
			assert(static_cast<int>(s_knot.size()) == cpt[i] + degree[i] + 1);
			for (int j = 0; j < s_knot.size(); ++j)
				knots[i].push_back(std::stod(s_knot[j]));

			// ¹éÒ»»¯
			double interval = knots[i].back() - knots[i].front();
			double begin = knots[i].front();
			for (int j = 0; j < knots[i].size(); ++j)
				knots[i][j] = (knots[i][j] - begin) / interval;

		}

		value = item->FirstChild("weights")->ToElement()->GetText();
		std::vector<std::string> s_weight(std::sregex_token_iterator(value.begin(), value.end(), pattern, -1), std::sregex_token_iterator());
		assert(static_cast<int>(s_weight.size()) == cptNum);
		for (int i = 0; i < cptNum; ++i)
			weights.emplace_back(std::stod(s_weight[i]));

		value = item->FirstChild("points")->ToElement()->GetText();
		std::vector<std::string> s_points(std::sregex_token_iterator(value.begin(), value.end(), pattern, -1), std::sregex_token_iterator());
		assert(static_cast<int>(s_points.size()) == cptNum * 3);
		for (int i = 0; i < cptNum; ++i)
			points.emplace_back(Point{ std::stod(s_points[i * 3]),std::stod(s_points[i * 3 + 1]),std::stod(s_points[i * 3 + 2]) });


		std::shared_ptr<Geo> patch;
		if (num == 1) patch = std::make_shared<Curve>(degree[0], points, knots[0], weights);
		else if (num == 2) patch = std::make_shared<Surface>(degree[0], degree[1], points, knots[0], knots[1], weights);
		else patch = std::make_shared<Volume>(degree[0], degree[1], degree[2], points, knots[0], knots[1], knots[2], weights);

		geometry.addPatch(patch);
	}
	std::regex pat(" ");
	auto bdyEle = root->FirstChild("boundary");
	std::vector<std::array<int, 2>> boundary;
	if (bdyEle && bdyEle->ToElement()->GetText() != 0) {
		std::string s_bdy = bdyEle->ToElement()->GetText();
		std::vector<std::string> bdy(std::sregex_token_iterator(s_bdy.begin(), s_bdy.end(), pat, -1), std::sregex_token_iterator());
		for (int i = 0; i < bdy.size() / 2; ++i)
			boundary.push_back({ std::stoi(bdy[2 * i]), std::stoi(bdy[2 * i + 1]) });
	}
	else if (s_type == "volume")
	{
		printf("error!!! Volume need boundary to draw\n");
	}
	geometry.setBoundaryInfo(boundary);
}

void FileUtil::saveFileXml(const std::string filename, MultiGeo& Geo)
{
	std::ofstream cur;
	cur.open(filename);
	cur << "<xml>" << "\n";

	for (int i = 0; i < Geo.getPatchSize(); ++i)
	{
		Geometry geometry = Geo.PatchPointer(i);
		std::string gtype = ""; int parDim = 0;
		if (geometry->type == Geo::Gtype::Curve) gtype = "curve", parDim = 1;
		else if (geometry->type == Geo::Gtype::Surface) gtype = "surface", parDim = 2;
		else if (geometry->type == Geo::Gtype::Volume) gtype = "volume", parDim = 3;

		cur << "  <" + gtype + ">" << "\n";

		cur << "    <degree>";
		for (int j = 0; j < parDim; ++j) cur << std::to_string(geometry->m_Basis[j].getDegree()) << " ";
		cur << "    </degree>\n";

		cur << "    <number>";
		for (int j = 0; j < parDim; ++j) cur << std::to_string(geometry->m_Basis[j].getKnotSize() - geometry->m_Basis[j].getDegree() - 1) << " ";
		cur << "    </number>\n";

		for (int j = 0; j < parDim; ++j)
		{
			auto knots = geometry->m_Basis[j].getKnots();
			cur << "    <knots>";
			for (auto knot : knots) cur << std::to_string(knot) << " ";
			cur << "    </knots>\n";
		}

		auto& weights = geometry->m_Weights;
		cur << "    <weights>\n";
		for (auto weight : weights) cur << std::to_string(weight) << " ";
		cur << "\n    </weights>\n";

		auto& points = geometry->m_ControlPoints;
		cur << "    <points>\n";
		for (auto point : points) cur << "      " << std::to_string(point[0]) << " " << std::to_string(point[1]) << " " << std::to_string(point[2]) << "\n";
		cur << "    </points>\n";

		cur << "  </" + gtype + ">" << "\n";

	}



	auto& boundary = Geo.m_Boundary;
	cur << "  <boundary>\n";
	for (auto& bdy : boundary) cur << "    " << std::to_string(bdy[0]) << " " << std::to_string(bdy[1]) << "\n";
	cur << "\n  </boundary>\n";

	cur << "</xml>" << "\n";
	cur.close();
}
