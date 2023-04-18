#include "PrintInfo.h"
#include "ModelDraw.h"

PrintInfo::PrintInfo (QWidget *parent):QTextBrowser (parent){
}
PrintInfo::~PrintInfo(void) {

}
void PrintInfo::infoClear (){
	this->clear ();
}
void PrintInfo::printString(QString str) {
	this->insertPlainText(str);
	this->append("");
}

void PrintInfo::inputMeshInfo(QString &filename, int vcnt, int fcnt, QString &str) {
	this->insertPlainText("read " + filename + " :  vcnt = " + QString::number(vcnt) + ",  fcnt = " + QString::number(fcnt) + ";  " + str);
	this->append("");
}
void PrintInfo::outputMeshInfo(QString &filename, int vcnt = 0, int fcnt = 0) {
	this->insertPlainText("write " + filename + " :  vcnt = " + QString::number(vcnt) + ",  fcnt = " + QString::number(fcnt));
	this->append("");
}
void PrintInfo::inputFileInfo (QString &pathinfo,QString &filetype){
	this->clear ();
	this->insertPlainText (codec->toUnicode (" ————————————————————文件信息———————————————————— "));
	this->append ("");
	this->insertPlainText (codec->toUnicode(" 已读取文件，文件格式： ")+filetype);
	this->append ("");
	this->insertPlainText (codec->toUnicode (" 文件路径： ")+ codec->toUnicode(pathinfo.toStdString().c_str()));
}
void PrintInfo::outputFileInfo (QString &pathinfo){
	this->clear ();
	this->insertPlainText (codec->toUnicode (" ————————————————————文件信息———————————————————— "));
	this->append ("");
	this->insertPlainText (codec->toUnicode (" 已保存文件 "));
	this->append ("");
	this->insertPlainText (codec->toUnicode (" 文件路径： ") + codec->toUnicode(pathinfo.toStdString ().c_str ()));
}
void PrintInfo::curveModelInfo (vector<string> objtype,vector<int> ControlNum,vector<int> degree){
	this->clear ();
	this->insertPlainText (codec->toUnicode (" ————————————————————几何曲线信息———————————————————— "));
	for(int i = 0;i < objtype.size ();i++){
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 第 ")+ codec->toUnicode (QString::number (i+1).toStdString ().c_str ())+ codec->toUnicode(" 条曲线 "));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 几何类型： "));
		this->insertPlainText (codec->toUnicode (objtype[i].c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 控制点个数： "));
		this->insertPlainText (codec->toUnicode (QString::number (ControlNum[i]).toStdString ().c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 次数： "));
		this->insertPlainText (codec->toUnicode (QString::number (degree[i]).toStdString ().c_str ()));
		this->append ("");
	}
}
void PrintInfo::surfaceModelInfo (vector<string> objtype,vector<int> uControlNum,vector<int> vControlNum,vector<int> udegree,vector<int> vdegree){
	this->clear ();
	this->insertPlainText (codec->toUnicode (" ————————————————————几何曲面信息———————————————————— "));
	for(int i = 0;i < objtype.size ();i++){
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 第 ") + codec->toUnicode (QString::number (i + 1).toStdString ().c_str ()) + codec->toUnicode (" 张曲面 "));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 几何类型： "));
		this->insertPlainText (codec->toUnicode(objtype[i].c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" U方向控制点个数： "));
		this->insertPlainText (codec->toUnicode (QString::number(uControlNum[i]).toStdString().c_str()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" V方向控制点个数： "));
		this->insertPlainText (codec->toUnicode (QString::number (vControlNum[i]).toStdString ().c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" U方向次数： "));
		this->insertPlainText (codec->toUnicode (QString::number (udegree[i]).toStdString ().c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" V方向次数： "));
		this->insertPlainText (codec->toUnicode (QString::number (vdegree[i]).toStdString ().c_str ()));
		this->append ("");
	}
}
