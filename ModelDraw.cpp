#include "ModelDraw.h"
#include <QtCore/QtCore>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/qprogressdialog.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "NurbsSDK/AutoGeoCreate.h"

ModelDraw::ModelDraw(QWidget* parent) :ViewOpenGL(parent),
editModel(MODELVIEW),
pickType(PickPoint)
{

}

ModelDraw::~ModelDraw(void) {

}

/*=====================================表面网格操作=========================================*/

void ModelDraw::createModel() {
	currentNewID++;
	Model* model = new Model();

	modelList.insert(std::make_pair(currentNewID, model));
	currentModelID = currentNewID;
	currentModel = modelList[currentNewID];
	currentModel->modelScale = 1.0f;
	currentModel->VAO = 0;
	currentModel->VBO = 0;
	currentModel->pVAO = 0;
	currentModel->pVBO = 0;
}

void ModelDraw::deleteModel() {
	modelList.erase(currentModelID);

	std::map<int, Model*>::iterator it;
	it = modelList.begin();
	if (it != modelList.end()) {
		currentModel = modelList[it->first];
		currentModelID = it->first;
	}
	else {
		currentModel = nullptr;
		currentModelID = -1;
	}
}

bool ModelDraw::loadNurbsGeo(const std::string& fileName) {
	int beg = fileName.find_last_of('/');
	QString filePath = QString::fromStdString(fileName.substr(++beg, fileName.length()));

	clock_t time1 = clock();
	QString file_full;
	QFileInfo fileinfo;
	file_full = QString::fromLocal8Bit(fileName.c_str());
	fileinfo = QFileInfo(file_full);
	file_suffix = fileinfo.suffix();
	int sides_num;

	createModel();
	currentModel->structure = NURBS;
	saveFlieName(filePath.toStdString());
	if (file_suffix == "xml") {
		std::cout << fileName << std::endl;
		FileUtil::readFileXml(fileName, currentModel->nurbs_patchs);
		//currentModel->nurbs_patchs.readFileXml(fileName);
	}
	else {
		std::cerr << file_suffix.data() << "is not support! " << std::endl;
		return false;
	}
	std::cout << "currentModelID: " << currentModelID << std::endl;

	updateMeshModel();
	updateCurrentMesh();
	clock_t time2 = clock();
	std::cout << time2 - time1 << " ms" << std::endl;
	return true;
}




void ModelDraw::creatNurbs(const std::vector<int>& control_points_size, const std::vector<int>& degree_size) {
    createModel();
    currentModel->structure = NURBS;
    std::cout << "start creatNurbs! \n";
    if (control_points_size.size() == 1) {
        bool ans = AutoGeoCreate::CreateModel(currentModel->nurbs_patchs, control_points_size[0], degree_size[0]);
    }
    else if (control_points_size.size() == 2) {
        bool ans = AutoGeoCreate::CreateModel(currentModel->nurbs_patchs, control_points_size[0], control_points_size[1], degree_size[0], degree_size[1]);
    }
    else if(control_points_size.size() == 3) {
        bool ans = AutoGeoCreate::CreateModel(currentModel->nurbs_patchs, control_points_size[0], control_points_size[1], control_points_size[2], degree_size[0], degree_size[1], degree_size[2]);
    }

    updateMeshModel();
    updateCurrentMesh();
}

bool ModelDraw::saveNurbsGeo(const std::string& filename) {
	if (!currentModel) {
		return false;
	}
	auto file_full = QString::fromLocal8Bit(filename.c_str());
	auto fileinfo = QFileInfo(file_full);
	auto file_suffix = fileinfo.suffix();
	MultiGeo io;
	if (file_suffix == "xml")
	{
		FileUtil::saveFileXml(filename, currentModel->nurbs_patchs);
	}
	return true;
}




void ModelDraw::clearMesh() {


	deleteModel();
	update();
}

void ModelDraw::changeCurrentMeshToNext() {

	std::map<int, Model*>::iterator it;
	it = modelList.find(currentModelID);
	if (it == --modelList.end()) {
		it = modelList.begin();
	}
	else {
		it++;
	}
	currentModel = modelList[it->first];
	currentModelID = it->first;
	showInfo();
	update();
}

void ModelDraw::changeCurrentMeshToLast() {

	std::map<int, Model*>::iterator it;
	it = modelList.find(currentModelID);
	if (it == modelList.begin()) {
		it = --modelList.end();
	}
	else {
		it--;
	}
	currentModel = modelList[it->first];
	currentModelID = it->first;
	showInfo();
	update();
}

void ModelDraw::updateMeshModel(bool isFocus) {
	if (currentModel->structure == NURBS) {

		if (currentModel->pick_vertices.empty() == false) {
			int id = 0;
			for (auto& pv : currentModel->pick_vertices) {
				if(id % 3 == 0) pv = pv / 2 * init_max_gap + centerX;
				else if (id % 3 == 1) pv = pv / 2 * init_max_gap + centerY;
				else if (id % 3 == 2) pv = pv / 2 * init_max_gap + centerZ;
				id++;
			}
		}

		minX = std::numeric_limits<double>::max(); maxX = -std::numeric_limits<double>::max();
		minY = std::numeric_limits<double>::max(); maxY = -std::numeric_limits<double>::max();
		minZ = std::numeric_limits<double>::max(); maxZ = -std::numeric_limits<double>::max();
		for (auto& patch : currentModel->nurbs_patchs.m_Geometry) {
			auto& cc = patch->m_ControlPoints;
			for (auto& geo_point : cc) {

				minX = std::min(minX, geo_point[0]), maxX = std::max(maxX, geo_point[0]);
				minY = std::min(minY, geo_point[1]), maxY = std::max(maxY, geo_point[1]);
				minZ = std::min(minZ, geo_point[2]), maxZ = std::max(maxZ, geo_point[2]);
			}
		}
		centerX = (minX + maxX) / 2;
		centerY = (minY + maxY) / 2;
		centerZ = (minZ + maxZ) / 2;
		maxZ += 0.01; maxY += 0.01; maxX += 0.01;
		minZ -= 0.01; minY -= 0.01; minX -= 0.01;
		init_max_gap = 0.01;
		init_max_gap = fmax(maxX - minX, maxY - minY);
		init_max_gap = fmax(init_max_gap, maxZ - minZ);


		if (!currentModel->pick_vertices.empty()) {
			int id = 0;
			for (auto& pv : currentModel->pick_vertices) {
				if (id % 3 == 0)  pv = (pv - centerX) / init_max_gap * 2;
				else if (id % 3 == 1) pv = (pv - centerY) / init_max_gap * 2;
				else if(id % 3 == 2) pv = (pv - centerZ) / init_max_gap * 2;
				id++;
			}
		}
		if(isFocus)	SetScenePosition(Vector3d(0, 0, 0), 1.2f * radius);
		update();
	}
	else {
		SetScenePosition(Vector3d(0, 0, 0), 1.6f * radius);
		update();
	}

}

