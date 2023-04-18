#include <iostream>
#include <sstream>
#include "ViewOpenGL.h"
#include <QtWidgets/QApplication>
#if __linux__
#include <QMouseEvent>
#else
#include <QtGUI/QMouseEvent>
#endif
#include <QtWidgets/QDesktopWidget>
const double ViewOpenGL::trackballradius = 0.6;
ViewOpenGL::ViewOpenGL (QWidget *_parent)
	: QOpenGLWidget (_parent),
      drawMode (FLATFILL),
      colorMode(LIGHT),
      projectionMode (PERSPECTIVE),
      windowleft (-1),
      windowright (1),
      windowtop (1),
      windowbottom (-1),
      mouseMode (Qt::NoButton),
      center (0, 0, 0),
      radius (0),
      lastpoint2 (0,0),
      lastpoint3 (0, 0, 0),
      lastpointok (false)
{
	Init ();
	//QSurfaceFormat surfaceFormat;
	//surfaceFormat.setSamples(16);//多重采样
	//setFormat(surfaceFormat); //setFormat是QOpenGLWidget的函数
}

ViewOpenGL::~ViewOpenGL (void){
}

void ViewOpenGL::Init (void){
	// qt stuff
	setAttribute (Qt::WA_NoSystemBackground,true);
	setFocusPolicy (Qt::StrongFocus);//选中窗口变色
	//setAcceptDrops(true); //拖拽功能
	//SetProjectionMode (projectionMode);
}

QSize ViewOpenGL::minimumSizeHint (void) const{
	return QSize (10,10);
}

QSize ViewOpenGL::sizeHint (void) const{
	QRect rect = QApplication::desktop()->screenGeometry ();
	return QSize (int (rect.width () * 0.8),int (rect.height () * 1.0));
}

const double &ViewOpenGL::Radius (void) const{
	return radius;
}

const Vector3d &ViewOpenGL::Center (void) const{
	return center;
}

const QMatrix4x4 *ViewOpenGL::GetModelviewMatrix1(void) const {
	return &viewMatrix;
}

const QMatrix4x4* ViewOpenGL::GetProjectionMatrix1(void) const {
	return &projectionMatrix;
}

const ViewOpenGL::ProjectionMode& ViewOpenGL::GetProjectionMode(void) const {
	return projectionMode;
}

void ViewOpenGL::ResetModelviewMatrix (void){
	viewMatrix.setToIdentity();
}

void ViewOpenGL::SetProjectionMode(const ProjectionMode& pm) {
    projectionMode = pm;
	UpdateProjectionMatrix();
	ViewAll();
}

void ViewOpenGL::CopyModelViewMatrix (void){

}

void ViewOpenGL::LoadCopyModelViewMatrix (void){

}

void ViewOpenGL::SetDrawMode (const DrawMode &dm){
    drawMode = dm;
	update();
}

void ViewOpenGL::SetColorMode(const ColorMode& rm) {
    colorMode = rm;
	update();
}

const ViewOpenGL::DrawMode &ViewOpenGL::GetDrawMode (void)const {
	return drawMode;
}


