#pragma once
/* 模型渲染类，继承于ViewOpenGL类，传入模型对象，处理模型 */
#include <QtCore/QString>
#include <QtCore/qstring.h>
#include <vector>
#if __linux__
#include <QtGui>
#else
#include <QtGUI/QtGui>
#endif
#include <QtWidgets/qfiledialog.h>
#include "ViewOpenGL.h"
#include <Eigen/Dense>
#include <Eigen/Core>
#include "NurbsSDK/MultiGeo.h"
#include "NurbsSDK/FileUtil.h"

class ModelDraw :public ViewOpenGL {
	Q_OBJECT
public:
	ModelDraw(QWidget* parent = 0);
	virtual ~ModelDraw(void);

protected:

    // 模型结构体
    struct Model{
        std::string fileName;   // 文件名
        Structure structure;    // 结构类型：表面网格、体网格、样条
        MeshType meshType;      // 
		MultiGeo nurbs_patchs;     //样条的集合
		//控制面和采样面数目
		int control_face_cnt;
		long long all_sample_face_cnt;
		//曲线
		int control_interval_cnt;
		long long all_sample_interval_cnt;
		//曲体
		int control_grid_cnt;
		long long all_sample_grid_cnt;
        QMatrix4x4 modelMatrix; // 模型矩阵
        QMatrix4x4 rotateMatrix; // 旋转矩阵
        float modelScale;   // 模型缩放比例
        std::vector<GLfloat> vertices;  // 存贮模型的顶点数据，用于传递到显存中进行渲染
        std::vector<GLfloat> pick_vertices; // 存储点选数据
        GLuint VAO, VBO;  // 顶点VAO、VBO
        GLuint pVAO, pVBO;  // 点选VAO、VBO
    };
    // 模型列表和当前模型指针
    std::map<int, Model*> modelList;
    Model* currentModel = nullptr;  // 此指针指用来指向当前的模型
    Model* tempModel = nullptr;
    int currentNewID = -1;
    int currentModelID = -1;
    int tempModelID = -1;
    bool isCurrentModel = false;

    QString file_suffix;    // 文件名后缀

    GLuint cVAO = 0, cVBO = 0;	// 坐标系VAO、VBO
    GLuint bVAO = 0, bVBO = 0;  // 背景VAO、VBO
    std::vector<GLfloat> coord_vertices;	// 坐标系和圆
	std::vector<GLfloat> cutline_vertices;	// 存储切割线顶点数据
	

	std::vector<QVector3D> cutVectors;	// 切割平面的两个法向量
	

	// 线与点的距离
	double point2LineDis(QVector3D point1, QVector3D point2, QVector3D point0);
	// 判断线是否穿过三角形
	bool isIntersectTriangle(QVector3D orig, QVector3D end, QVector3D v0, QVector3D v1, QVector3D v2, QVector3D& intersection);

    // 显示当前模型数据
	std::function<void(void)> showInfo;
	std::function<void(double, double, double)> updatePositionText;
	

public:

    void setShowInfo(std::function<void(void)> info) {
        showInfo = info;
    }

	void setUpdatePositionText(std::function<void(double, double, double)> updatePositionText) {
		this->updatePositionText = updatePositionText;
	}


	bool isMeshValid() {
		if (!currentModel) return false;
		return true;
	}

	
	
	inline bool hasMesh() {
		return currentModel != nullptr;
	}



	void saveFlieName(std::string fname) {
        currentModel->fileName = fname;
	}

	std::string getFileName() {
		return currentModel->fileName;
	}

    // 当前点选的ID
	int pickedItemHandle = -1;
	std::string pickedItem;
	int getPickedItemHandle(std::string& item) {
		if (!currentModel) return -1;
		item = pickedItem;
		return pickedItemHandle;
	};

    // 点选、切割相关
    void drawPickItem();
    void drawPickPoint();
    void drawCutLine();
    void updateCutMesh();

