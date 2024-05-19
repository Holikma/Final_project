#pragma once
#include <QtWidgets>
#include <random>
#include <qrandom.h>
#include <iostream>

//class representing a layer in the viewer, contains vector of points
class Layer {
	private:
		QVector<QPoint> points;
		int LayerID = 0;
		int Layer_Depth = 0;
		int type = -1;
		// 0 = line, 1 = circle, 2 = polygon
		QColor BorderColor = QColor(125, 125, 125);
		QColor FillColor = QColor(255, 255, 125);
		int draw_type = 0;
		bool fill_state = false;
	public:
		Layer() {};
		Layer(QVector<QPoint> points) : points(points) {};
		int Get_Layer_Depth() { return Layer_Depth; }
		void Set_Layer_Depth(int depth) { Layer_Depth = depth; }
		QVector<QPoint> Get_Points() { return points; }
		QPoint Get_Point(int index) { return points[index]; }
		void Set_Point(int index, int dx, int dy) { points[index].setX(points[index].x() + dx); points[index].setY(points[index].y() + dy); }
		void Set_Point(int index, QPoint point) { points[index] = point; }
		int Get_Type() { return type; }
		void Set_Type(int t) { type = t; }
		QColor Get_Fill_Color() { return FillColor; }
		void Set_Fill_Color(QColor color) { FillColor = color; }
		QColor Get_Border_Color() { return BorderColor; }
		void Set_Border_Color(QColor color) { BorderColor = color; }
		void Add_Point(QPoint point) { points.push_back(point); }
		void Print_Info();
		void Set_LayerID(int id) { LayerID = id; }
		int Get_LayerID() { return LayerID; }
		int Get_Draw_Type() { return draw_type; }
		bool Get_Fill_State() { return fill_state; }
		void Set_Fill_State(bool state) { fill_state = state; }
};

struct Edge {
	QPoint start;
	QPoint end;
	double m;
	int delta_y;
	double x;
	double w;
};

class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;
	uchar* data = nullptr;

	bool drawLineActivated = false;
	bool drawCircleActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);
	QPoint drawCircleBegin = QPoint(0, 0);

	bool isDragging = false;
	QPoint DragStart = QPoint(0, 0);

	QVector<Layer> Layers;

public:
	ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	bool changeSize(int width, int height);
	void setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a = 255);
	void setPixel(int x, int y, double valR, double valG, double valB, double valA = 1.);
	void setPixel(int x, int y, const QColor& color);
	bool isInside(int x, int y) { return (x >= 0 && y >= 0 && x < img->width() && y < img->height()) ? true : false; }
	//Draw functions
	void drawLine(QPoint start, QPoint end, QColor color, int algType = 0);
	void setDrawLineBegin(QPoint begin) { drawLineBegin = begin; }
	void setDrawCircleBegin(QPoint begin) { drawCircleBegin = begin; }
	QPoint getDrawLineBegin() { return drawLineBegin; }
	QPoint getDrawCircleBegin() { return drawCircleBegin; }
	void setDrawLineActivated(bool state) { drawLineActivated = state; }
	void setDrawCircleActivated(bool state) { drawCircleActivated = state; }
	bool getDrawLineActivated() { return drawLineActivated; }
	bool getDrawCircleActivated() { return drawCircleActivated; }
	void setDragStart(QPoint start) { DragStart = start; }
	void setDragging(bool state) { isDragging = state; }
	QPoint getDragStart() { return DragStart; }
	bool getDragging() { return isDragging; }
	//Get/Set functions
	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }
	void setPainter() { painter = new QPainter(img); }
	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };
	void clear();

	//custom functions
	double dotProduct(QPoint a, QPoint b) { return a.x() * b.x() + a.y() * b.y(); }
	void DDALine(QPoint start, QPoint end, QColor color);
	void BresenhamLine(QPoint start, QPoint end, QColor color);
	void drawLine(Layer layer);
	void BresenhamCircle(Layer layer);
	void Cyrus_Beck(Layer* layer);
	QVector<QPoint> Sutherland_Hodgeman(Layer* layer);
	void drawPolygon(Layer layer);
	void ZBuffer();
	void Add_Layer();
	void Translation(Layer* layer, int dx, int dy);
	void Flip(Layer* layer);
	void Rotation(Layer* layer, int angle);
	void Scale(Layer* layer, double sx, double sy);
	void Shear(Layer* layer, double shx);
	void Fill(Layer* layer);
	void Fill(QVector<QPoint> points, QColor color);
	void FillCircle(Layer* layer);
	void Rem_Layer(int index);
	void ScanLine(QVector <QPoint> points, QColor color);
	QVector<Layer> Get_Layers() { return Layers; }	
	Layer* Get_Layer(int index) {return &Layers[index]; }
	void Set_Layers(QVector<Layer> layers) { Layers = layers; }
	void Swap_Layers_Depth(int index1, int index2);
	bool Save_Data();
	bool Load_Data();
	void Clear_Data();

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};