#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent) : QMainWindow(parent), ui(new Ui::ImageViewerClass) {
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(500, 500));
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setAlignment(Qt::AlignCenter);
	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);
	DisableTools();
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event) {
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event) {
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event) {
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton && ui->toolButtonDrawLine->isChecked() && ui->ListLayers->currentItem() != nullptr) {
		if (w->getDrawLineActivated()) {
			w->Get_Layer(ui->ListLayers->currentRow())->Add_Point(e->pos());
			w->setDrawLineActivated(false);
			w->ZBuffer();
			w->update();
			EnableTools();
			ui->toolButtonDrawLine->setChecked(false);
		}
		else {
			if (w->Get_Layer(ui->ListLayers->currentRow())->Get_Points().size() == 0) {
				w->setDrawLineBegin(e->pos());
				w->setDrawLineActivated(true);
				w->Get_Layer(ui->ListLayers->currentRow())->Add_Point(w->getDrawLineBegin());
				w->Get_Layer(ui->ListLayers->currentRow())->Set_Type(0);
				w->setPixel(e->pos().x(), e->pos().y(), Qt::black);
				w->update();
			}
		}
	}
	if (e->button() == Qt::LeftButton && ui->toolButtonCircle->isChecked() && ui->ListLayers->currentItem() != nullptr) {
		if (w->getDrawCircleActivated()) {
			w->Get_Layer(ui->ListLayers->currentRow())->Add_Point(e->pos());
			w->ZBuffer();
			w->setDrawCircleActivated(false);
			w->update();
			EnableTools();
			ui->toolButtonCircle->setChecked(false);
		}
		else {
			if (vW->Get_Layer(ui->ListLayers->currentRow())->Get_Points().size() == 0) {
				w->setDrawCircleBegin(e->pos());
				w->setDrawCircleActivated(true);
				vW->Get_Layer(ui->ListLayers->currentRow())->Add_Point(w->getDrawCircleBegin());
				vW->Get_Layer(ui->ListLayers->currentRow())->Set_Type(1);
				w->setPixel(e->pos().x(), e->pos().y(), globalColor);
				w->update();
			}
		}
	}
	if (e->button() == Qt::LeftButton && ui->toolButtonPolygon->isChecked() && ui->ListLayers->currentItem() != nullptr) {
		if (w->getDrawLineActivated()) {
			w->Get_Layer(ui->ListLayers->currentRow())->Add_Point(e->pos());
			if (w->Get_Layer(ui->ListLayers->currentRow())->Get_Points().size() > 1){
				w->drawLine(w->Get_Layer(ui->ListLayers->currentRow())->Get_Points().at(w->Get_Layer(ui->ListLayers->currentRow())->Get_Points().size() - 2), e->pos(), Qt::black);
			}
		}
		if (w->Get_Layer(ui->ListLayers->currentRow())->Get_Points().size() == 0) {
			w->setDrawLineBegin(e->pos());
			w->setDrawLineActivated(true);
			w->Get_Layer(ui->ListLayers->currentRow())->Add_Point(w->getDrawLineBegin());
			w->Get_Layer(ui->ListLayers->currentRow())->Set_Type(2);
			w->setPixel(e->pos().x(), e->pos().y(), Qt::black);
			w->update();
		}
	}
	if (e->button() == Qt::RightButton && ui->toolButtonPolygon->isChecked() && ui->ListLayers->currentItem() != nullptr) {
		w->Get_Layer(ui->ListLayers->currentRow())->Add_Point(w->Get_Layer(ui->ListLayers->currentRow())->Get_Points().at(0));
		w->setDrawLineActivated(false);
		w->ZBuffer();
		EnableTools();
		w->update();
		ui->toolButtonPolygon->setChecked(false);
	}
}
void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event){
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event){
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	static QPoint delta;
	if (e->buttons() & Qt::LeftButton && w->getDragging() && ui->ListLayers->currentItem() != nullptr) {
		setCursor(Qt::ClosedHandCursor);
		delta = e->pos() - w->getDragStart();
		w->Translation(w->Get_Layer(ui->ListLayers->currentRow()), delta.x(), delta.y());
		w->ZBuffer();
		w->setDragStart(e->pos());
		w->update();
	}
	else {
		w->setDragStart(e->pos());
		setCursor(Qt::ArrowCursor); // Change cursor to indicate no dragging
	}


}
void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event){
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event){
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event){
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event){
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename){
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		return vW->setImage(loadedImg);
	}
	return false;
}
bool ImageViewer::saveImage(QString filename){
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();

	QImage* img = vW->getImage();
	return img->save(filename, extension.toStdString().c_str());
}


