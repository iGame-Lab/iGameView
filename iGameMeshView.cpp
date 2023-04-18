#include "iGameMeshView.h"

iGameMeshView::iGameMeshView(QWidget* parent) :QMainWindow(parent), ui(new Ui::MainWindow)
{

	ui->setupUi(this);


	connect(ui->btnLastMesh, SIGNAL(clicked(bool)), this, SLOT(LastMesh()));
	connect(ui->btnNextMesh, SIGNAL(clicked(bool)), this, SLOT(NextMesh()));


	///样条
	//打开和保存
	connect(ui->btnOpenCurve, SIGNAL(clicked(bool)), this, SLOT(OpenNurbs()));
	connect(ui->btnOpenSurface, SIGNAL(clicked(bool)), this, SLOT(OpenNurbs()));
	connect(ui->btnOpenVolume, SIGNAL(clicked(bool)), this, SLOT(OpenNurbs()));
	connect(ui->btnSaveCurve, SIGNAL(clicked(bool)), this, SLOT(SaveNurbs()));
	connect(ui->btnSaveSurface, SIGNAL(clicked(bool)), this, SLOT(SaveNurbs()));
	connect(ui->btnSaveVolume, SIGNAL(clicked(bool)), this, SLOT(SaveNurbs()));
	///todo: 通过控制点生成文件
	//curveControlCnt,curveControlDegree_u
	//surfaceControlCnt_u,surfaceControlCnt_v,surfaceControlDegree_u,surfaceControlDegree_v
	//volumeControlCnt_u,volumeControlCnt_v,volumeControlCnt_w,volumeControlDegree_u,volumeControlDegree_v,volumeControlDegree_w
	connect(ui->bthGenerateCurve, SIGNAL(clicked(bool)), this, SLOT(CreateNurbsCurve()));
	connect(ui->bthGenrateSurface, SIGNAL(clicked(bool)), this, SLOT(CreateNurbsSurface()));
	connect(ui->bthGenrateVolume,SIGNAL(clicked(bool)),this, SLOT(CreateNurbsVolume()));




	// 点选和切割操作
	connect(ui->rtnModelView, SIGNAL(clicked(bool)), this, SLOT(setModelView()));
	connect(ui->rtnPickItem, SIGNAL(clicked(bool)), this, SLOT(setPickItem()));
	connect(ui->rtnModelCut, SIGNAL(clicked(bool)), this, SLOT(setModelCut()));

	connect(ui->rtnPickPoint, SIGNAL(clicked(bool)), this, SLOT(setPickPoint()));
	connect(ui->rtnPickEdge, SIGNAL(clicked(bool)), this, SLOT(setPickEdge()));
	connect(ui->rtnPickFace, SIGNAL(clicked(bool)), this, SLOT(setPickFace()));
	connect(ui->rtnPickCell, SIGNAL(clicked(bool)), this, SLOT(setPickCell()));

	connect(ui->btnPickRemoveAll, SIGNAL(clicked(bool)), this, SLOT(removeSelectedAll()));
	connect(ui->btnToCut, SIGNAL(clicked(bool)), this, SLOT(cutModel()));
	connect(ui->btnRemoveLine, SIGNAL(clicked(bool)), this, SLOT(removeLine()));

	//绘制样式
	connect(ui->viewpoints,SIGNAL (clicked (bool)),this,SLOT (ViewStylePoints ()));
	connect(ui->viewlines,SIGNAL (clicked (bool)),this,SLOT (ViewStyleLines ()));
	connect(ui->viewfill,SIGNAL (clicked (bool)),this,SLOT (ViewStyleFill ()));
	connect(ui->viewflatfill, SIGNAL(clicked(bool)), this, SLOT(ViewStyleFlatFill()));
	connect(ui->viewclear, SIGNAL(clicked(bool)), this, SLOT(ViewClear()));
	connect(ui->viewlight, SIGNAL(clicked(bool)), this, SLOT(ViewLight()));
	connect(ui->viewnormal, SIGNAL(clicked(bool)), this, SLOT(ViewNormal()));
	connect(ui->ckbSurfaceGrid, SIGNAL(clicked(bool)), this, SLOT(ChangeSurfaceGrid()));
	connect(ui->ckbFaceCull, SIGNAL(clicked(bool)), this, SLOT(ChangeFaceCull()));
	connect(ui->ckbCoordCircle, SIGNAL(clicked(bool)), this, SLOT(ChangeCoordCircle()));


	//x,y,z切割
	connect(ui->ckbCutPlane, SIGNAL(clicked(bool)), this, SLOT(ChangeCutFlag()));
	connect(ui->cutPlaneX, SIGNAL(valueChanged(int)), this, SLOT(UpdateCutXYZ()));
	connect(ui->cutPlaneY, SIGNAL(valueChanged(int)), this, SLOT(UpdateCutXYZ()));
	connect(ui->cutPlaneZ, SIGNAL(valueChanged(int)), this, SLOT(UpdateCutXYZ()));
    
	// 信息监视器
	ui->openGLWidget->setShowInfo([&]() {
			PrintMeshInfo();
		});
}