// 每当mesh被修改之后，都要把新的顶点数据传入VBO中
void ModelDraw::updateCurrentMesh() {
	currentModel->vertices.clear();
	if (currentModel->structure == NURBS) {
		/// Todo Curve VOLUME
		if (currentModel->nurbs_patchs.m_Geometry.size() == 0) return;

		if ((currentModel->nurbs_patchs.m_Geometry)[0]->type == Geo::Surface) {
			currentModel->meshType = NURBSSURFACE;
		}
		else if ((currentModel->nurbs_patchs.m_Geometry)[0]->type == Geo::Curve) {
			currentModel->meshType = NURBSCURVE;
		}
		else if ((currentModel->nurbs_patchs.m_Geometry)[0]->type == Geo::Volume) {
			currentModel->meshType = NURBSVOLUME;
		}

		currentModel->control_face_cnt = 0;
		currentModel->all_sample_face_cnt = 0;

		currentModel->control_interval_cnt = 0;
		currentModel->all_sample_interval_cnt = 0;

		currentModel->control_grid_cnt = 0;
		currentModel->all_sample_grid_cnt = 0;



		if (currentModel->meshType == NURBSSURFACE) {


			int SAMPLE_NUM = (1000 / currentModel->nurbs_patchs.m_Geometry.size());
			SAMPLE_NUM = std::max(SAMPLE_NUM, 1);
			SAMPLE_NUM = std::min(SAMPLE_NUM, 100);

			// 1. 传入控制顶点
			for (auto& patch : currentModel->nurbs_patchs.m_Geometry) {

				int u_control_cnt = patch->m_Basis[0].getControlSize();
				int v_control_cnt = patch->m_Basis[1].getControlSize();


				for (int u_id = 0; u_id < u_control_cnt - 1; ++u_id) {
					for (int v_id = 0; v_id < v_control_cnt - 1; ++v_id) {
						auto v0 = patch->m_ControlPoints[u_id + v_id * u_control_cnt];
						auto v1 = patch->m_ControlPoints[(u_id + 1) + v_id * u_control_cnt];
						auto v2 = patch->m_ControlPoints[(u_id + 1) + (v_id + 1) * u_control_cnt];
						auto v3 = patch->m_ControlPoints[u_id + (v_id + 1) * u_control_cnt];
						std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
						for (auto v : tempV) {
							currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
							currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
							currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

							currentModel->vertices.push_back(0);
							currentModel->vertices.push_back(0);
							currentModel->vertices.push_back(0);
						}
						currentModel->control_face_cnt++;
					}
				}

			}
			// 2.传入离散化面片
			for (auto& patch : currentModel->nurbs_patchs.m_Geometry)
			{
				double sample_gap = 1.f / SAMPLE_NUM;
				for (int u_sample = 0; u_sample < SAMPLE_NUM; ++u_sample) {
					for (int v_sample = 0; v_sample < SAMPLE_NUM; ++v_sample) {
						currentModel->all_sample_face_cnt++;
						
						auto v0 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap, v_sample* sample_gap});
						auto v1 = patch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, v_sample* sample_gap});
						auto v2 = patch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, (v_sample + 1)* sample_gap});
						auto v3 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap, (v_sample + 1)* sample_gap});
						std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
						for (auto v : tempV) {
							currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
							currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
							currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

							currentModel->vertices.push_back(0);
							currentModel->vertices.push_back(0);
							currentModel->vertices.push_back(0);
						}

					}
				}
			}

			std::cout << currentModel->control_face_cnt << " " << currentModel->all_sample_face_cnt << std::endl;
		}
		else if (currentModel->meshType == NURBSCURVE) {
			/// TODO:
			int SAMPLE_NUM = (1000 / currentModel->nurbs_patchs.m_Geometry.size());
			SAMPLE_NUM = std::max(SAMPLE_NUM, 1);
			SAMPLE_NUM = std::min(SAMPLE_NUM, 100);
			//1.
			for (auto& patch : currentModel->nurbs_patchs.m_Geometry)
			{

				int u_control_cnt = patch->m_Basis[0].getControlSize();

				for (int u_id = 0; u_id < u_control_cnt; ++u_id)
				{
					auto v0 = patch->m_ControlPoints[u_id];

					std::vector<Point*> tempV{ &v0 };
					for (auto v : tempV)
					{
						currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
						currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
						currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

						currentModel->vertices.push_back(0);
						currentModel->vertices.push_back(0);
						currentModel->vertices.push_back(0);
					}
					currentModel->control_interval_cnt++;
				}

				//离散化的采样区间（线段）绘制
				for (auto& patch : currentModel->nurbs_patchs.m_Geometry)
				{
					double sample_gap = 1.f / SAMPLE_NUM;
					for (int u_sample = 0; u_sample < SAMPLE_NUM; ++u_sample) {
						currentModel->all_sample_interval_cnt++;

						auto v0 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap });


						std::vector<Point*> tempV{ &v0 };
						for (auto v : tempV) {
							currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
							currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
							currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

							currentModel->vertices.push_back(0);
							currentModel->vertices.push_back(0);
							currentModel->vertices.push_back(0);
						}

					}
				}


			}

			std::cout << currentModel->control_interval_cnt << " " << currentModel->all_sample_interval_cnt << std::endl;
		}
		else if (currentModel->meshType == NURBSVOLUME) {
			/// TODO:
			for (auto patch : currentModel->nurbs_patchs.m_Geometry)
			{
				int u_control_cnt = patch->m_Basis[0].getControlSize();
				int v_control_cnt = patch->m_Basis[1].getControlSize();
				int w_control_cnt = patch->m_Basis[2].getControlSize();
				//创建三维数组存储
				int uvCnt = u_control_cnt * v_control_cnt, uCnt = u_control_cnt;

				for (int u_id = 0; u_id < u_control_cnt - 1; ++u_id) {
					for (int v_id = 0; v_id < v_control_cnt - 1; ++v_id) {
						for (int w_id = 0; w_id < w_control_cnt - 1; ++w_id) {
							currentModel->control_grid_cnt++;
							std::vector<int> cube_id(8);
							cube_id[0] = uvCnt * w_id + uCnt * v_id + u_id;
							cube_id[1] = uvCnt * w_id + uCnt * v_id + u_id + 1;
							cube_id[2] = uvCnt * (w_id + 1) + uCnt * v_id + u_id + 1;
							cube_id[3] = uvCnt * (w_id + 1) + uCnt * v_id + u_id;
							cube_id[4] = uvCnt * w_id + uCnt * (v_id + 1) + u_id;
							cube_id[5] = uvCnt * w_id + uCnt * (v_id + 1) + u_id + 1;
							cube_id[6] = uvCnt * (w_id + 1) + uCnt * (v_id + 1) + u_id + 1;
							cube_id[7] = uvCnt * (w_id + 1) + uCnt * (v_id + 1) + u_id;

							std::vector<std::vector<int>> edges = {
								{0,1},{1,2},{2,3},{3,0},
								{0,4},{1,5},{2,6},{3,7},
								{4,5},{5,6},{6,7},{7,4},
							};

							for (auto edge : edges) {
								for (auto vid : edge) {
									auto v = patch->m_ControlPoints[cube_id[vid]];
									//auto v = cube[cube_id[vid]];
									currentModel->vertices.push_back((v[0] - centerX) / init_max_gap * 2);
									currentModel->vertices.push_back((v[1] - centerY) / init_max_gap * 2);
									currentModel->vertices.push_back((v[2] - centerZ) / init_max_gap * 2);
									//currentModel->vertices.push_back(v[0]);
									//currentModel->vertices.push_back(v[1]);
									//currentModel->vertices.push_back(v[2]);

									currentModel->vertices.push_back(0);
									currentModel->vertices.push_back(0);
									currentModel->vertices.push_back(0);
								}
							}

						}

					}
				}
			}

			int SAMPLE_NUM = (1000 / currentModel->nurbs_patchs.m_Geometry.size());
			SAMPLE_NUM = std::max(SAMPLE_NUM, 1);
			SAMPLE_NUM = std::min(SAMPLE_NUM, 100);
			double sample_gap = 1.f / SAMPLE_NUM;

			for (auto arr : currentModel->nurbs_patchs.m_Boundary) {
				int patch_id = arr[0];
				auto currentPatch = currentModel->nurbs_patchs.m_Geometry[patch_id];
				if (arr[1] == 0)
				{
					//u=0oru=1,固定u的大小
					for (int v_sample = 0; v_sample < SAMPLE_NUM; ++v_sample) {
						for (int w_sample = 0; w_sample < SAMPLE_NUM; ++w_sample) {
							currentModel->all_sample_grid_cnt++;
							//用离散采样点获得绘制点
							auto v0 = currentPatch->getPointAtParam(std::vector<double>{0, w_sample* sample_gap, v_sample* sample_gap, 0});
							auto v1 = currentPatch->getPointAtParam(std::vector<double>{0, (w_sample + 1)* sample_gap, v_sample* sample_gap});
							auto v2 = currentPatch->getPointAtParam(std::vector<double>{0, (w_sample + 1)* sample_gap, (v_sample + 1)* sample_gap});
							auto v3 = currentPatch->getPointAtParam(std::vector<double>{0, w_sample* sample_gap, (v_sample + 1)* sample_gap});


							std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
							for (auto v : tempV) {
								currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
							}
						}
					}
				}
				else if (arr[1] == 1)
				{
					for (int v_sample = 0; v_sample < SAMPLE_NUM; ++v_sample) {
						for (int w_sample = 0; w_sample < SAMPLE_NUM; ++w_sample) {
							currentModel->all_sample_grid_cnt++;

							//用离散采样点获得绘制点
							auto v0 = currentPatch->getPointAtParam(std::vector<double>{1, w_sample* sample_gap, v_sample* sample_gap, 0});
							auto v1 = currentPatch->getPointAtParam(std::vector<double>{1, (w_sample + 1)* sample_gap, v_sample* sample_gap});
							auto v2 = currentPatch->getPointAtParam(std::vector<double>{1, (w_sample + 1)* sample_gap, (v_sample + 1)* sample_gap});
							auto v3 = currentPatch->getPointAtParam(std::vector<double>{1, w_sample* sample_gap, (v_sample + 1)* sample_gap});


							std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
							for (auto v : tempV) {
								currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
							}
						}
					}
				}
				else if (arr[1] == 2) {
					for (int w_sample = 0; w_sample < SAMPLE_NUM; ++w_sample) {
						for (int u_sample = 0; u_sample < SAMPLE_NUM; ++u_sample) {
							currentModel->all_sample_grid_cnt++;

							//用离散采样点获得绘制点
							auto v0 = currentPatch->getPointAtParam(std::vector<double>{u_sample* sample_gap, 0, w_sample* sample_gap});
							auto v1 = currentPatch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, 0, w_sample* sample_gap});
							auto v2 = currentPatch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, 0, (w_sample + 1)* sample_gap});
							auto v3 = currentPatch->getPointAtParam(std::vector<double>{u_sample* sample_gap, 0, (w_sample + 1)* sample_gap});


							std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
							for (auto v : tempV) {
								currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
							}
						}
					}
				}
				else if (arr[1] == 3) {
					for (int w_sample = 0; w_sample < SAMPLE_NUM; ++w_sample) {
						for (int u_sample = 0; u_sample < SAMPLE_NUM; ++u_sample) {
							currentModel->all_sample_grid_cnt++;

							//用离散采样点获得绘制点
							auto v0 = currentPatch->getPointAtParam(std::vector<double>{u_sample* sample_gap, 1, w_sample* sample_gap});
							auto v1 = currentPatch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, 1, w_sample* sample_gap});
							auto v2 = currentPatch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, 1, (w_sample + 1)* sample_gap});
							auto v3 = currentPatch->getPointAtParam(std::vector<double>{u_sample* sample_gap, 1, (w_sample + 1)* sample_gap});


							std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
							for (auto v : tempV) {
								currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
							}
						}
					}
				}
				else if (arr[1] == 4) {
					for (int v_sample = 0; v_sample < SAMPLE_NUM; ++v_sample) {
						for (int u_sample = 0; u_sample < SAMPLE_NUM; ++u_sample) {
							currentModel->all_sample_grid_cnt++;

							//用离散采样点获得绘制点
							auto v0 = currentPatch->getPointAtParam(std::vector<double>{u_sample* sample_gap, v_sample* sample_gap, 0});
							auto v1 = currentPatch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, v_sample* sample_gap, 0});
							auto v2 = currentPatch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, (v_sample + 1)* sample_gap, 0});
							auto v3 = currentPatch->getPointAtParam(std::vector<double>{u_sample* sample_gap, (v_sample + 1)* sample_gap, 0});


							std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
							for (auto v : tempV) {
								currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
							}
						}
					}

				}
				else if (arr[1] == 5) {
					for (int w_sample = 0; w_sample < SAMPLE_NUM; ++w_sample) {
						for (int u_sample = 0; u_sample < SAMPLE_NUM; ++u_sample) {
							currentModel->all_sample_grid_cnt++;

							//用离散采样点获得绘制点
							auto v0 = currentPatch->getPointAtParam(std::vector<double>{u_sample* sample_gap, w_sample* sample_gap, 1});
							auto v1 = currentPatch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, w_sample* sample_gap, 1});
							auto v2 = currentPatch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, (w_sample + 1)* sample_gap, 1});
							auto v3 = currentPatch->getPointAtParam(std::vector<double>{u_sample* sample_gap, (w_sample + 1)* sample_gap, 1});


							std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
							for (auto v : tempV) {
								currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
							}
						}
					}
				}


			}

		}


	}
	update();
}

