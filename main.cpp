#include "iGameMeshView.h"
#include <QtWidgets/QApplication>
#if __linux__
#include <qsurfaceformat.h>
#include <qpixmap.h>
#include <qsplashscreen.h>
#include <qtextcodec.h>
#else
#include <QtGUI/Qsurfaceformat.h>
#include <QtGUI/QPixmap>
#include <QtWidgets/Qsplashscreen.h>
#include <QtCore/Qtextcodec.h>
#endif
int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);	// 窗口高分辨率支持
	QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);// 图标高分辨率支持
	//QCoreApplication::setAttribute (Qt::AA_UseDesktopOpenGL);
	QApplication a(argc, argv);
	a.processEvents ();
	QSurfaceFormat format;
	format.setRenderableType (QSurfaceFormat::OpenGL);
	format.setSamples (16);
	format.setDepthBufferSize (32);
	//format.setStencilBufferSize (16);
	format.setVersion (4,1);
	//format.setProfile (QSurfaceFormat::CompatibilityProfile);
	format.setProfile (QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat (format);
	QPixmap pixmap ("../Resources/iGameLogo.png");
	QSplashScreen splash (pixmap);
	splash.show ();
	QTextCodec *codec = QTextCodec::codecForName ("GBK");
	iGameMeshView w;
	w.setWindowTitle (codec->toUnicode ("iGame"));
    splash.finish (&w);
	w.show ();
	return a.exec();
}