iGameMeshView::~iGameMeshView (void){
}

bool iGameMeshView::OpenNurbs(void) {
	
	QString filePath = QFileDialog::getOpenFileName(this, tr("Load file"), tr(""), tr("XmlFiles(*.xml)"));
	if (filePath.isEmpty())
		return false;
	else {
		ui->openGLWidget->loadNurbsGeo(filePath.toStdString());
		std::string filename = filePath.toStdString();
		int beg = filename.find_last_of('/');
		filePath = QString::fromStdString(filename.substr(++beg, filename.length()));

		return true;
	}
}

bool iGameMeshView::CreateNurbsCurve(void) {
	std::vector<int> cc{ ui->curveControlCnt->value() };
	std::vector<int> dd{ui->curveControlDegree->value()};

	ui->openGLWidget->creatNurbs(cc, dd);

	return true;
}

bool iGameMeshView::CreateNurbsSurface(void) {
	std::vector<int> cc{ ui->surfaceControlCnt_u->value(),  ui->surfaceControlCnt_v->value() };
	std::vector<int> dd{ ui->surfaceControlDegree_u->value(),ui->surfaceControlDegree_v->value()};

	ui->openGLWidget->creatNurbs(cc, dd);

	return true;
}

bool iGameMeshView::CreateNurbsVolume(void) {
	std::vector<int> cc{ ui->volumeControlCnt_u->value(), ui->volumeControlCnt_v->value(), ui->volumeControlCnt_w->value() };
	std::vector<int> dd{ ui->volumeControlDegree_u->value(), ui->volumeControlDegree_v->value(), ui->volumeControlDegree_w->value() };

	ui->openGLWidget->creatNurbs(cc, dd);
	return true;
}

bool iGameMeshView::SaveNurbs(void) {
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save model file:"), tr("untitled"), tr("XmlFiles(*.xml)"));
	if (!fileName.isEmpty())
	{
		assert(ui->openGLWidget);
		std::string filename = fileName.toStdString();
		ui->openGLWidget->saveNurbsGeo(filename);
		int beg = filename.find_last_of('/');
		fileName = QString::fromStdString(filename.substr(++beg, filename.length()));
		return true;
	}
	return false;
}

bool iGameMeshView::OpenMesh(void) {
	return false;
}

bool iGameMeshView::SaveMesh(void)
{
	return false;
}

bool iGameMeshView::LastMesh(void) {
	ui->openGLWidget->changeCurrentMeshToLast();
	return true;
}

bool iGameMeshView::NextMesh(void) {
	ui->openGLWidget->changeCurrentMeshToNext();
	return true;
}



bool iGameMeshView::CopyMesh(void) {
	if (ui->openGLWidget->copyMesh()) ui->info->printString("copy mesh success");
	else ui->info->printString("copy mesh fail, triMesh is nullptr");
	return true;
}

bool iGameMeshView::RecoverMesh(void) {
	if (ui->openGLWidget->recoverMesh()) ui->info->printString("recover mesh success");
	else ui->info->printString("recover mesh fail, copyTriMesh is nullptr");
	return true;
}

bool iGameMeshView::AddNoise(void) {
	if (!ui->openGLWidget->hasMesh()) return false;
    QtConcurrent::run([&](){
        ui->info->printString("add noise success");});
	return true;
}

bool iGameMeshView::MotorcycleGraphs(void) {
	if (!ui->openGLWidget->hasMesh()) return false;
	return true;
}

bool iGameMeshView::MeshAlgorithm1(void) {
    if (!ui->openGLWidget->hasMesh()) return false;
    ui->info->printString("custom Algorithm1");
    return true;
}

bool iGameMeshView::MeshAlgorithm2(void) {
    if (!ui->openGLWidget->hasMesh()) return false;
    ui->info->printString("custom Algorithm1");
    return true;
}

