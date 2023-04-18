#pragma once
/*
OpenGL绘图窗口类，包括图形的渲染和视图显示
*/
#include <QtWidgets/qopenglwidget.h>
#if __linux__
#include <qopenglfunctions.h>
#else
#include <QtGUI/qopenglfunctions.h>
#endif
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <map>
#include "TinyVector.h"
//class QOpenGLTexture;
class ViewOpenGL :public QOpenGLWidget
{
	Q_OBJECT
public:
	ViewOpenGL (QWidget *_parent =0);
	virtual ~ViewOpenGL (void);
private:
	void Init (void);// OpenGL初始化
public:
	QSize minimumSizeHint (void) const override;
	QSize sizeHint (void) const override;
	/* 视角半径 */
	const double &Radius (void) const;
	/* 视角中心点 */
	const Vector3d &Center (void) const;
	//const double *GetModelviewMatrix (void) const;
	const QMatrix4x4 *GetModelviewMatrix1(void) const;
	void ResetModelviewMatrix (void);
	void CopyModelViewMatrix (void);
	void LoadCopyModelViewMatrix (void);
	//const double *GetProjectionMatrix (void) const;
	const QMatrix4x4 *GetProjectionMatrix1(void) const;

	enum ProjectionMode{
		PERSPECTIVE,ORTHOGRAPHIC
	};
	void SetProjectionMode (const ProjectionMode &pm);
	const ProjectionMode &GetProjectionMode (void) const;
	/* 绘制样式---点、线框、线、平滑面、面片 */
	enum DrawMode{// BOUNDLINE 暂未实现
		POINTS, BOUNDLINE, FLATLINES, FILL, FLATFILL
	};
	enum ColorMode {
		LIGHT, NORMAL
	};
	enum MeshType{
		NURBSCURVE, NURBSSURFACE, NURBSVOLUME
	};
	enum Structure {
		SURFACEMESH, VOLUMEMESH, NURBS
	};
	void SetDrawMode (const DrawMode &dm);
	void SetColorMode(const ColorMode& dm);
	//void SetDrawModule (const MeshType &module);
	//void SetStructure(const Structure& structure);

	const DrawMode &GetDrawMode (void) const;
	const ColorMode& GetColorMode(void) const;

protected:

	/* OpenGL绘制重写函数 */
	void initializeGL (void) override;
	void resizeGL (int w,int h) override;
	void paintGL (void) override;
	/* 设置绘制样式 */
	virtual void DrawScene ();
	/* 鼠标键盘事件 */
	virtual void mousePressEvent (QMouseEvent *) override;
	virtual void mouseMoveEvent (QMouseEvent *) override;
	virtual void mouseReleaseEvent (QMouseEvent *) override;
	virtual void wheelEvent (QWheelEvent *) override;
	virtual void keyPressEvent (QKeyEvent *) override;
	virtual void keyReleaseEvent (QKeyEvent *) override;
protected:
	/* 旋转、缩放、平移等变换 */
	void Translation (const QPoint &p);
    void Rotation (const QPoint &p);
	bool MapToSphere (const QPoint &point,Vector3d &result);
	void UpdateProjectionMatrix (void);
    void ViewRotate(const QPoint& p);

    virtual void Translate (const Vector3d &trans);
    virtual void Rotate (const Vector3d &axis,const double &angle);
    virtual void ModelScale(const Vector3d& trans);
    virtual void PickItemTranslation(const QPoint& p);

public:
	/* 设置OpenGL绘制窗口的显示区域---显示中心和显示半径 */
	void SetScenePosition (const Vector3d &c,const double &r);
	void ViewAll (void);//更新模型矩阵
protected:
	DrawMode drawMode;// 绘制样式枚举值
	ColorMode colorMode;// 渲染样式枚举值
	ProjectionMode projectionMode;// 变换矩阵
	double windowleft;
	double windowright;
	double windowtop;
	double windowbottom;
	/* 鼠标按压模式 */
	Qt::MouseButton mouseMode;
	/* 视角中心和半径值，可以通过鼠标操作改变 */
	Vector3d center{0.0, 0.0, 0.0};
	double radius = 1.0f;
	bool ModelMoveState = false;	// 记录模型是否正在移动
	// 声明时已为单位矩阵
	QMatrix4x4 projectionMatrix;
	QMatrix4x4 viewMatrix;
	QMatrix4x4 viewRotateMatrix;
	QMatrix4x4 pickItemMoveMatrix;
	/* 鼠标移动时记录坐标点的位置 */
	QPoint lastpoint2;
	Vector3d lastpoint3;
	bool lastpointok;

	QOpenGLFunctions_3_3_Core* core;	// OpenGL核心态
	QOpenGLShaderProgram shaderProgram; // shader对象

protected:
	static const double trackballradius;
};