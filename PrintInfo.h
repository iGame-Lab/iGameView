#pragma once
/* 输出操作提示信息与模型数据信息 */
#include <QtWidgets/qtextbrowser.h>
#include <QtCore/qtextcodec.h> 
#include <vector>
using namespace std;
class PrintInfo :
	public QTextBrowser{
	Q_OBJECT
public:
	PrintInfo (QWidget *parent = Q_NULLPTR);
	virtual ~PrintInfo (void);
public:
	void infoClear ();
	void inputMeshInfo(QString &filename, int vcnt, int fcnt, QString &str);
	void outputMeshInfo(QString &filename, int vcnt, int fcnt);
	void printString(QString);

	void inputFileInfo (QString &pathinfo,QString &filetype);//读取的文件信息
	void outputFileInfo (QString &pathinfo);//保存文件信息
	void curveModelInfo (vector<string> objtype,vector<int> ControlNum,vector<int> degree);
	void surfaceModelInfo (vector<string> objtype,vector<int> uControlNum,vector<int> vControlNum,vector<int> udegree,vector<int> vdegree);//打印模型信息
private:
	QTextCodec *codec = QTextCodec::codecForName ("GBK");//文本转换对象
};