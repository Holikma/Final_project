#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"

class ImageViewer : public QMainWindow{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);

private:
	Ui::ImageViewerClass* ui;
	ViewerWidget* vW;

	QColor globalColor;
	QColor globalFillColor;
	QSettings settings;
	QMessageBox msgBox;

	//Event filters
	bool eventFilter(QObject* obj, QEvent* event);

	//ViewerWidget Events
	bool ViewerWidgetEventFilter(QObject* obj, QEvent* event);
	void ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event);
	void ViewerWidgetLeave(ViewerWidget* w, QEvent* event);
	void ViewerWidgetEnter(ViewerWidget* w, QEvent* event);
	void ViewerWidgetWheel(ViewerWidget* w, QEvent* event);

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	bool openImage(QString filename);
	bool saveImage(QString filename);

	void List_Layers();
	void DisableTools();
	void EnableTools();
	void DisableDrawTools();

private slots:
	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_actionClear_triggered();
	void on_actionExit_triggered();

	//Tools slots
	void on_pushButtonSetColor_clicked();
	void on_pushButtonSetFillColor_clicked();
	void on_toolButtonAddLayer_clicked();
	void on_toolButtonRemLayer_clicked();
	void on_ListLayers_itemClicked(QListWidgetItem* item);
	void on_toolButtonMoveUp_clicked();
	void on_toolButtonMoveDown_clicked();
	void on_toolButtonTranslation_clicked();
	void on_toolButtonFlip_clicked();
	void on_toolButtonRotate_clicked();
	void on_toolButtonScale_clicked();
	void on_toolButtonShear_clicked();
	void on_toolButtonFill_clicked();
	void on_toolButtonDrawLine_clicked();
	void on_toolButtonCircle_clicked();
	void on_toolButtonPolygon_clicked();
	void on_toolButtonSaveData_clicked();
	void on_toolButtonLoadData_clicked();
};