void ModelDraw::updateCutMesh() {
	if (!currentModel) {
		showInfo();
		return;
	}
	currentModel->vertices.clear();
	if (currentModel->structure == NURBS) {
		if (flag_CutPlane) {
			currentModel->vertices.clear();
			/*minX = std::numeric_limits<double>::max(); maxX = std::numeric_limits<double>::min();
			minY = std::numeric_limits<double>::max(); maxY = std::numeric_limits<double>::min();
			minZ = std::numeric_limits<double>::max(); maxZ = std::numeric_limits<double>::min();

			for (auto& patch : currentModel->nurbs_patchs.m_Geometry) {
				auto& cc = patch->m_ControlPoints;
				for (auto& geo_point : cc) {

					minX = std::min(minX, geo_point[0]), maxX = std::max(maxX, geo_point[0]);
					minY = std::min(minY, geo_point[1]), maxY = std::max(maxY, geo_point[1]);
					minZ = std::min(minZ, geo_point[2]), maxZ = std::max(maxZ, geo_point[2]);
				}
			}*/
			updateMeshModel(false);
			double draw_min_x = cutX * (maxX - minX) + minX;
			double draw_min_y = cutY * (maxY - minY) + minY;
			double draw_min_z = cutZ * (maxZ - minZ) + minZ;
			printf("draw min: (%.2f, %.2f, %.2f)\n", draw_min_x, draw_min_y, draw_min_z);

			if (currentModel->nurbs_patchs.m_Geometry.size() == 0)
				return;

			if ((currentModel->nurbs_patchs.m_Geometry)[0]->type == Geo::Surface)
			{
				currentModel->meshType = NURBSSURFACE;
			}
			else if ((currentModel->nurbs_patchs.m_Geometry)[0]->type == Geo::Curve)
			{
				currentModel->meshType = NURBSCURVE;
			}
			else if ((currentModel->nurbs_patchs.m_Geometry)[0]->type == Geo::Volume)
			{
				currentModel->meshType = NURBSVOLUME;
			}

			currentModel->control_face_cnt = 0;
			currentModel->all_sample_face_cnt = 0;

			currentModel->control_interval_cnt = 0;
			currentModel->all_sample_interval_cnt = 0;

			currentModel->control_grid_cnt = 0;
			currentModel->all_sample_grid_cnt = 0;

			if (currentModel->meshType == NURBSSURFACE)
			{
				int SAMPLE_NUM = (1000 / currentModel->nurbs_patchs.m_Geometry.size());
				SAMPLE_NUM = std::max(SAMPLE_NUM, 1);
				SAMPLE_NUM = std::min(SAMPLE_NUM, 100);

				// 1. 传入控制顶点
				for (auto& patch : currentModel->nurbs_patchs.m_Geometry) {

					int u_control_cnt = patch->m_Basis[0].getControlSize();
					int v_control_cnt = patch->m_Basis[1].getControlSize();


					for (int u_id = 0; u_id < u_control_cnt - 1; ++u_id) {
						for (int v_id = 0; v_id < v_control_cnt - 1; ++v_id) {
							auto v0 = patch->m_ControlPoints[u_id + v_id * u_control_cnt];
							auto v1 = patch->m_ControlPoints[(u_id + 1) + v_id * u_control_cnt];
							auto v2 = patch->m_ControlPoints[(u_id + 1) + (v_id + 1) * u_control_cnt];
							auto v3 = patch->m_ControlPoints[u_id + (v_id + 1) * u_control_cnt];
							std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
							for (auto v : tempV) {
								currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
								currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
								currentModel->vertices.push_back(0);
							}
							currentModel->control_face_cnt++;
						}
					}

				}
				// 2.传入离散化面片
				for (auto& patch : currentModel->nurbs_patchs.m_Geometry)
				{
					double sample_gap = 1.f / SAMPLE_NUM;
					for (int u_sample = 0; u_sample < SAMPLE_NUM; ++u_sample) {
						for (int v_sample = 0; v_sample < SAMPLE_NUM; ++v_sample) {
							currentModel->all_sample_face_cnt++;
							// 这里的minX和maxX有问题，应该取最大最小值，而不是参数点0和1
							auto v0 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap, v_sample* sample_gap});
							auto v1 = patch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, v_sample* sample_gap});
							auto v2 = patch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, (v_sample + 1)* sample_gap});
							auto v3 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap, (v_sample + 1)* sample_gap});
							std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v0, &v2 };
							bool needDraw = true;
							for (auto v : tempV) {
								if ((*v)[0] < draw_min_x || (*v)[1] < draw_min_y || (*v)[2] < draw_min_z)
								{
									needDraw = false;
									break;
								}
							}

							if (needDraw)
							{
								for (auto v : tempV) {

									currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
									currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
									currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

									currentModel->vertices.push_back(0);
									currentModel->vertices.push_back(0);
									currentModel->vertices.push_back(0);
								}
							}

						}
					}
				}
			}

			else if (currentModel->meshType == NURBSVOLUME)
			{
				for (auto patch : currentModel->nurbs_patchs.m_Geometry)
				{

					int u_control_cnt = patch->m_Basis[0].getControlSize();
					int v_control_cnt = patch->m_Basis[1].getControlSize();
					int w_control_cnt = patch->m_Basis[2].getControlSize();
					//创建三维数组存储
					int uvCnt = u_control_cnt * v_control_cnt, uCnt = u_control_cnt;

					for (int u_id = 0; u_id < u_control_cnt - 1; ++u_id) {
						for (int v_id = 0; v_id < v_control_cnt - 1; ++v_id) {
							for (int w_id = 0; w_id < w_control_cnt - 1; ++w_id) {
								currentModel->control_grid_cnt++;
								std::vector<int> cube_id(8);
								cube_id[0] = uvCnt * w_id + uCnt * v_id + u_id;
								cube_id[1] = uvCnt * w_id + uCnt * v_id + u_id + 1;
								cube_id[2] = uvCnt * (w_id + 1) + uCnt * v_id + u_id + 1;
								cube_id[3] = uvCnt * (w_id + 1) + uCnt * v_id + u_id;
								cube_id[4] = uvCnt * w_id + uCnt * (v_id + 1) + u_id;
								cube_id[5] = uvCnt * w_id + uCnt * (v_id + 1) + u_id + 1;
								cube_id[6] = uvCnt * (w_id + 1) + uCnt * (v_id + 1) + u_id + 1;
								cube_id[7] = uvCnt * (w_id + 1) + uCnt * (v_id + 1) + u_id;

								std::vector<std::vector<int>> edges = {
									{0,1},{1,2},{2,3},{3,0},
									{0,4},{1,5},{2,6},{3,7},
									{4,5},{5,6},{6,7},{7,4},
								};

								for (auto edge : edges)
								{
									for (auto vid : edge)
									{
										auto v = patch->m_ControlPoints[cube_id[vid]];
										currentModel->vertices.push_back((v[0] - centerX) / init_max_gap * 2);
										currentModel->vertices.push_back((v[1] - centerY) / init_max_gap * 2);
										currentModel->vertices.push_back((v[2] - centerZ) / init_max_gap * 2);

										currentModel->vertices.push_back(0);
										currentModel->vertices.push_back(0);
										currentModel->vertices.push_back(0);
									}
								}
							}
						}
					}
				}
				int SAMPLE_NUM = (1000 / currentModel->nurbs_patchs.m_Geometry.size());
				SAMPLE_NUM = std::max(SAMPLE_NUM, 1);
				SAMPLE_NUM = std::min(SAMPLE_NUM, 10);
				std::cout << "SAMPLE_NUM = " << SAMPLE_NUM << std::endl;
				double sample_gap = 1.f / SAMPLE_NUM;
				for (auto& patch : currentModel->nurbs_patchs.m_Geometry)
				{
					double sample_gap = 1.f / SAMPLE_NUM;
					for (int u_sample = 0; u_sample < SAMPLE_NUM; ++u_sample)
					{
						for (int v_sample = 0; v_sample < SAMPLE_NUM; ++v_sample)
						{
							for (int w_sample = 0; w_sample < SAMPLE_NUM; ++w_sample)
							{
								currentModel->all_sample_face_cnt++;

								auto v0 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap, v_sample* sample_gap, w_sample* sample_gap});
								auto v1 = patch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, v_sample* sample_gap, w_sample* sample_gap});
								auto v2 = patch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, (v_sample + 1)* sample_gap, w_sample* sample_gap});
								auto v3 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap, (v_sample + 1)* sample_gap, w_sample* sample_gap});
								auto v4 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap, v_sample* sample_gap, (w_sample + 1)* sample_gap});
								auto v5 = patch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, v_sample* sample_gap, (w_sample + 1)* sample_gap});
								auto v6 = patch->getPointAtParam(std::vector<double>{(u_sample + 1)* sample_gap, (v_sample + 1)* sample_gap, (w_sample + 1)* sample_gap});
								auto v7 = patch->getPointAtParam(std::vector<double>{u_sample* sample_gap, (v_sample + 1)* sample_gap, (w_sample + 1)* sample_gap});

								bool needDraw = true;

								const std::vector<Point*> tempV{ &v0, &v1, &v2, &v3, &v4, &v5 ,&v6,&v7 };

								for (auto v : tempV)
								{
									if ((*v)[0] < draw_min_x || (*v)[1] < draw_min_y || (*v)[2] < draw_min_z)
									{
										needDraw = false;
										break;
									}
								}
								if (needDraw)
								{
									const std::vector<Point*> tempV2{
									&v0, &v1, &v2,
									&v3, &v0, &v2,
									&v4, &v5, &v6,
									&v4, &v6, &v7,
									&v0, &v1, &v5,
									&v0, &v5, &v4,
									&v3, &v2, &v6,
									&v3, &v6, &v7,
									&v0, &v3, &v7,
									&v0, &v7, &v4,
									&v1, &v2, &v6,
									&v1, &v6, &v5,
									};

									for (auto v : tempV2)
									{

										currentModel->vertices.push_back(((*v)[0] - centerX) / init_max_gap * 2);
										currentModel->vertices.push_back(((*v)[1] - centerY) / init_max_gap * 2);
										currentModel->vertices.push_back(((*v)[2] - centerZ) / init_max_gap * 2);

										currentModel->vertices.push_back(0);
										currentModel->vertices.push_back(0);
										currentModel->vertices.push_back(0);
									}
								}
							}
						}
					}
				}
				//采样


			}

		}


	}
	update();
}