//Slots
void ImageViewer::on_actionOpen_triggered(){
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered(){
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered(){
	vW->clear();
}
void ImageViewer::on_actionExit_triggered(){
	this->close();
}

void ImageViewer::on_pushButtonSetColor_clicked(){
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColor->setStyleSheet(style_sheet);
		vW->Get_Layer(ui->ListLayers->currentRow())->Set_Border_Color(newColor);
		vW->ZBuffer();
	}
}
void ImageViewer::on_pushButtonSetFillColor_clicked(){
	QColor newColor = QColorDialog::getColor(globalFillColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetFillColor->setStyleSheet(style_sheet);
		vW->Get_Layer(ui->ListLayers->currentRow())->Set_Fill_Color(newColor);
		vW->ZBuffer();
	}
}
// custom functions
void ImageViewer::List_Layers() {
	ui->ListLayers->clear();
	//lisst itms based on depth but print their ID as name in list
	for (int i = 0; i < vW->Get_Layers().size(); i++) {
		ui->ListLayers->addItem(QString::number(vW->Get_Layer(i)->Get_LayerID()));
	}
	vW->ZBuffer();
}
void ImageViewer::DisableTools() {
	ui->toolButtonMoveDown->setEnabled(false);
	ui->comboBoxLineAlg->setEnabled(false);
	ui->toolButtonDrawLine->setEnabled(false);
	ui->toolButtonCircle->setEnabled(false);
	ui->toolButtonPolygon->setEnabled(false);
	ui->toolButtonRemLayer->setEnabled(false);
	ui->toolButtonMoveUp->setEnabled(false);
	ui->toolButtonTranslation->setEnabled(false);
	ui->toolButtonFlip->setEnabled(false);
	ui->toolButtonRotate->setEnabled(false);
	ui->toolButtonScale->setEnabled(false);
	ui->toolButtonShear->setEnabled(false);
	ui->toolButtonFill->setEnabled(false);
}
void ImageViewer::EnableTools() {
	ui->toolButtonMoveDown->setEnabled(true);
	ui->toolButtonDrawLine->setEnabled(true);
	ui->toolButtonCircle->setEnabled(true);
	ui->toolButtonPolygon->setEnabled(true);
	ui->toolButtonAddLayer->setEnabled(true);
	ui->toolButtonRemLayer->setEnabled(true);
	ui->toolButtonMoveUp->setEnabled(true);
	ui->toolButtonTranslation->setEnabled(true);
	ui->toolButtonFlip->setEnabled(true);
	ui->toolButtonRotate->setEnabled(true);
	ui->toolButtonScale->setEnabled(true);
	ui->toolButtonShear->setEnabled(true);
	ui->toolButtonFill->setEnabled(true);
	ui->comboBoxLineAlg->setEnabled(true);
}
void ImageViewer::DisableDrawTools() {
	ui->toolButtonDrawLine->setEnabled(false);
	ui->toolButtonCircle->setEnabled(false);
	ui->toolButtonPolygon->setEnabled(false);
}
void ImageViewer::on_toolButtonAddLayer_clicked(){
	vW->Add_Layer();
	List_Layers();
	EnableTools();
	ui->toolButtonTranslation->setChecked(false);
	ui->toolButtonFill->setChecked(false);
}
void ImageViewer::on_toolButtonRemLayer_clicked(){
	if (ui->ListLayers->currentItem() != nullptr) {
		int index = ui->ListLayers->currentRow();
		vW->Rem_Layer(index);
		List_Layers();
	}
}
void ImageViewer::on_ListLayers_itemClicked(QListWidgetItem* item) {
	int index = ui->ListLayers->currentRow();
	//set color of buttons to layer color
	ui->pushButtonSetColor->setStyleSheet(QString("background-color: #%1;").arg(vW->Get_Layer(index)->Get_Border_Color().rgba(), 0, 16));
	ui->pushButtonSetColor->setProperty("BorderColor", vW->Get_Layer(index)->Get_Border_Color());
	ui->pushButtonSetFillColor->setStyleSheet(QString("background-color: #%1;").arg(vW->Get_Layer(index)->Get_Fill_Color().rgba(), 0, 16));
	ui->pushButtonSetFillColor->setProperty("FillColor", vW->Get_Layer(index)->Get_Fill_Color());
	ui->toolButtonFill->setChecked(vW->Get_Layer(index)->Get_Fill_State());
	ui->toolButtonTranslation->setChecked(false);
	vW->setDragging(false);
	// get layer from index
	Layer* layer1 = vW->Get_Layer(index);
	layer1->Print_Info();
}
void ImageViewer::on_toolButtonMoveUp_clicked() {
	if (ui->ListLayers->currentItem() != nullptr) {
		int index1 = ui->ListLayers->currentRow();
		qDebug() << "Index1: " << index1 << " item: " << ui->ListLayers->currentItem()->text();
		int index2 = index1-1;
		if (index2 < 0) {
			return;
		}
		vW->Swap_Layers_Depth(index1, index2);
		//sort by depth;
		QVector<Layer> layers = vW->Get_Layers();
		std::sort(layers.begin(), layers.end(), [](Layer a, Layer b) {return a.Get_Layer_Depth() < b.Get_Layer_Depth();});
		vW->Set_Layers(layers);
		List_Layers();
	}
}
void ImageViewer::on_toolButtonMoveDown_clicked() {
	if (ui->ListLayers->currentItem() != nullptr) {
		int index1 = ui->ListLayers->currentRow();
		int index2 = index1 + 1;
		if (index2 >= vW->Get_Layers().size()) {
			return;
		}
		vW->Swap_Layers_Depth(index1, index2);
		QVector<Layer> layers = vW->Get_Layers();
		std::sort(layers.begin(), layers.end(), [](Layer a, Layer b) {return a.Get_Layer_Depth() < b.Get_Layer_Depth();});
		vW->Set_Layers(layers);
		List_Layers();
	}
}
void ImageViewer::on_toolButtonTranslation_clicked() {
	if (ui->ListLayers->currentItem() != nullptr && ui->toolButtonTranslation->isChecked()) {
		vW->setDragging(true);
	}
	else {
		vW->setDragging(false);
	}
}
void ImageViewer::on_toolButtonFlip_clicked() {
	if (ui->ListLayers->currentItem() != nullptr) {
		vW->Flip(vW->Get_Layer(ui->ListLayers->currentRow()));
		vW->ZBuffer();
	}
}
void ImageViewer::on_toolButtonRotate_clicked(){
	if (ui->ListLayers->currentItem() != nullptr) {
		vW->Rotation(vW->Get_Layer(ui->ListLayers->currentRow()), ui->spinBoxRot->value());
		vW->ZBuffer();
	}
}
void ImageViewer::on_toolButtonScale_clicked(){
	if (ui->ListLayers->currentItem() != nullptr) {
		vW->Scale(vW->Get_Layer(ui->ListLayers->currentRow()), ui->DoubleSpinBoxScaleX->value(), ui->DoubleSpinBoxScaleY->value());
		vW->ZBuffer();
	}
}
void ImageViewer::on_toolButtonShear_clicked() {
	if (ui->ListLayers->currentItem() != nullptr) {
		vW->Shear(vW->Get_Layer(ui->ListLayers->currentRow()), ui->DoubleSpinBoxShearX->value());
		vW->ZBuffer();
	}
}
void ImageViewer::on_toolButtonFill_clicked() {
	if (ui->ListLayers->currentItem() != nullptr) {
		if (ui->toolButtonFill->isChecked()) {
			vW->Get_Layer(ui->ListLayers->currentRow())->Set_Fill_State(true);
		}
		else {
			vW->Get_Layer(ui->ListLayers->currentRow())->Set_Fill_State(false);
		}
		vW->ZBuffer();
	}
}
void ImageViewer::on_toolButtonDrawLine_clicked() {
	if (ui->toolButtonDrawLine->isChecked()) {
		ui->toolButtonMoveDown->setEnabled(false);
		ui->comboBoxLineAlg->setEnabled(false);
		ui->toolButtonCircle->setEnabled(false);
		ui->toolButtonPolygon->setEnabled(false);
		ui->toolButtonAddLayer->setEnabled(false);
		ui->toolButtonRemLayer->setEnabled(false);
		ui->toolButtonMoveUp->setEnabled(false);
		ui->toolButtonTranslation->setEnabled(false);
		ui->toolButtonFlip->setEnabled(false);
		ui->toolButtonRotate->setEnabled(false);
		ui->toolButtonScale->setEnabled(false);
		ui->toolButtonShear->setEnabled(false);
		ui->toolButtonFill->setEnabled(false);
	}
	else {
		ui->toolButtonMoveDown->setEnabled(true);
		ui->comboBoxLineAlg->setEnabled(true);
		ui->toolButtonCircle->setEnabled(true);
		ui->toolButtonPolygon->setEnabled(true);
		ui->toolButtonAddLayer->setEnabled(true);
		ui->toolButtonRemLayer->setEnabled(true);
		ui->toolButtonMoveUp->setEnabled(true);
		ui->toolButtonTranslation->setEnabled(true);
		ui->toolButtonFlip->setEnabled(true);
		ui->toolButtonRotate->setEnabled(true);
		ui->toolButtonScale->setEnabled(true);
		ui->toolButtonShear->setEnabled(true);
		ui->toolButtonFill->setEnabled(true);
	}
}
void ImageViewer::on_toolButtonCircle_clicked() {
	if (ui->toolButtonCircle->isChecked()) {
		ui->toolButtonMoveDown->setEnabled(false);
		ui->comboBoxLineAlg->setEnabled(false);
		ui->toolButtonDrawLine->setEnabled(false);
		ui->toolButtonPolygon->setEnabled(false);
		ui->toolButtonAddLayer->setEnabled(false);
		ui->toolButtonRemLayer->setEnabled(false);
		ui->toolButtonMoveUp->setEnabled(false);
		ui->toolButtonTranslation->setEnabled(false);
		ui->toolButtonFlip->setEnabled(false);
		ui->toolButtonRotate->setEnabled(false);
		ui->toolButtonScale->setEnabled(false);
		ui->toolButtonShear->setEnabled(false);
		ui->toolButtonFill->setEnabled(false);
	}
	else {
		ui->toolButtonMoveDown->setEnabled(true);
		ui->comboBoxLineAlg->setEnabled(true);
		ui->toolButtonDrawLine->setEnabled(true);
		ui->toolButtonPolygon->setEnabled(true);
		ui->toolButtonAddLayer->setEnabled(true);
		ui->toolButtonRemLayer->setEnabled(true);
		ui->toolButtonMoveUp->setEnabled(true);
		ui->toolButtonTranslation->setEnabled(true);
		ui->toolButtonFlip->setEnabled(true);
		ui->toolButtonRotate->setEnabled(true);
		ui->toolButtonScale->setEnabled(true);
		ui->toolButtonShear->setEnabled(true);
		ui->toolButtonFill->setEnabled(true);
	}
}
void ImageViewer::on_toolButtonPolygon_clicked() {
	if (ui->toolButtonPolygon->isChecked()) {
		ui->toolButtonMoveDown->setEnabled(false);
		ui->comboBoxLineAlg->setEnabled(false);
		ui->toolButtonDrawLine->setEnabled(false);
		ui->toolButtonCircle->setEnabled(false);
		ui->toolButtonAddLayer->setEnabled(false);
		ui->toolButtonRemLayer->setEnabled(false);
		ui->toolButtonMoveUp->setEnabled(false);
		ui->toolButtonTranslation->setEnabled(false);
		ui->toolButtonFlip->setEnabled(false);
		ui->toolButtonRotate->setEnabled(false);
		ui->toolButtonScale->setEnabled(false);
		ui->toolButtonShear->setEnabled(false);
		ui->toolButtonFill->setEnabled(false);
	}
	else {
		ui->toolButtonMoveDown->setEnabled(true);
		ui->comboBoxLineAlg->setEnabled(true);
		ui->toolButtonDrawLine->setEnabled(true);
		ui->toolButtonCircle->setEnabled(true);
		ui->toolButtonAddLayer->setEnabled(true);
		ui->toolButtonRemLayer->setEnabled(true);
		ui->toolButtonMoveUp->setEnabled(true);
		ui->toolButtonTranslation->setEnabled(true);
		ui->toolButtonFlip->setEnabled(true);
		ui->toolButtonRotate->setEnabled(true);
		ui->toolButtonScale->setEnabled(true);
		ui->toolButtonShear->setEnabled(true);
		ui->toolButtonFill->setEnabled(true);
	}
}
void ImageViewer::on_toolButtonSaveData_clicked() {
	vW->Save_Data();
	List_Layers();
}
void ImageViewer::on_toolButtonLoadData_clicked() {
	vW->Load_Data();
	List_Layers();
}