    // 模型操作相关（加载、保存、删除）
	void changeCurrentMeshToNext();
	void changeCurrentMeshToLast();
    void createModel();
    void deleteModel();
    void clearMesh();
    bool ConvertMeshToHedra();
	bool loadNurbsGeo(const std::string& filename);
	void creatNurbs(const std::vector<int>& control_points_size, const std::vector<int>& degree_size);
	bool saveNurbsGeo(const std::string& filename);



	inline bool copyMesh() {
		if (!currentModel) return false;
        
        return true;
	}
	inline bool recoverMesh() {
       
        return true;
	}

	inline bool isTetMesh() {
		return currentModel->structure == VOLUMEMESH;
	}

public:

	void viewCenter();// 视点中心
	void LoadRotation(void);
	void ResetView(void);// 重置视图

	// 不同的编辑模式，供鼠标事件选择
	enum EditModel {
		MODELVIEW, PICKITEM, MODELCUT, DEFORMATION
	};
	EditModel editModel;
	void setEditModel(const EditModel& editmodel) {
		editModel = editmodel;
		switch (editModel) {
		case MODELVIEW: {
			this->setCursor(Qt::SizeAllCursor);
			break;
		}
		case PICKITEM: {
			this->setCursor(Qt::PointingHandCursor);
			break;
		}
		case MODELCUT: {
			this->setCursor(Qt::CrossCursor);
			break;
		}
		case DEFORMATION: {
			this->setCursor(Qt::CrossCursor);
			break;
		}
		}
	}
	EditModel getEditModel() {
		return editModel;
	}
	// 点选类型（点、线、面、体）
	enum PickType {
		PickPoint, PickEdge, PickFace, PickCell
	};
	PickType pickType;
	void setPickType(const PickType& pickmode) {
		pickType = pickmode;
	}
	PickType getPickType() {
		return pickType;
	}
	void removeSelectedAll();

    // 渲染开关
	bool faceCull = false;  // 面剔除开关
	bool surfaceGrid = true;    //表面网格开关
	bool coordCircle = true;    //坐标圈开关

	void changeSurfaceGrid() {
		surfaceGrid = !surfaceGrid;
		update();
	}
	void changeFaceCull() {
		faceCull = !faceCull;
		update();
	}
	void changeCoordCircle() {
		coordCircle = !coordCircle;
		update();
	}

    // 切割相关
	bool flag_CutPlane = false;
	double cutX = 0.f, cutY = 0.f, cutZ = 0.f;// 0 全部显示，1 全部不显示
	double minX, minY, minZ, maxX, maxY, maxZ;	// 模型坐标轴下的最大最小值
	double init_max_gap;
	double centerX =0.0, centerY = 0.0, centerZ = 0.0, radius = 1.0f;	// 模型中心点和半径
    void updateCutFlag(bool flag) {
        flag_CutPlane = flag;
        if (flag_CutPlane) updateCutMesh();
        else updateCurrentMesh();
    }

protected:

	void DrawScene();// 绘制模型
	void DrawCoordCircle();// 绘制坐标和圆形
    void DrawBackGround();
	void DrawSceneModel(void);// 传入模型
	void DrawStyle(void);// 绘制样式：点、线、面片、平滑面
	void ColorStyle(void);// 着色样式：模拟光照着色、根据点法向着色
    // 渲染相关
    void updateMeshModel();
    void updateCurrentMesh();
    void loadShader();
    void drawTriMesh();
    void drawQuadMesh();
	void drawNurbsSurface();
	void drawNurbsCurve();
	void drawNurbsVolume();

	// 点选
	void pickPoint(double x, double y);
	void pick_point_with_depth(float x, float y, double (&p)[3], double itemPosition[3]);
    QVector2D lineBeginPoint;
    QVector2D lineEndPoint;
    // 点选物体的中心坐标
    double pickedItemPosition[3];
	// 切割
	//void calcCutLine(QVector2D begin, QVector2D end);
	void PickItemTranslate();

	//重写鼠标事件，增加点击操作
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);
    // 重写模型旋转、平移、缩放操作
    void Translate (const Vector3d &trans);
    void Rotate (const Vector3d &axis,const double &angle);
    void ModelScale(const Vector3d& trans);
    void PickItemTranslation(const QPoint& p);

};