/*=====================================体网格操作=========================================*/

//bool ModelDraw::loadHedra(std::string fileName) {
//    int beg = fileName.find_last_of('/');
//    QString filePath = QString::fromStdString(fileName.substr(++beg, fileName.length()));
//
//    MeshKernel::IO io;
//    //clearMesh();
//    clock_t time1 = clock();
//    QString file_full;
//    QFileInfo fileinfo;
//    file_full = QString::fromLocal8Bit(fileName.c_str());
//    fileinfo = QFileInfo(file_full);
//    file_suffix = fileinfo.suffix();
//    int sides_num;// todo : check mesh type is tet or hex
//
//    createModel();
//    currentModel->structure = VOLUMEMESH;
//    saveFlieName(filePath.toStdString());
//    if (file_suffix == "mesh") {
//        currentModel->tetMesh = MeshKernel::TetMesh(io.ReadTetMeshFile(fileName));
//    }
//
//    updateMeshModel();
//    updateCurrentMesh();
//    clock_t time2 = clock();
//    std::cout << time2 - time1 << " ms" << std::endl;
//    return true;
//}
//
//bool ModelDraw::saveHedra(std::string fileName) {
//
//    return true;
//}

bool ModelDraw::ConvertMeshToHedra() {

	return false;
}



/*=====================================点选切割操作=========================================*/

