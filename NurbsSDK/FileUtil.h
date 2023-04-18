#pragma once
#include"MultiGeo.h"
#include"tinyXMl/tinyxml.h"
#include <regex>
#include<fstream>
class FileUtil {
public:
	FileUtil(){}

	FileUtil(const std::string filename, MultiGeo& geometry);

	static void readFileXml(const std::string filename, MultiGeo& geometry);

	static void saveFileXml(const std::string filename, MultiGeo& Geo);


};