bool iGameMeshView::MeshAlgorithm3(void) {
    if (!ui->openGLWidget->hasMesh()) return false;
    ui->info->printString("custom Algorithm1");
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool iGameMeshView::ViewStylePoints (){
	ui->openGLWidget->SetDrawMode (ModelDraw::POINTS);
	ui->openGLWidget->makeCurrent ();
	return true;
}

bool iGameMeshView::ViewStyleLines (){
	ui->openGLWidget->SetDrawMode (ModelDraw::FLATLINES);
	ui->openGLWidget->makeCurrent ();
	return true;
}

bool iGameMeshView::ViewStyleFill (){
	ui->openGLWidget->SetDrawMode (ModelDraw::FILL);
	ui->openGLWidget->makeCurrent ();
	return true;
}

bool iGameMeshView::ViewStyleFlatFill() {
	ui->openGLWidget->SetDrawMode(ModelDraw::FLATFILL);
	ui->openGLWidget->makeCurrent();
	return true;
}

bool iGameMeshView::ViewClear() {
	ui->openGLWidget->clearMesh();
	return true;
}

bool iGameMeshView::ViewLight() {
	ui->openGLWidget->SetColorMode(ModelDraw::LIGHT);
	return true;
}

bool iGameMeshView::ViewNormal() {
	ui->openGLWidget->SetColorMode(ModelDraw::NORMAL);
	return true;
}

bool iGameMeshView::ChangeSurfaceGrid() {
	ui->openGLWidget->changeSurfaceGrid();
	return true;
}

bool iGameMeshView::ChangeFaceCull() {
	ui->openGLWidget->changeFaceCull();
	return true;
}

bool iGameMeshView::ChangeCoordCircle() {
	ui->openGLWidget->changeCoordCircle();
	return true;
}

void iGameMeshView::PrintMeshInfo() {
}

bool iGameMeshView::setModelView() {
	ui->openGLWidget->setEditModel(ModelDraw::MODELVIEW);
	ui->info->printString("Edit Mode: Model View");
	return true;
}

bool iGameMeshView::setPickItem() {
	ui->openGLWidget->setEditModel(ModelDraw::PICKITEM);
	ui->info->printString("Edit Mode: Pick Item");
	return true;
}

bool iGameMeshView::setModelCut() {
	ui->openGLWidget->setEditModel(ModelDraw::MODELCUT);
	ui->info->printString("Edit Mode: Model Cut");
	return true;
}

bool iGameMeshView::setPickPoint() {
	ui->openGLWidget->setPickType(ModelDraw::PickPoint);
	ui->info->printString("Pick : Point");
	return true;
}

bool iGameMeshView::setPickEdge() {
	ui->openGLWidget->setPickType(ModelDraw::PickEdge);
	ui->info->printString("Pick : Edge");
	update();
	return true;
}

bool iGameMeshView::setPickFace() {
	ui->openGLWidget->setPickType(ModelDraw::PickFace);
	ui->info->printString("Pick : Face");
	return true;
}

bool iGameMeshView::setPickCell() {
	ui->openGLWidget->setPickType(ModelDraw::PickCell);
	ui->info->printString("Pick : Cell");
	return true;
}

bool iGameMeshView::removeSelectedAll() {
	ui->openGLWidget->removeSelectedAll();
	return true;
}

bool iGameMeshView::cutModel() {
	//ui->openGLWidget->toCutModel();
	return true;
}

bool iGameMeshView::removeLine() {
	//ui->openGLWidget->removeCutLine();
	return true;
}

bool iGameMeshView::ChangeCutFlag() {
	if (ui->ckbCutPlane->isChecked()) {
		ui->info->printString("enable cut plane");
		UpdateCutXYZ();// 会自动更新 drawdata
	}
	else {
		ui->info->printString("disable cut plane");
		ui->openGLWidget->updateCutFlag(false);
	}
	update();
	return true;
}

bool iGameMeshView::UpdateCutXYZ() {
	ui->openGLWidget->cutX = (ui->cutPlaneX->value() / 100.f);
	ui->openGLWidget->cutY = (ui->cutPlaneY->value() / 100.f);
	ui->openGLWidget->cutZ = (ui->cutPlaneZ->value() / 100.f);
	if (ui->ckbCutPlane->isChecked()) ui->openGLWidget->updateCutFlag(true);
	else ui->openGLWidget->updateCutFlag(false);
	printf("cut rate: %.2f, %.2f, %.2f\n", ui->openGLWidget->cutX, ui->openGLWidget->cutY, ui->openGLWidget->cutZ);
	//ui->info->printString("update cut plane success");
	return true;
}