void ModelDraw::drawPickPoint() {
	//shaderProgram.setUniformValue("model", viewRotateMatrix * currentModel->modelMatrix);	// 把模型矩阵传入shader
	// 释放旧数据的内存
	core->glDeleteVertexArrays(1, &currentModel->pVAO);
	core->glDeleteBuffers(1, &currentModel->pVBO);
	// 1.新建顶点数组对象
	core->glGenVertexArrays(1, &currentModel->pVAO);
	core->glGenBuffers(1, &currentModel->pVBO);
	// 2. 绑定顶点数组对象
	core->glBindVertexArray(currentModel->pVAO);
	// 3. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	core->glBindBuffer(GL_ARRAY_BUFFER, currentModel->pVBO);
	core->glBufferData(GL_ARRAY_BUFFER, currentModel->pick_vertices.size() * sizeof(GLfloat), currentModel->pick_vertices.data(), GL_STATIC_DRAW);
	// 4. 设定顶点属性指针
	// 顶点坐标传入VBO，location=0
	core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	core->glEnableVertexAttribArray(0);
	core->glBindVertexArray(0);
	core->glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ModelDraw::drawCutLine() {
	shaderProgram.setUniformValue("colormode", 3);
	shaderProgram.setUniformValue("color", QVector4D(230, 230, 0, 0.6));
	// 释放旧数据的内存
	core->glDeleteVertexArrays(1, &cVAO);
	core->glDeleteBuffers(1, &cVBO);
	// 1.新建顶点数组对象
	core->glGenVertexArrays(1, &cVAO);
	core->glGenBuffers(1, &cVBO);
	// 2. 绑定顶点数组对象
	core->glBindVertexArray(cVAO);
	// 3. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	core->glBindBuffer(GL_ARRAY_BUFFER, cVBO);
	core->glBufferData(GL_ARRAY_BUFFER, cutline_vertices.size() * sizeof(GLfloat), cutline_vertices.data(), GL_STATIC_DRAW);
	// 4. 设定顶点属性指针
	// 顶点坐标传入VBO，location=0
	core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	core->glEnableVertexAttribArray(0);
	core->glBindVertexArray(0);
	core->glBindBuffer(GL_ARRAY_BUFFER, 0);
	core->glBindVertexArray(cVAO);	// 前面已经记录了VBO和EBO现在只需一步调用即可
	core->glPointSize(8);
	core->glLineWidth(3);
	core->glDrawArrays(GL_LINES, 0, cutline_vertices.size() / 3);
}

void ModelDraw::pickPoint(double x, double y) {
	if (!currentModel) return;
	QVector4D point(x, y, 0.9, 1.0);		// point为鼠标点击的位置
	QVector4D toward(x, y, -1, 1.0);	// 指向toward与投影矩阵的fov相关
	QVector4D pointEnd = point + 2 * toward;
	QVector4D tpoint = currentModel->modelMatrix.inverted() * viewRotateMatrix.inverted() * viewMatrix.inverted() * projectionMatrix.inverted() * point;
	QVector4D tpointEnd = currentModel->modelMatrix.inverted() * viewRotateMatrix.inverted() * viewMatrix.inverted() * projectionMatrix.inverted() * pointEnd;
	// point1为射线起点，point2为射线终点
	QVector3D point1(tpoint.x() / tpoint.w(), tpoint.y() / tpoint.w(), tpoint.z() / tpoint.w());
	QVector3D point2(tpointEnd.x() / tpointEnd.w(), tpointEnd.y() / tpointEnd.w(), tpointEnd.z() / tpointEnd.w());
	float zMax = -9999999;

	std::vector<QVector3D> tempPoints;
	
	QVector3D tempIntersection;
	if (currentModel->structure == NURBS) {
		if (currentModel->meshType == NURBSSURFACE) {

			if (pickType == PickPoint) {
				double pointMin = 99999;
				bool isPicked = false;
				QVector3D tempVertex;
				// 遍历一个面中所有的点，找出其中离射线最近的点

				for (int test_grid_id = 0; test_grid_id < currentModel->control_face_cnt; ++test_grid_id) {
					for (int i = 0; i < 4; i++) {
						QVector3D point0(currentModel->vertices[test_grid_id * 6 * 6 + i * 6 + 0], currentModel->vertices[test_grid_id * 6 * 6 + i * 6 + 1], currentModel->vertices[test_grid_id * 6 * 6 + i * 6 + 2]);

						double dist = point2LineDis(point1, point2, point0);
						/*if (pointMin > dist) {
							pointMin = dist;
							tempVertex = point0;
						}*/
						QVector4D fDepth(point0, 1.f);
						fDepth = currentModel->modelMatrix * fDepth;
						if (dist < pointMin) {
							//if (zMax < fDepth[2]) {
								//zMax = fDepth[2];
							pointMin = fmin(pointMin, dist);
							tempVertex = point0;
							isPicked = true;
							//}
						}

					}
				}
				std::cout << pointMin << std::endl;
				if (isPicked == false) return;

				pickedItemPosition[0] = tempVertex.x();
				pickedItemPosition[1] = tempVertex.y();
				pickedItemPosition[2] = tempVertex.z();
				bool deleteFlag = false;
				for (int i = 0; i < currentModel->pick_vertices.size() / 3; i++) {
					if (tempVertex.x() == currentModel->pick_vertices[i * 3 + 0]
						&& tempVertex.y() == currentModel->pick_vertices[i * 3 + 1]
						&& tempVertex.z() == currentModel->pick_vertices[i * 3 + 2]) {
						currentModel->pick_vertices.erase(currentModel->pick_vertices.begin() + 3 * i, currentModel->pick_vertices.begin() + 3 * i + 3);
						deleteFlag = true;
					}
				}
				if (deleteFlag == false) {

					pickedItem = "vertex";
					currentModel->pick_vertices.push_back(tempVertex.x());
					currentModel->pick_vertices.push_back(tempVertex.y());
					currentModel->pick_vertices.push_back(tempVertex.z());
				}
			}

		}
		else if (currentModel->meshType == NURBSCURVE) {
			if (pickType == PickPoint) {
				double pointMin = 99999;
				bool isPicked = false;
				QVector3D tempVertex;
				// 遍历一个面中所有的点，找出其中离射线最近的点

				for (int test_grid_id = 0; test_grid_id < currentModel->control_interval_cnt; ++test_grid_id) {

					QVector3D point0(currentModel->vertices[test_grid_id * 6 + 0], currentModel->vertices[test_grid_id * 6 + 1], currentModel->vertices[test_grid_id * 6 + 2]);

					double dist = point2LineDis(point1, point2, point0);
					/*if (pointMin > dist) {
						pointMin = dist;
						tempVertex = point0;
					}*/
					QVector4D fDepth(point0, 1.f);
					fDepth = currentModel->modelMatrix * fDepth;
					if (dist < pointMin) {
						//if (zMax < fDepth[2]) {
							//zMax = fDepth[2];
						pointMin = fmin(pointMin, dist);
						tempVertex = point0;
						isPicked = true;
						//}
					}

				}

				std::cout << pointMin << std::endl;
				if (isPicked == false) return;

				pickedItemPosition[0] = tempVertex.x();
				pickedItemPosition[1] = tempVertex.y();
				pickedItemPosition[2] = tempVertex.z();
				bool deleteFlag = false;
				for (int i = 0; i < currentModel->pick_vertices.size() / 3; i++) {
					if (tempVertex.x() == currentModel->pick_vertices[i * 3 + 0]
						&& tempVertex.y() == currentModel->pick_vertices[i * 3 + 1]
						&& tempVertex.z() == currentModel->pick_vertices[i * 3 + 2]) {
						currentModel->pick_vertices.erase(currentModel->pick_vertices.begin() + 3 * i, currentModel->pick_vertices.begin() + 3 * i + 3);
						deleteFlag = true;
					}
				}
				if (deleteFlag == false) {

					pickedItem = "vertex";
					currentModel->pick_vertices.push_back(tempVertex.x());
					currentModel->pick_vertices.push_back(tempVertex.y());
					currentModel->pick_vertices.push_back(tempVertex.z());
				}
			}

		}
		else if (currentModel->meshType == NURBSVOLUME) {
		if (pickType == PickPoint) {
			double pointMin = 99999;
			bool isPicked = false;
			QVector3D tempVertex;
			// 遍历一个面中所有的点，找出其中离射线最近的点

			for (int test_grid_id = 0; test_grid_id < currentModel->control_grid_cnt * 24; ++test_grid_id) {

				QVector3D point0(currentModel->vertices[test_grid_id * 6 + 0], currentModel->vertices[test_grid_id * 6 + 1], currentModel->vertices[test_grid_id * 6 + 2]);

				double dist = point2LineDis(point1, point2, point0);
				/*if (pointMin > dist) {
					pointMin = dist;
					tempVertex = point0;
				}*/
				QVector4D fDepth(point0, 1.f);
				fDepth = currentModel->modelMatrix * fDepth;
				if (dist < pointMin) {
					//if (zMax < fDepth[2]) {
						//zMax = fDepth[2];
					pointMin = fmin(pointMin, dist);
					tempVertex = point0;
					isPicked = true;
					//}
				}

			}

			std::cout << pointMin << std::endl;
			if (isPicked == false) return;

			pickedItemPosition[0] = tempVertex.x();
			pickedItemPosition[1] = tempVertex.y();
			pickedItemPosition[2] = tempVertex.z();
			bool deleteFlag = false;
			for (int i = 0; i < currentModel->pick_vertices.size() / 3; i++) {
				if (tempVertex.x() == currentModel->pick_vertices[i * 3 + 0]
					&& tempVertex.y() == currentModel->pick_vertices[i * 3 + 1]
					&& tempVertex.z() == currentModel->pick_vertices[i * 3 + 2]) {
					currentModel->pick_vertices.erase(currentModel->pick_vertices.begin() + 3 * i, currentModel->pick_vertices.begin() + 3 * i + 3);
					deleteFlag = true;
				}
			}
			if (deleteFlag == false) {

				pickedItem = "vertex";
				currentModel->pick_vertices.push_back(tempVertex.x());
				currentModel->pick_vertices.push_back(tempVertex.y());
				currentModel->pick_vertices.push_back(tempVertex.z());
			}
		}

		}
	}
	showInfo();
	update();
}

void ModelDraw::removeSelectedAll() {
	currentModel->pick_vertices.clear();
	pickedItemHandle = -1;
	showInfo();
	update();
}

double ModelDraw::point2LineDis(QVector3D point1, QVector3D point2, QVector3D point0) {

	//直线的法向量(p,q,r)
	QVector3D normal = (point1 - point2).normalized();

	//两个向量
	QVector3D normal01 = (point1 - point0).normalized();
	QVector3D normal02 = (point2 - point0).normalized();

	//进行叉乘
	double cross_x = normal01.y() * normal02.z() - normal02.y() * normal01.z();
	double cross_y = normal02.x() * normal01.z() - normal02.z() * normal01.x();
	double cross_z = normal01.x() * normal02.y() - normal01.y() * normal02.x();

	double cross_length = sqrt(cross_x * cross_x + cross_y * cross_y + cross_z * cross_z);
	double dx = (point2 - point1).x();
	double dy = (point2 - point1).y();
	double dz = (point2 - point1).z();

	double qb_length = sqrt(dx * dx + dy * dy + dz * dz);

	double ds = cross_length / qb_length;
	return ds;

}

bool ModelDraw::isIntersectTriangle(QVector3D orig, QVector3D end, QVector3D v0, QVector3D v1, QVector3D v2, QVector3D& intersection) {
	// Refer to https://www.cnblogs.com/graphics/archive/2010/08/09/1795348.html
	float t, u, v;
	QVector3D dir = end - orig;
	QVector3D E1 = v1 - v0;
	QVector3D E2 = v2 - v0;
	// 叉乘
	QVector3D P(dir.y() * E2.z() - E2.y() * dir.z(),
		E2.x() * dir.z() - E2.z() * dir.x(),
		dir.x() * E2.y() - dir.y() * E2.x());
	// 点乘
	float det = E1.x() * P.x() + E1.y() * P.y() + E1.z() * P.z();
	QVector3D T;
	if (det > 0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}
	if (det < 0)
		return false;

	// Calculate u and make sure u <= 1
	u = T.x() * P.x() + T.y() * P.y() + T.z() * P.z();
	if (u < 0.0f || u > det)
		return false;

	// Q
	QVector3D Q(T.y() * E1.z() - E1.y() * T.z(),
		E1.x() * T.z() - E1.z() * T.x(),
		T.x() * E1.y() - T.y() * E1.x());

	// Calculate v and make sure u + v <= 1
	v = dir.x() * Q.x() + dir.y() * Q.y() + dir.z() * Q.z();
	if (v < 0.0f || u + v > det)
		return false;

	//// Calculate t, scale parameters, ray intersects triangle
	//t = E2.x() * Q.x() + E2.y() * Q.y() + E2.z() * Q.z();
	float fInvDet = 1.0f / det;
	//t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;
	intersection = u * E1 + v * E2 + v0;
	return true;
}

void ModelDraw::PickItemTranslate() {
	// 此功能只是试用，不会继续修改完善
	


	if (currentModel->structure == NURBS) {
		if (currentModel->pick_vertices.size() == 0) return;



		if (pickType == PickPoint) {
			int pick_num = currentModel->pick_vertices.size();
			QVector4D QpickPoint(currentModel->pick_vertices[pick_num - 3], currentModel->pick_vertices[pick_num - 2], currentModel->pick_vertices[pick_num - 1], 1);
			QVector4D OriPoint(QpickPoint);
			QpickPoint = pickItemMoveMatrix * QpickPoint;

			// 更新点选渲染的位置，让红色的点也跟着被拖拽的点一起移动

			currentModel->pick_vertices[pick_num - 3] = QpickPoint.x();
			currentModel->pick_vertices[pick_num - 2] = QpickPoint.y();
			currentModel->pick_vertices[pick_num - 1] = QpickPoint.z();

			for (auto& patch : currentModel->nurbs_patchs.m_Geometry) {
				for (auto& C : patch->m_ControlPoints) {
					double xx = (C[0] - centerX) / init_max_gap * 2;
					double yy = (C[1] - centerY) / init_max_gap * 2;
					double zz = (C[2] - centerZ) / init_max_gap * 2;

					if (fabs(xx - OriPoint.x()) + fabs(yy - OriPoint.y()) + fabs(zz - OriPoint.z()) < 1e-5) {
						C[0] = (QpickPoint.x()) / 2 * init_max_gap + centerX;
						C[1] = (QpickPoint.y()) / 2 * init_max_gap + centerY;
						C[2] = (QpickPoint.z()) / 2 * init_max_gap + centerZ;
					}
				}
			}

		}
	}
	updateCurrentMesh();
}

void ModelDraw::pick_point_with_depth(float x, float y, double(&p)[3], double itemPosition[3]) {
	if (!currentModel) return;
	QVector4D point(x, y, 0.9, 1.0);		// point为鼠标点击的位置
	QVector4D toward(x, y, -1, 1.0);	// 指向toward与投影矩阵的fov相关
	QVector4D pointView(0, 0, 1, 1.0);
	QVector4D pointEnd = point + 2 * toward;
	QVector4D tpoint = currentModel->modelMatrix.inverted() * viewMatrix.inverted() * projectionMatrix.inverted() * point;
	QVector4D tpointEnd = currentModel->modelMatrix.inverted() * viewMatrix.inverted() * projectionMatrix.inverted() * pointEnd;
	QVector4D tpointView = currentModel->modelMatrix.inverted() * viewMatrix.inverted() * projectionMatrix.inverted() * pointView;
	// point1为射线起点，point2为射线终点
	QVector3D point1(tpoint.x() / tpoint.w(), tpoint.y() / tpoint.w(), tpoint.z() / tpoint.w());
	QVector3D point2(tpointEnd.x() / tpointEnd.w(), tpointEnd.y() / tpointEnd.w(), tpointEnd.z() / tpointEnd.w());
	QVector3D faceNormal(tpointView.x() / tpointView.w(), tpointView.y() / tpointView.w(), tpointView.z() / tpointView.w());

	// 射线向量
	double e[3]{ point2.x() - point1.x(),point2.y() - point1.y(), point2.z() - point1.z() };
	// 射线起点
	double PL[3]{ point1.x(),point1.y(), point1.z() };
	// 平面向量
	double q[3]{ itemPosition[0] - faceNormal.x(),itemPosition[1] - faceNormal.y(), itemPosition[2] - faceNormal.z() };

	double t = (q[0] * (itemPosition[0] - PL[0]) + q[1] * (itemPosition[1] - PL[1]) + q[2] * (itemPosition[2] - PL[2])) / (q[0] * e[0] + q[1] * e[1] + q[2] * e[2]);
	p[0] = PL[0] + e[0] * t;
	p[1] = PL[1] + e[1] * t;
	p[2] = PL[2] + e[2] * t;

	currentModel->pick_vertices.push_back(p[0]);
	currentModel->pick_vertices.push_back(p[1]);
	currentModel->pick_vertices.push_back(p[2]);
}


/*=====================================渲染操作=========================================*/

void ModelDraw::DrawScene(void) {
	DrawBackGround();
	DrawSceneModel();
	drawPickItem();
	//drawCutLine();
	DrawCoordCircle();
}

void ModelDraw::DrawBackGround(void) {
	// 渐变背景，其实是一个长方形面片，绘制在最底下
	std::vector<GLfloat> back_ground_vertices{
		// 顶点位置            // 坐标颜色
		1.0f, 1.0f, 0.0f,   0.8f, 0.8f, 0.8f,   // 右上角
		-1.0f, -1.0f, 0.0f, 0.2f, 0.2f, 0.2f,   // 左下角
		1.0f, -1.0f, 0.0f,  0.2f, 0.2f, 0.2f,   // 右下角
		-1.0f, 1.0f, 0.0f,  0.8f, 0.8f, 0.8f,    // 左上角
		-1.0f, -1.0f, 0.0f, 0.2f, 0.2f, 0.2f,   // 左下角
		1.0f, 1.0f, 0.0f,   0.8f, 0.8f, 0.8f    // 右上角
	};

	// 使用单位化矩阵赋值给模型、视角、投影矩阵，目的是为了旋转缩放等操作不影响背景的改变
	QMatrix4x4 identityMatrix;
	shaderProgram.setUniformValue("model", identityMatrix);	// 模型矩阵
	shaderProgram.setUniformValue("view", identityMatrix); // 视角矩阵
	shaderProgram.setUniformValue("projection", identityMatrix); // 投影矩阵
	shaderProgram.setUniformValue("colormode", 4);

	// 释放旧数据的内存
	core->glDeleteVertexArrays(1, &bVAO);
	core->glDeleteBuffers(1, &bVBO);
	// 1.新建顶点数组对象
	core->glGenVertexArrays(1, &bVAO);
	core->glGenBuffers(1, &bVBO);
	// 2. 绑定顶点数组对象
	core->glBindVertexArray(bVAO);
	// 3. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	core->glBindBuffer(GL_ARRAY_BUFFER, bVBO);
	core->glBufferData(GL_ARRAY_BUFFER, back_ground_vertices.size() * sizeof(GLfloat), back_ground_vertices.data(), GL_STATIC_DRAW);
	// 4. 设定顶点属性指针
	// 顶点坐标传入VBO，location=0
	core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	core->glEnableVertexAttribArray(0);
	// 顶点法向传入VBO，location=1
	core->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	core->glEnableVertexAttribArray(1);
	core->glBindVertexArray(0);
	core->glBindBuffer(GL_ARRAY_BUFFER, 0);

	core->glBindVertexArray(bVAO);	// 前面已经记录了VBO和EBO现在只需一步调用即可
	core->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	core->glDepthRange(0.9999, 1);
	core->glDrawArrays(GL_TRIANGLES, 0, back_ground_vertices.size() / 6);
	core->glDepthRange(0, 1);

}

void ModelDraw::DrawCoordCircle(void) {
	if (!currentModel) return;

	coord_vertices.clear();

	float R = 1.2f;
	int n = 100;     //这里的n表示用多边形绘制圆的精度，可以考虑增大精度

	core->glLineWidth(1);// 线宽
	// 圆形坐标
	for (int i = 0; i < n; i++)     //通过数学计算来画多边形的点
	{
		coord_vertices.push_back(R * cos(2 * M_PI * i / n));
		coord_vertices.push_back(R * sin(2 * M_PI * i / n));
		coord_vertices.push_back(0);
	}

	for (int i = 0; i < n; i++)
	{
		coord_vertices.push_back(R * cos(2 * M_PI * i / n));
		coord_vertices.push_back(0);
		coord_vertices.push_back(R * sin(2 * M_PI * i / n));
	}

	for (int i = 0; i < n; i++)
	{
		coord_vertices.push_back(0);
		coord_vertices.push_back(R * cos(2 * M_PI * i / n));
		coord_vertices.push_back(R * sin(2 * M_PI * i / n));
	}

	// 十字坐标
	coord_vertices.push_back(0);
	coord_vertices.push_back(0);
	coord_vertices.push_back(1.2f * R);
	coord_vertices.push_back(0);
	coord_vertices.push_back(0);
	coord_vertices.push_back(-1.2f * R);

	coord_vertices.push_back(0);
	coord_vertices.push_back(1.2f * R);
	coord_vertices.push_back(0);
	coord_vertices.push_back(0);
	coord_vertices.push_back(-1.2f * R);
	coord_vertices.push_back(0);

	coord_vertices.push_back(1.2f * R);
	coord_vertices.push_back(0);
	coord_vertices.push_back(0);
	coord_vertices.push_back(-1.2f * R);
	coord_vertices.push_back(0);
	coord_vertices.push_back(0);


	shaderProgram.setUniformValue("model", viewRotateMatrix);	// 把模型矩阵传入shader
	shaderProgram.setUniformValue("colormode", 3);

	// 释放旧数据的内存
	core->glDeleteVertexArrays(1, &cVAO);
	core->glDeleteBuffers(1, &cVBO);
	// 1.新建顶点数组对象
	core->glGenVertexArrays(1, &cVAO);
	core->glGenBuffers(1, &cVBO);
	// 2. 绑定顶点数组对象
	core->glBindVertexArray(cVAO);
	// 3. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	core->glBindBuffer(GL_ARRAY_BUFFER, cVBO);
	core->glBufferData(GL_ARRAY_BUFFER, coord_vertices.size() * sizeof(GLfloat), coord_vertices.data(), GL_STATIC_DRAW);
	// 4. 设定顶点属性指针
	// 顶点坐标传入VBO，location=0
	core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	core->glEnableVertexAttribArray(0);
	core->glBindVertexArray(0);
	core->glBindBuffer(GL_ARRAY_BUFFER, 0);
	core->glBindVertexArray(cVAO);	// 前面已经记录了VBO和EBO现在只需一步调用即可

	if (coordCircle) {
		shaderProgram.setUniformValue("color", QVector4D(0.0, 0.9, 0.9, 0.6));
		core->glDrawArrays(GL_LINE_LOOP, 0, n);
		shaderProgram.setUniformValue("color", QVector4D(1.0, 0.84, 0.0, 0.6));
		core->glDrawArrays(GL_LINE_LOOP, n, n);
		shaderProgram.setUniformValue("color", QVector4D(0.8, 0.0, 0.0, 0.6));
		core->glDrawArrays(GL_LINE_LOOP, 2 * n, n);
	}
	if (ModelMoveState) {
		shaderProgram.setUniformValue("color", QVector4D(0.0, 0.9, 0.9, 0.6));
		core->glDrawArrays(GL_LINES, 3 * n, 2);
		shaderProgram.setUniformValue("color", QVector4D(1.0, 0.84, 0.0, 0.6));
		core->glDrawArrays(GL_LINES, 3 * n + 2, 2);
		shaderProgram.setUniformValue("color", QVector4D(0.8, 0.0, 0.0, 0.6));
		core->glDrawArrays(GL_LINES, 3 * n + 4, 2);
	}
}

void ModelDraw::DrawStyle(void) {
	switch (drawMode) {
	case POINTS:
		core->glPointSize(4);
		core->glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		shaderProgram.setUniformValue("rendermode", 1);
		break;
	case FLATLINES:
		core->glLineWidth(1);// 线宽
		core->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shaderProgram.setUniformValue("rendermode", 2);
		break;
	case FILL:
		core->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		shaderProgram.setUniformValue("rendermode", 3);
		break;
	case FLATFILL:
		core->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		shaderProgram.setUniformValue("rendermode", 4);
		break;
	default:
		break;
	}
}

void ModelDraw::ColorStyle(void) {
	switch (colorMode) {
	case LIGHT:
		shaderProgram.setUniformValue("colormode", 1);
		break;
	case NORMAL:
		shaderProgram.setUniformValue("colormode", 2);
		break;
	default:
		break;
	}
}

void ModelDraw::loadShader() {
	if (!currentModel) return;

	// 相关数据传入shader
	shaderProgram.setUniformValue("model", viewRotateMatrix * currentModel->modelMatrix);	// 把模型矩阵传入shader
	shaderProgram.setUniformValue("view", viewMatrix); // 视角矩阵
	shaderProgram.setUniformValue("projection", projectionMatrix); // 投影矩阵
	shaderProgram.setUniformValue("rotate", viewRotateMatrix * currentModel->rotateMatrix); // 旋转矩阵（法向量专用）
	shaderProgram.setUniformValue("viewpos", 0.0f, 0.0f, 1.0f);  // 视角方向
	shaderProgram.setUniformValue("light.position", 0.0f, 0.0f, 1.0f);  // 光照方向
	shaderProgram.setUniformValue("light.ambient", 0.4f, 0.4f, 0.4f);	// 环境光照系数
	shaderProgram.setUniformValue("light.diffuse", 0.5f, 0.5f, 0.5f);	// 漫反射系数
	shaderProgram.setUniformValue("light.specular", 0.5f, 0.5f, 0.5f);	// 镜面反射系数
	shaderProgram.setUniformValue("shininess", 32.0f);

	// 释放旧数据的内存
	core->glDeleteVertexArrays(1, &currentModel->VAO);
	core->glDeleteBuffers(1, &currentModel->VBO);
	// 1.新建顶点数组对象
	core->glGenVertexArrays(1, &currentModel->VAO);
	core->glGenBuffers(1, &currentModel->VBO);
	// 2. 绑定顶点数组对象
	core->glBindVertexArray(currentModel->VAO);
	// 3. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	core->glBindBuffer(GL_ARRAY_BUFFER, currentModel->VBO);
	core->glBufferData(GL_ARRAY_BUFFER, currentModel->vertices.size() * sizeof(GLfloat), currentModel->vertices.data(), GL_STATIC_DRAW);
	// 4. 设定顶点属性指针
	// 顶点坐标传入VBO，location=0
	core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	core->glEnableVertexAttribArray(0);
	// 顶点法向传入VBO，location=1
	core->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	core->glEnableVertexAttribArray(1);
	core->glBindVertexArray(0);
	core->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModelDraw::drawTriMesh() {
	if (!currentModel) return;
	if (faceCull) core->glEnable(GL_CULL_FACE);
	else core->glDisable(GL_CULL_FACE);
	core->glBindVertexArray(currentModel->VAO);	// 前面已经记录了VBO和EBO现在只需一步调用即可
	core->glDrawArrays(GL_TRIANGLES, 0, currentModel->vertices.size() / 6);

	// 表面网格
	if (surfaceGrid) {
		shaderProgram.setUniformValue("rendermode", 2);
		shaderProgram.setUniformValue("colormode", 3);
		if (isCurrentModel) {
			shaderProgram.setUniformValue("color", QVector4D(0.1, 0.1, 0.1, 1.0));
			//isCurrentModel = false;
		}
		else {
			shaderProgram.setUniformValue("color", QVector4D(0.5, 0.5, 0.5, 1.0));
		}
		core->glLineWidth(1);
		core->glDepthRange(0, 0.999999);	// 把线框位置深度调近，防止面片盖住线框
		core->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// 线框
		core->glBindVertexArray(currentModel->VAO);
		core->glDrawArrays(GL_TRIANGLES, 0, currentModel->vertices.size() / 6);
		core->glDepthRange(0, 1);	// 画完线框后改回原本深度
	}
}




void ModelDraw::drawNurbsCurve()
{
	
	if (!currentModel) return;
	if (faceCull) core->glEnable(GL_CULL_FACE);
	else core->glDisable(GL_CULL_FACE);

	core->glBindVertexArray(currentModel->VAO);


	shaderProgram.setUniformValue("rendermode", 2);
	shaderProgram.setUniformValue("colormode", 3);
	core->glDepthRange(0, 0.999999);

	shaderProgram.setUniformValue("color", QVector4D(1.0, 0, 0, 1.0));
	core->glPointSize(10);

	core->glDrawArrays(GL_POINTS, 0, currentModel->control_interval_cnt); // 画控制点
	core->glPointSize(1);

	core->glLineWidth(2);
	core->glDrawArrays(GL_LINE_STRIP, 0, currentModel->control_interval_cnt);

	shaderProgram.setUniformValue("color", QVector4D(0.0, 1.0, 0, 1.0));
	core->glLineWidth(2);
	core->glDrawArrays(GL_LINE_STRIP, currentModel->control_interval_cnt, currentModel->all_sample_interval_cnt); // 画线 // 画控制点
	core->glLineWidth(1);
	core->glDepthRange(0, 1);
}


void ModelDraw::drawNurbsVolume()
{
	if (!currentModel) return;
	if (faceCull) core->glEnable(GL_CULL_FACE);
	else core->glDisable(GL_CULL_FACE);

	core->glBindVertexArray(currentModel->VAO);
	core->glDrawArrays(GL_TRIANGLES, currentModel->control_grid_cnt * 24, currentModel->vertices.size() / 6 - currentModel->control_grid_cnt * 24);

	shaderProgram.setUniformValue("rendermode", 2);
	shaderProgram.setUniformValue("colormode", 3);
	core->glDepthRange(0, 0.999999);

	core->glPointSize(5);
	shaderProgram.setUniformValue("color", QVector4D(1.0, 0, 0, 1.0));
	core->glDrawArrays(GL_POINTS, 0, currentModel->control_grid_cnt * 24); // 画控制点
	core->glPointSize(1);

	shaderProgram.setUniformValue("color", QVector4D(0, 0, 0, 1.0));
	core->glLineWidth(1);

	core->glDrawArrays(GL_LINES, 0, currentModel->control_grid_cnt * 24);

	core->glLineWidth(1);
	core->glDepthRange(0, 1);
}


void ModelDraw::drawNurbsSurface() {
	if (!currentModel) return;
	if (faceCull) core->glEnable(GL_CULL_FACE);
	else core->glDisable(GL_CULL_FACE);
	core->glBindVertexArray(currentModel->VAO);	// 前面已经记录了VBO和EBO现在只需一步调用即可
	core->glDrawArrays(GL_TRIANGLES, currentModel->control_face_cnt * 6, currentModel->vertices.size() / 6 - currentModel->control_face_cnt * 6);

	// 控制网格
	{
		shaderProgram.setUniformValue("rendermode", 2);
		shaderProgram.setUniformValue("colormode", 3);
		if (isCurrentModel) {
			shaderProgram.setUniformValue("color", QVector4D(0.1, 0.1, 0.1, 1.0));
			//isCurrentModel = false;
		}
		else {
			shaderProgram.setUniformValue("color", QVector4D(0.5, 0.5, 0.5, 1.0));
		}
		core->glPointSize(10);
		core->glLineWidth(2);


		core->glDepthRange(0, 0.999999);	// 把线框位置深度调近，防止面片盖住线框
		core->glBindVertexArray(currentModel->VAO);
		for (int i = 0; i < currentModel->control_face_cnt; i++) {
			core->glDrawArrays(GL_LINE_LOOP, i * 6, 4);
		}
		shaderProgram.setUniformValue("color", QVector4D(1.0, 0., 0., 1.0));

		for (int i = 0; i < currentModel->control_face_cnt; i++)
		{

			core->glDrawArrays(GL_POINTS, i * 6 + 0, 1);
			core->glDrawArrays(GL_POINTS, i * 6 + 1, 1);
			core->glDrawArrays(GL_POINTS, i * 6 + 2, 1);
			core->glDrawArrays(GL_POINTS, i * 6 + 3, 1);
		}
		shaderProgram.setUniformValue("color", QVector4D(0.5, 0.5, 0.5, 1.0));
		core->glDepthRange(0, 1);	// 画完线框后改回原本深度
	}
}


void ModelDraw::DrawSceneModel(void) {
	if (!currentModel) return;
	tempModel = currentModel;
	tempModelID = currentModelID;

	// 对于所有模型，挨个渲染一次
	for (auto mp : modelList) {
		currentModel = mp.second;
		currentModelID = mp.first;
		if (currentModelID == tempModelID) isCurrentModel = true;
		DrawStyle();
		ColorStyle();
		loadShader();

		switch (currentModel->meshType) {
		case NURBSSURFACE:
			drawNurbsSurface();
			break;
		case NURBSCURVE:
			drawNurbsCurve();
			break;
		case NURBSVOLUME:
			drawNurbsVolume();
			break;
		default:
			break;
		}
		isCurrentModel = false;
	}
	currentModel = tempModel;
	currentModelID = tempModelID;

}

void ModelDraw::drawPickItem() {
	if (!currentModel) return;
	shaderProgram.setUniformValue("colormode", 3);
	shaderProgram.setUniformValue("color", QVector4D(0.8, 0.0, 0.0, 0.6));
	tempModel = currentModel;
	tempModelID = currentModelID;
	// 对于每个模型的点选信息，挨个渲染一次
	for (auto mp : modelList) {
		currentModel = mp.second;
		currentModelID = mp.first;
		drawPickPoint();
		core->glBindVertexArray(currentModel->pVAO);	// 前面已经记录了VBO和EBO现在只需一步调用即可

		if (currentModel->structure == NURBS) {
			core->glPointSize(15);
			core->glDepthRange(0, 0.9999);
			core->glDrawArrays(GL_POINTS, 0, currentModel->pick_vertices.size() / 3);
		}
		else {
			core->glPointSize(7);
			core->glLineWidth(3);
			core->glDepthRange(0, 0.9999);
			switch (pickType) {
			case PickPoint: {
				core->glDrawArrays(GL_POINTS, 0, currentModel->pick_vertices.size() / 3);
				break;
			}
			case PickEdge: {
				core->glDrawArrays(GL_LINES, 0, currentModel->pick_vertices.size() / 3);
				break;
			}
			case PickFace: {
				core->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				core->glDrawArrays(GL_TRIANGLES, 0, currentModel->pick_vertices.size() / 3);
				break;
			}
			case PickCell: {
				break;
			}
			}
		}

		core->glDrawArrays(GL_POINTS, 0, currentModel->pick_vertices.size() / 3);
		core->glDepthRange(0, 1);
	}
	currentModel = tempModel;
	currentModelID = tempModelID;

}

/*=====================================鼠标绘制事件=========================================*/

void ModelDraw::mousePressEvent(QMouseEvent* _event) {
	if (!currentModel) return;
	switch (editModel) {
	case MODELVIEW: case DEFORMATION: {
		lastpoint2 = _event->pos();
		//makeCurrent();
		lastpointok = MapToSphere(lastpoint2, lastpoint3);
		mouseMode = _event->button();
		//update();
		break;
	}
	case PICKITEM: {
		if (_event->button() == Qt::LeftButton) {
			float winX = (float)(_event->x() * 2 - width()) / (width());	// 窗口坐标，范围在[-1,1]之内
			float winY = -(float)(_event->y() * 2 - height()) / (height());	// 窗口坐标，范围在[-1,1]之内
			pickPoint(winX, winY);
		}
		//else if (_event->button() == Qt::RightButton) { // 鼠标右键单击绘制当前选点同深度的点
		//    float winX = (float)(_event->x() * 2 - width()) / (width());	// 窗口坐标，范围在[-1,1]之内
		//    float winY = -(float)(_event->y() * 2 - height()) / (height());	// 窗口坐标，范围在[-1,1]之内
		//    double p[3];
		//    pick_point_with_depth(winX, winY, p, pickedItemPosition);
		//    std::cout << p[0] << " " << p[1] << " " << p[2] << std::endl;
		//}
		lastpoint2 = _event->pos();
		makeCurrent();
		lastpointok = MapToSphere(lastpoint2, lastpoint3);
		mouseMode = _event->button();
		update();
		break;
	}
	case MODELCUT: {
		lineBeginPoint = QVector2D((double)(_event->x() * 2 - width()) / (width()), -(double)(_event->y() * 2 - height()) / (height()));
		makeCurrent();
		lastpointok = true;
		update();
		break;
	}
	}
}

void ModelDraw::mouseMoveEvent(QMouseEvent* _event) {
	if (!currentModel) return;
	switch (editModel) {
	case MODELVIEW: {
		QPoint newPoint2D = _event->pos();
		this->setCursor(Qt::SizeAllCursor);
		//makeCurrent();
		if (lastpointok) {
			switch (mouseMode) {
				// 鼠标左键旋转模型
			case Qt::LeftButton:
				ModelMoveState = true;
				Rotation(newPoint2D);
				break;
				// 鼠标中键移动模型
			case Qt::MiddleButton:
				ModelMoveState = true;
				Translation(newPoint2D);
				break;
				// 鼠标右键移动视角
			case Qt::RightButton:
				ModelMoveState = true;
				ViewRotate(newPoint2D);
				break;
			default:
				break;
			}
		}
		lastpoint2 = newPoint2D;
		lastpointok = MapToSphere(lastpoint2, lastpoint3);
		//update();
		break;
	}
	case PICKITEM: {
		this->setCursor(Qt::PointingHandCursor);
		QPoint newPoint2D = _event->pos();
		makeCurrent();
		if (lastpointok) {
			// 可以鼠标右键拖拽来实现PickItem位置的移动（此功能只是试用）
			if (mouseMode == Qt::RightButton) {
				PickItemTranslation(newPoint2D);	// 根据点的位置
				PickItemTranslate();
			}
		}
		lastpoint2 = newPoint2D;
		lastpointok = MapToSphere(lastpoint2, lastpoint3);
		update();
		break;
	}
	case MODELCUT: {
		lineEndPoint = QVector2D((double)(_event->x() * 2 - width()) / (width()), -(double)(_event->y() * 2 - height()) / (height()));
		this->setCursor(Qt::CrossCursor);
		makeCurrent();
		if (lastpointok) {
			//calcCutLine(lineBeginPoint, lineEndPoint);
		}
		//update();
		break;
	}
	case DEFORMATION: {// 实时渲染变形结果
		// deformation相关的代码已移除
		break;
	}
	default:
		break;
	}
	update();
}

void ModelDraw::mouseReleaseEvent(QMouseEvent* _event) {
	//assert(mouseMode < N_MOUSE_MODES);
	mouseMode = Qt::NoButton;
	lastpointok = false;
	ModelMoveState = false;
	update();
}

void ModelDraw::wheelEvent(QWheelEvent* _event) {
	if (!currentModel) return;
	// 0.03代表缩放速度
	double d = -_event->delta() / 120.0 * 0.03 * radius;
	float scale = 1 + d;
	currentModel->modelScale *= scale;
	ModelScale(Vector3d(scale, scale, scale));
	update();
}

void ModelDraw::keyPressEvent(QKeyEvent* _event) {
	switch (_event->key()) {
	case Qt::Key_Escape:
		qApp->quit();
		break;
	}
	_event->ignore();
}

void ModelDraw::keyReleaseEvent(QKeyEvent* _event) {
	_event->ignore();
}

void ModelDraw::LoadRotation(void) {
	LoadCopyModelViewMatrix();
	update();
}

void ModelDraw::ResetView(void) {
	ResetModelviewMatrix();
	viewCenter();
	update();
}

void ModelDraw::viewCenter() {
	update();
}

void ModelDraw::PickItemTranslation(const QPoint& p) {
	pickItemMoveMatrix.setToIdentity();
	double z = -(viewMatrix(2, 3) / viewMatrix(2, 2));		// 右键拖动的距离与模型离视角的远近进行绑定（使拖动更流畅）

	double w = width(); double h = height();
	double aspect = w / h;
	double near_plane = 0.01 * radius;
	double top = tan(45.0 / 2.0f * M_PI / 180.0f) * near_plane;
	double right = aspect * top;

	double dx = p.x() - lastpoint2.x();
	double dy = p.y() - lastpoint2.y();

	QVector4D trans(2.0 * dx / w * right / near_plane * z, -2.0 * dy / h * top / near_plane * z, 0.0f, 1);
	trans = currentModel->rotateMatrix.inverted() * trans;
	std::cout << trans[0] << " " << trans[1] << " " << trans[2] << " " << std::endl;
	pickItemMoveMatrix.translate(trans[0], trans[1], trans[2]);
}

void ModelDraw::Translate(const Vector3d& _trans) {
	float x = _trans[0];
	float y = _trans[1];
	float z = _trans[2];
	QVector4D trans = QVector4D{ x, y, z, 1.0f };
	trans = currentModel->rotateMatrix.inverted() * viewRotateMatrix.inverted() * trans;
	float scale = currentModel->modelScale;
	currentModel->modelMatrix.translate(trans[0] / scale, trans[1] / scale, trans[2] / scale);
}

void ModelDraw::ModelScale(const Vector3d& _scale) {
	currentModel->modelMatrix.scale(_scale[0], _scale[1], _scale[2]);
}

void ModelDraw::Rotate(const Vector3d& _axis, const double& _angle) {
	QMatrix4x4 tempMatrix;
	QMatrix4x4 tempRotateMatrix;
	// 由于有些模型移动之后的center并不在坐标原点，需要把center先移到坐标原点再旋转
	tempMatrix.translate(currentModel->modelMatrix(0, 3), currentModel->modelMatrix(1, 3), currentModel->modelMatrix(2, 3));
	QVector4D view(_axis[0], _axis[1], _axis[2], 1);
	view = viewRotateMatrix.inverted() * view;
	tempMatrix.rotate(_angle, view.x(), view.y(), view.z());
	tempRotateMatrix.rotate(_angle, view.x(), view.y(), view.z());
	// 再把模型移回原坐标
	tempMatrix.translate(-currentModel->modelMatrix(0, 3), -currentModel->modelMatrix(1, 3), -currentModel->modelMatrix(2, 3));
	currentModel->modelMatrix = tempMatrix * currentModel->modelMatrix;		// 左乘的方式，使动态欧拉角改成静态欧拉角，解决方向旋转的问题
	currentModel->rotateMatrix = tempRotateMatrix * currentModel->rotateMatrix;	// rotateMatrix是用来储存旋转状态的（没有translate），模型法向量的旋转需要用到此矩阵
}