void ViewOpenGL::initializeGL (void){
	// OpenGL state
	//initializeOpenGLFunctions ();

	core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

	// 加载shader
	QOpenGLShader vertexShader(QOpenGLShader::Vertex);
	vertexShader.compileSourceFile(":/shader/color.vert");
	QOpenGLShader fragmentShader(QOpenGLShader::Fragment);
	fragmentShader.compileSourceFile(":/shader/color.frag");
	shaderProgram.addShader(&vertexShader);
	shaderProgram.addShader(&fragmentShader);
	shaderProgram.link();
	shaderProgram.bind();

	core->glClearColor (0.3, 0.3, 0.3, 1.0);
	core->glClearDepth (1.0);
	core->glEnable(GL_DITHER);
	core->glEnable (GL_DEPTH_TEST);//开启深度测试
	//开启抗锯齿
	core->glEnable (GL_MULTISAMPLE);
	core->glEnable(GL_POINT_SMOOTH);
	core->glEnable(GL_BLEND);
	core->glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	core->glHint(GL_POINT_SMOOTH_HINT,GL_DONT_CARE);//GL_NICEST图形显示质量优先   GL_FASTEST 速度优先  GL_DONT_CARE系统默认
	core->glEnable(GL_MULTISAMPLE);
	core->glEnable (GL_LINE_SMOOTH);
	core->glEnable(GL_BLEND);
	core->glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	core->glHint (GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
	core->glEnable(GL_MULTISAMPLE);
	core->glEnable (GL_POLYGON_SMOOTH);
	core->glEnable(GL_BLEND);
	core->glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	core->glHint(GL_POLYGON_SMOOTH_HINT,GL_DONT_CARE);

	viewMatrix.setToIdentity();
	SetScenePosition (Vector3d (0.0,0.0,0.0),1.0);

}

void ViewOpenGL::resizeGL (int _w,int _h){
	core->glViewport (0,0,_w,_h);
	UpdateProjectionMatrix ();
	update ();

}

void ViewOpenGL::paintGL(void){
	core->glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawScene ();
}

void ViewOpenGL::DrawScene(){

}

void ViewOpenGL::mousePressEvent (QMouseEvent *_event){
	lastpoint2 = _event->pos ();
	lastpointok = MapToSphere (lastpoint2,lastpoint3);
    mouseMode = _event->button ();
}

void ViewOpenGL::mouseMoveEvent (QMouseEvent *_event){
	QPoint newPoint2D = _event->pos ();
	if(lastpointok){
		switch(mouseMode){
		case Qt::LeftButton:
			Rotation (newPoint2D);
			break;
		case Qt::RightButton:
			Translation (newPoint2D);
			break;
		default:
			break;
		}
	} // end of if

	// remember this point
	lastpoint2 = newPoint2D;
	lastpointok = MapToSphere (lastpoint2,lastpoint3);
	// trigger redraw
	update ();
}

void ViewOpenGL::mouseReleaseEvent (QMouseEvent *_event){

    mouseMode = Qt::NoButton;
	lastpointok = false;
}

void ViewOpenGL::wheelEvent (QWheelEvent *_event){
	// 0.1代表缩放尺度
	double d = -_event->delta () / 120.0 * 0.1*radius;
	Translate (Vector3d (0.0,0.0,d));
	update ();
}
void ViewOpenGL::keyPressEvent (QKeyEvent *_event){
	switch(_event->key ()){
	case Qt::Key_Escape:
		qApp->quit ();
		break;
	}
	_event->ignore ();
}

void ViewOpenGL::keyReleaseEvent (QKeyEvent *_event){
	_event->ignore ();
}

void ViewOpenGL::PickItemTranslation(const QPoint& p) {
	pickItemMoveMatrix.setToIdentity();
	double z = -(viewMatrix(2, 3) / viewMatrix(2, 2));		// 右键拖动的距离与模型离视角的远近进行绑定（使拖动更流畅）

	double w = width(); double h = height();
	double aspect = w / h;
	double near_plane = 0.01 * radius;
	double top = tan(45.0 / 2.0f * M_PI / 180.0f) * near_plane;
	double right = aspect * top;

	double dx = p.x() - lastpoint2.x();
	double dy = p.y() - lastpoint2.y();

	QVector4D trans(2.0 * dx / w * right / near_plane * z,
		-2.0 * dy / h * top / near_plane * z,
		0.0f, 1);
//	trans = currentModel->rotateMatrix.inverted() * trans;
	std::cout << trans[0] << " " << trans[1] << " " << trans[2] << " " << std::endl;
	pickItemMoveMatrix.translate(trans[0], trans[1], trans[2]);
}

void ViewOpenGL::Translation (const QPoint &p){

	double z = -(viewMatrix(2, 3) / viewMatrix(2, 2));		// 右键拖动的距离与模型离视角的远近进行绑定（使拖动更流畅）

	double w = width (); double h = height ();
	double aspect = w / h;
	double near_plane = 0.01 * radius;
	double top = tan (45.0 / 2.0f * M_PI / 180.0f) * near_plane;
	double right = aspect * top;

	double dx = p.x () - lastpoint2.x ();
	double dy = p.y () - lastpoint2.y ();

	Translate(Vector3d(2.0 * dx / w * right / near_plane * z,
		-2.0 * dy / h * top / near_plane * z,
		0.0f));

}

void ViewOpenGL::Translate (const Vector3d &_trans){
	float x = _trans[0];
	float y = _trans[1];
	float z = _trans[2];
	QVector4D trans = QVector4D{ x, y, z, 1.0f};
//	trans = currentModel->rotateMatrix.inverted() * viewRotateMatrix.inverted() * trans;
//	//modelmatrix.translate(_trans[0] / modelScale, _trans[1] / modelScale, _trans[2] / modelScale);
//    float scale = currentModel->modelScale;
//    currentModel->modelMatrix.translate(trans[0] / scale, trans[1] / scale, trans[2] / scale);
}

void ViewOpenGL::ModelScale(const Vector3d& _scale) {
//    currentModel->modelMatrix.scale(_scale[0], _scale[1], _scale[2]);
}

void ViewOpenGL::Rotation (const QPoint &p){
	Vector3d  newPoint3D;
	bool newPoint_hitSphere = MapToSphere (p,newPoint3D);
	if(newPoint_hitSphere){
		Vector3d axis = lastpoint3.Cross(newPoint3D);// corss product
		if(axis.Length() < 1e-7){
			axis = Vector3d (1,0,0);
		}
		else{
			axis.Normalize();
		}
		// find the amount of rotation
		Vector3d d = lastpoint3 - newPoint3D;
		double t = 0.5 * d.Length() / trackballradius;
		if(t < -1.0) t = -1.0;
		else if(t > 1.0) t = 1.0;
		double phi = 2.0 * asin (t);
		double  angle = phi * 180.0 / M_PI;
		Rotate (axis,angle);
	}
}

void ViewOpenGL::Rotate (const Vector3d &_axis,const double &_angle){
	QMatrix4x4 tempmatrix;
	//tempmatrix.translate(modelmatrix(0,3), modelmatrix(1, 3), modelmatrix(2, 3));	// 由于许多模型的初始center并不在坐标原点，需要把center先移到坐标原点再旋转
	tempmatrix.rotate(_angle, _axis[0], _axis[1], _axis[2]);
	//tempmatrix.translate(-modelmatrix(0, 3), -modelmatrix(1, 3), -modelmatrix(2, 3));
//    currentModel->modelMatrix = tempmatrix * currentModel->modelMatrix;		// 左乘的方式，使动态欧拉角改成静态欧拉角，解决方向旋转的问题
//    currentModel->rotateMatrix =  tempmatrix * currentModel->rotateMatrix;	// rotatematrix是用来储存旋转状态的（没有translate），模型法向量的旋转需要用到此矩阵
}

bool ViewOpenGL::MapToSphere (const QPoint &_v2D,Vector3d &_v3D){
	// This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
	// based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
	double x = (2.0 * _v2D.x () - width ()) / width ();
	double y = -(2.0 * _v2D.y () - height ()) / height ();
	double xval = x;
	double yval = y;
	double x2y2 = xval * xval + yval * yval;

	const double rsqr = trackballradius * trackballradius;
	_v3D[0] = xval;
	_v3D[1] = yval;
	if(x2y2 < 0.5 * rsqr){
		_v3D[2] = sqrt (rsqr - x2y2);
	}
	else{
		_v3D[2] = 0.5 * rsqr / sqrt (x2y2);
	}

	return true;
}

void ViewOpenGL::SetScenePosition(const Vector3d& _center, const double& _radius) {
	center = _center;
	radius = _radius;
	viewMatrix.setToIdentity();
	UpdateProjectionMatrix();
	ViewAll();
}

void ViewOpenGL::UpdateProjectionMatrix (void){
	projectionMatrix.setToIdentity();

	if(PERSPECTIVE == projectionMode){
		projectionMatrix.frustum(-0.01 * radius * (sqrt(2.0) - 1) * width() / height(),
			0.01 * radius * (sqrt(2.0) - 1) * width() / height(),
			-0.01 * radius * (sqrt(2.0) - 1),
			0.01 * radius * (sqrt(2.0) - 1),
			0.01 * radius,
			100.0 * radius);
	}
	else if(ORTHOGRAPHIC == projectionMode) //not work for
	{
		projectionMatrix.ortho(windowleft, windowright, windowbottom, windowtop, -1, 1);
	}

}

void ViewOpenGL::ViewAll (void){
	//makeCurrent();

	viewMatrix.translate(-center[0], -center[1], -(center[2] + 2.0 * radius));
}

void ViewOpenGL::ViewRotate(const QPoint& p) {
	Vector3d  newPoint3D;
	bool newPoint_hitSphere = MapToSphere(p, newPoint3D);
	if (newPoint_hitSphere) {
		Vector3d axis = lastpoint3.Cross(newPoint3D);// corss product
		if (axis.Length() < 1e-7) {
			axis = Vector3d(1, 0, 0);
		}
		else {
			axis.Normalize();
		}
		// find the amount of rotation
		Vector3d d = lastpoint3 - newPoint3D;
		double t = 0.5 * d.Length() / trackballradius;
		if (t < -1.0) t = -1.0;
		else if (t > 1.0) t = 1.0;
		double phi = 2.0 * asin(t);
		double  angle = phi * 180.0 / M_PI;
		
		QMatrix4x4 tempmatrix;
		//tempmatrix.translate(0, 0, -2.0 * radius);	// 由于许多模型的初始center并不在坐标原点，需要把center先移到坐标原点再旋转
		tempmatrix.rotate(angle, axis[0], axis[1], axis[2]);
		//viewRotateMatrix.rotate(angle, axis[0], axis[1], axis[2]);
		//tempmatrix.translate(0, 0, 2.0 * radius);
		//viewMatrix = tempmatrix * viewMatrix;		// 左乘的方式，使动态欧拉角改成静态欧拉角，解决方向旋转的问题
		viewRotateMatrix = tempmatrix * viewRotateMatrix;
	}

}

