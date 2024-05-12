#pragma once
#include <QtWidgets>


//class representing a layer in the viewer, contains vector of points
class Layer {
	private:
		QVector<QPoint> points;
		int Layer_Depth = 0;
		int type = -1;
		QColor BorderColor = QColor(0, 0, 0);
		QColor FillColor = QColor(255, 255, 255);
		QVector<QVector<QColor>> ZBuffer;
		bool fill_state = false;
		bool border_state = true;
		bool visible_state = true;
		bool selected_state = false;
	public:
		Layer() {};
		Layer(QVector<QPoint> points) : points(points) {};
		int Get_Layer_Depth() { return Layer_Depth; }
		void Set_Layer_Depth(int depth) { Layer_Depth = depth; }
		void Print_Info();
};


class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;
	uchar* data = nullptr;

	bool drawLineActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);
	QVector<Layer*> Layers;

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
	QPoint getDrawLineBegin() { return drawLineBegin; }
	void setDrawLineActivated(bool state) { drawLineActivated = state; }
	bool getDrawLineActivated() { return drawLineActivated; }

	//Get/Set functions
	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }
	void setPainter() { painter = new QPainter(img); }
	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };
	void clear();

	//custom functions

	void Add_Layer();
	QVector<Layer*> Get_Layers() { return Layers; }	
	Layer* Get_Layer(int index) { return Layers[index]; }

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};