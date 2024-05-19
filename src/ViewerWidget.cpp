#include   "ViewerWidget.h"

void Layer::Print_Info() {
	qDebug() << "Layer Depth: " << Layer_Depth;
	qDebug() << "Points: ";
	for (int i = 0; i < points.size(); i++) {
		qDebug() << points[i];
	}
	qDebug() << "Border color: " << BorderColor;
	qDebug() << "Fill color: " << FillColor;
	qDebug() << "Fill: " << fill_state;
	qDebug() << "";

}
ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent): QWidget(parent){
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget(){
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size){
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}
//Image functions
bool ViewerWidget::setImage(const QImage& inputImg){
	if (img != nullptr) {
		delete painter;
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	setDataPtr();
	update();

	return true;
}
bool ViewerWidget::isEmpty(){
	if (img == nullptr) {
		return true;
	}

	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}
bool ViewerWidget::changeSize(int width, int height){
	QSize newSize(width, height);

	if (newSize != QSize(0, 0)) {
		if (img != nullptr) {
			delete painter;
			delete img;
		}

		img = new QImage(newSize, QImage::Format_ARGB32);
		if (!img) {
			return false;
		}
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
		update();
	}

	return true;
}
void ViewerWidget::setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a){
	r = r > 255 ? 255 : (r < 0 ? 0 : r);
	g = g > 255 ? 255 : (g < 0 ? 0 : g);
	b = b > 255 ? 255 : (b < 0 ? 0 : b);
	a = a > 255 ? 255 : (a < 0 ? 0 : a);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = b;
	data[startbyte + 1] = g;
	data[startbyte + 2] = r;
	data[startbyte + 3] = a;
}
void ViewerWidget::setPixel(int x, int y, double valR, double valG, double valB, double valA){
	valR = valR > 1 ? 1 : (valR < 0 ? 0 : valR);
	valG = valG > 1 ? 1 : (valG < 0 ? 0 : valG);
	valB = valB > 1 ? 1 : (valB < 0 ? 0 : valB);
	valA = valA > 1 ? 1 : (valA < 0 ? 0 : valA);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = static_cast<uchar>(255 * valB);
	data[startbyte + 1] = static_cast<uchar>(255 * valG);
	data[startbyte + 2] = static_cast<uchar>(255 * valR);
	data[startbyte + 3] = static_cast<uchar>(255 * valA);
}
void ViewerWidget::setPixel(int x, int y, const QColor& color){
	if (color.isValid()) {
		size_t startbyte = y * img->bytesPerLine() + x * 4;

		data[startbyte] = color.blue();
		data[startbyte + 1] = color.green();
		data[startbyte + 2] = color.red();
		data[startbyte + 3] = color.alpha();
	}
}
//Draw functions
void ViewerWidget::drawLine(QPoint start, QPoint end, QColor color, int algType){
	painter->setPen(QPen(color));
	if (algType == 0) {
		DDALine(start, end, color);
	}
	else if (algType == 1) {
		BresenhamLine(start, end, color);
	}
	update();
}
void ViewerWidget::clear(){
	img->fill(Qt::white);
	update();
}
//Slots
void ViewerWidget::paintEvent(QPaintEvent* event){
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}
//custom functions
void ViewerWidget::drawLine(Layer layer){
	if (layer.Get_Draw_Type() == 0) {
		DDALine(layer.Get_Points()[0], layer.Get_Points()[1], layer.Get_Border_Color());
	}
	if (layer.Get_Points().size() == 1) {
		BresenhamLine(layer.Get_Points()[0], layer.Get_Points()[1], layer.Get_Border_Color());
	}
}
void ViewerWidget::DDALine(QPoint start, QPoint end, QColor color) {
	float dx = end.x() - start.x();
	float dy = end.y() - start.y();

	// Calculate the number of steps
	float steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	// Increment for each step
	float xinc = dx / steps;
	float yinc = dy / steps;

	float x = start.x();
	float y = start.y();

	for (int i = 0; i < steps; i++) {
		// Check if coordinates are within the clipping region
		if (isInside(round(x), round(y))) {
			setPixel(round(x), round(y), color);
		}

		x += xinc;
		y += yinc;
	}
}
void ViewerWidget::BresenhamLine(QPoint start, QPoint end, QColor color) {
	int x0 = start.x();
	int y0 = start.y();
	int x1 = end.x();
	int y1 = end.y();

	// výpoèet smernice
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int step_x = (x0 < x1) ? 1 : -1;
	int step_y = (y0 < y1) ? 1 : -1;

	int error = dx - dy;

	while (true) {
		setPixel(x0, y0, color);
		if (x0 == x1 && y0 == y1)
			break;
		int double_error = 2 * error;

		if (double_error > -dy) {
			error -= dy;
			x0 += step_x;
		}
		if (double_error < dx) {
			error += dx;
			y0 += step_y;
		}
	}
}
void ViewerWidget::BresenhamCircle(Layer layer) {
	QPoint start = layer.Get_Points()[0];
	QPoint end = layer.Get_Points()[1];
	QColor color = layer.Get_Border_Color();
	int r = sqrt(pow(end.x() - start.x(), 2) + pow(end.y() - start.y(), 2)); //polomer kružnice
	int x = 0;
	int y = r;
	int p = 1 - r;

	while (x <= y) {
		// kontrola hraníc kružnice
		if (isInside(start.x() + x, start.y() + y)) setPixel(start.x() + x, start.y() + y, color);
		if (isInside(start.x() - x, start.y() + y)) setPixel(start.x() - x, start.y() + y, color);
		if (isInside(start.x() + x, start.y() - y)) setPixel(start.x() + x, start.y() - y, color);
		if (isInside(start.x() - x, start.y() - y)) setPixel(start.x() - x, start.y() - y, color);
		if (isInside(start.x() + y, start.y() + x)) setPixel(start.x() + y, start.y() + x, color);
		if (isInside(start.x() - y, start.y() + x)) setPixel(start.x() - y, start.y() + x, color);
		if (isInside(start.x() + y, start.y() - x)) setPixel(start.x() + y, start.y() - x, color);
		if (isInside(start.x() - y, start.y() - x)) setPixel(start.x() - y, start.y() - x, color);

		if (p > 0) {
			p += 2 * (x - y) + 5;
			y--;
		}
		else {
			p += 2 * x + 3;
		}
		x++;
	}
	if (layer.Get_Fill_State()) {
		FillCircle(&layer);
	}
}
void ViewerWidget::Cyrus_Beck(Layer* layer) {
	QVector<QPoint> points = layer->Get_Points();
	// Define vector D as the vector from the first point to the second point
	QPoint D = points[1] - points[0];

	// Initialize tl (t lower bound) to 0 and tu (t upper bound) to 1
	double tl = 0;
	double tu = 1;
	// Define the boundary of the clipping region (in this case, a rectangle)
	QVector<QPoint> plane = { QPoint(0, 0), QPoint(0, img->height()), QPoint(img->width(), img->height()), QPoint(img->width(), 0) };

	// Iterate over each edge of the clipping rectangle
	for (int i = 0; i < 4; i++) {
		QPoint E0 = plane[i];
		QPoint E1 = plane[(i + 1) % 4];

		// Define the vector representing the current edge (E1 - E0)
		QPoint E(E1 - E0);

		// Calculate the inside normal vector N to the current edge
		QPoint N(E.y(), -E.x());
		double dotDN = dotProduct(D, N);
		double dotWN = dotProduct((points[0] - E0), N);

		// If the dot product of D and N is not 0 (i.e., the line is not parallel to the edge)
		if (dotDN != 0) {
			double t = -dotWN / dotDN;
			if (dotDN > 0 && t <= 1) {
				tl = std::max(t, tl);
			}
			else if (dotDN < 0 && t >= 0) {
				tu = std::min(t, tu);
			}
		}
	}
	if (tl == 0 && tu == 1) {
		drawLine(points[0], points[1], layer->Get_Border_Color(), layer->Get_Draw_Type());
	}

	else if (tl < tu) {
		QPoint newStart = points[0] + D * tl;
		QPoint newEnd = points[0] + D * tu;
		drawLine(newStart, newEnd, layer->Get_Border_Color(), layer->Get_Draw_Type());
	}
}
QVector<QPoint> ViewerWidget::Sutherland_Hodgeman(Layer* layer) {
	QVector<QPoint> W; // Initialize a vector to hold the vertices of the clipped polygon
	QVector<QPoint> polygon = layer->Get_Points();
	int edges[4] = { 0,0,-499,-499 };

	for (int j = 0; j < 4; j++) {
		if (polygon.size() == 0) {
			break;
		}
		QPoint S = polygon[polygon.size() - 1]; // Initialize S to the last vertex
		double xmin = edges[j];
		for (int i = 0; i < polygon.size(); i++) {
			QPoint Vi = polygon[i];

			if (Vi.x() >= xmin) {
				if (S.x() >= xmin) {
					W.append(Vi);
				}
				else {
					// Calculate the intersection point P between the clipping edge and the line segment formed by S and Vi
					QPoint P = QPoint(xmin, S.y() + (xmin - S.x()) * (Vi.y() - S.y()) / (double)(Vi.x() - S.x()));
					W.append(P);
					W.append(Vi);
				}
			}
			else {
				if (S.x() >= xmin) {
					QPoint P = QPoint(xmin, S.y() + (xmin - S.x()) * (Vi.y() - S.y()) / (double)(Vi.x() - S.x()));
					W.append(P);
				}
			}
			S = Vi;
		}
		polygon = W;
		W.clear();
		// Rotate the polygon clockwise to handle the next clipping edge
		for (int k = 0; k < polygon.size(); k++) {
			QPoint swap = polygon[k];
			polygon[k].setX(swap.y());
			polygon[k].setY(-swap.x());
		}
	}

	for (int i = 0; i < polygon.size() - 1; i++) {
		drawLine(polygon[i], polygon[i + 1], layer->Get_Border_Color(), layer->Get_Draw_Type());
	}
	if (polygon.size() > 1) {
		drawLine(polygon[polygon.size() - 1], polygon[0], layer->Get_Border_Color(), layer->Get_Draw_Type());
	}
	update();
	return polygon;

}
QVector<QPoint> ViewerWidget::Sutherland_Hodgeman(QVector<QPoint> points, QColor color, int alg) {
	QVector<QPoint> W; // Initialize a vector to hold the vertices of the clipped polygon
	QVector<QPoint> polygon = points;
	int edges[4] = { 0,0,-499,-499 };

	for (int j = 0; j < 4; j++) {
		if (polygon.size() == 0) {
			break;
		}
		QPoint S = polygon[polygon.size() - 1]; // Initialize S to the last vertex
		double xmin = edges[j];
		for (int i = 0; i < polygon.size(); i++) {
			QPoint Vi = polygon[i];

			if (Vi.x() >= xmin) {
				if (S.x() >= xmin) {
					W.append(Vi);
				}
				else {
					// Calculate the intersection point P between the clipping edge and the line segment formed by S and Vi
					QPoint P = QPoint(xmin, S.y() + (xmin - S.x()) * (Vi.y() - S.y()) / (double)(Vi.x() - S.x()));
					W.append(P);
					W.append(Vi);
				}
			}
			else {
				if (S.x() >= xmin) {
					QPoint P = QPoint(xmin, S.y() + (xmin - S.x()) * (Vi.y() - S.y()) / (double)(Vi.x() - S.x()));
					W.append(P);
				}
			}
			S = Vi;
		}
		polygon = W;
		W.clear();
		// Rotate the polygon clockwise to handle the next clipping edge
		for (int k = 0; k < polygon.size(); k++) {
			QPoint swap = polygon[k];
			polygon[k].setX(swap.y());
			polygon[k].setY(-swap.x());
		}
	}

	for (int i = 0; i < polygon.size() - 1; i++) {
		drawLine(polygon[i], polygon[i + 1], color, alg);
	}
	if (polygon.size() > 1) {
		drawLine(polygon[polygon.size() - 1], polygon[0], color, alg);
	}
	update();
	return polygon;
}
void ViewerWidget::drawPolygon(Layer layer) {
	for (int i = 0; i < layer.Get_Points().size(); i++) {
		if (i == layer.Get_Points().size() - 1) {
			drawLine(layer.Get_Point(i), layer.Get_Point(0), layer.Get_Border_Color(), layer.Get_Draw_Type());
		}
		else {
			drawLine(layer.Get_Point(i), layer.Get_Point(i + 1), layer.Get_Border_Color(), layer.Get_Draw_Type());
		}
	}
	QVector<QPoint> clippedPolygon = Sutherland_Hodgeman(&layer);
	// Fill the polygon if the fill state is true
	if (layer.Get_Fill_State()) {
		Fill(clippedPolygon, layer.Get_Fill_Color());
	}
}
void ViewerWidget::drawBezier(Layer layer) {
	QVector<QPoint> points = layer.Get_Points();
	if (points.size() < 3) {
		return;
	}
	QColor color = layer.Get_Border_Color();
	double t = 0;
	double deltaT = 0.001;
	QPoint Q0 = points[0];

	while (t <= 1) {
		QVector<QPoint> tempPoints = points;

		for (int i = 1; i < tempPoints.size(); i++) {
			for (int j = 0; j < tempPoints.size() - i; j++) {
				tempPoints[j] = tempPoints[j] * (1 - t) + tempPoints[j + 1] * t;
			}
		}

		QPoint p = tempPoints[0];
		// Assuming isInside() and setPixel() are globally accessible functions
		if (isInside(p.x(), p.y())) {
			setPixel(p.x(), p.y(), color);
		}

		Q0 = p;
		t += deltaT;
	}

	// Ensure the curve ends at the last control point
	if (t < 1.0) {
		QPoint lastPoint = points.last();
		if (isInside(lastPoint.x(), lastPoint.y())) {
			setPixel(lastPoint.x(), lastPoint.y(), color);
		}
	}
}
void ViewerWidget::drawSquare(Layer layer) {
	//draw square based on 2 diagonal points
	QPoint start = layer.Get_Point(0);
	QPoint end = layer.Get_Point(1);
	QColor color = layer.Get_Border_Color();
	drawLine(start, QPoint(start.x(), end.y()), color, layer.Get_Draw_Type());
	drawLine(start, QPoint(end.x(), start.y()), color, layer.Get_Draw_Type());
	drawLine(end, QPoint(start.x(), end.y()), color, layer.Get_Draw_Type());
	drawLine(end, QPoint(end.x(), start.y()), color, layer.Get_Draw_Type());
	QVector<QPoint> points = { start, QPoint(start.x(), end.y()), end, QPoint(end.x(), start.y()) };
	QVector<QPoint> clippedPolygon = Sutherland_Hodgeman(points, layer.Get_Border_Color(), layer.Get_Draw_Type());
	if (layer.Get_Fill_State()) {
		Fill(clippedPolygon, layer.Get_Fill_Color());
	}
}
void ViewerWidget::Swap_Layers_Depth(int index1, int index2) {
	int temp = Layers[index1].Get_Layer_Depth();
	Layers[index1].Set_Layer_Depth(Layers[index2].Get_Layer_Depth());
	Layers[index2].Set_Layer_Depth(temp);

}
void ViewerWidget::Add_Layer() {
	Layer newLayer = Layer();
	int max_depth = 0;
	for (int i = 0; i < Layers.size(); i++) {
		if (Layers[i].Get_Layer_Depth() > max_depth) {
			max_depth = Layers[i].Get_Layer_Depth();
		}
	}
	newLayer.Set_LayerID(max_depth+1);
	newLayer.Set_Layer_Depth(max_depth+1);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 255);
	newLayer.Set_Border_Color(QColor(dis(gen), dis(gen), dis(gen)));
	newLayer.Set_Fill_Color(QColor(dis(gen), dis(gen), dis(gen)));
	Layers.push_back(newLayer);
}
void ViewerWidget::Rem_Layer(int index) {
	Layers.erase(Layers.begin() + index);
}
void ViewerWidget::ZBuffer() {
	clear();
	for (int i = 0; i < Layers.size(); i++) {
		Layer layer = Layers[i];
		if (layer.Get_Points().size() == 2 && layer.Get_Type() == 0) {
			drawLine(layer);
		}
		if (layer.Get_Points().size() == 2 && layer.Get_Type() == 1){
			BresenhamCircle(layer);
		}
		if (layer.Get_Points().size() > 2 && layer.Get_Type() == 2) {
			drawPolygon(layer);
		}
		if (layer.Get_Type() == 3) {
			drawBezier(layer);
		}
		if (layer.Get_Type() == 4) {
			drawSquare(layer);
		}
	}
	update();
}
void ViewerWidget::Translation(Layer* layer, int dx, int dy) {
	for (int i = 0; i < layer->Get_Points().size(); i++) {
		layer->Set_Point(i, dx, dy);
	}
	update();
}
void ViewerWidget::Flip(Layer* layer) {
	QVector<QPoint> points = layer->Get_Points();
	if (layer->Get_Points().size() == 0 || layer->Get_Points().size() == 1) {
		return;
	}
	if (layer->Get_Points().size() == 2) {
		int new_x = layer->Get_Point(1).x() + 2 * (layer->Get_Point(0).x() - layer->Get_Point(1).x());
		QPoint p(new_x, layer->Get_Point(1).y());
		layer->Set_Point(1, p);
	}
	else {
		double A = points[1].y() - points[0].y();
		double B = points[0].x() - points[1].x();
		double C = -A * points[0].x() - B * points[0].y();
		for (int i = 2; i < points.size(); i++) {
			double d = A * points[i].x() + B * points[i].y() + C;
			double new_x = points[i].x() - 2 * A * d / (A * A + B * B);
			double new_y = points[i].y() - 2 * B * d / (A * A + B * B);
			QPoint p(new_x, new_y);
			layer->Set_Point(i, p);
		}
	}
	update();
}
void ViewerWidget::ScanLine(QVector<QPoint> points, QColor color) {
	QVector<QPoint> polygon = points;
	QVector<Edge> edges;

	if (polygon.size() < 3) return;

	// Loop through each vertex of the polygon to generate edges
	for (int i = 0; i < polygon.size(); i++) {
		int next = (i + 1) % polygon.size();
		// Skip horizontal edges
		if (polygon[i].y() == polygon[next].y())
			continue;
		QPoint start = polygon[i].y() < polygon[next].y() ? polygon[i] : polygon[next];
		QPoint end = polygon[i].y() < polygon[next].y() ? polygon[next] : polygon[i];
		// Adjust the end point slightly to ensure correct edge classification
		end.setY(end.y() - 1);
		double dx = (end.x() - start.x());
		if (dx == 0) dx = 1 / DBL_MAX;

		double m = (double)(end.y() - start.y()) / dx;
		// Create an edge struct and add it to the vector
		Edge edge = { start, end, m };
		edges.push_back(edge);
	}
	// Sort edges based on the starting y-coordinate
	std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
		return e1.start.y() < e2.start.y();
		});

	// Find the minimum and maximum y-coordinates of the bounding box
	int ymin = edges.front().start.y();
	int ymax = edges.front().end.y();

	for (int i = 1; i < edges.size(); i++) {
		if (edges[i].start.y() < ymin)
			ymin = edges[i].start.y();
		if (edges[i].end.y() > ymax)
			ymax = edges[i].end.y();
	}

	QVector<QVector<Edge>> TH(ymax - ymin + 1);

	if (TH.size() < 2) return;

	for (int i = 0; i < edges.size(); i++) {
		int delta_y = edges[i].end.y() - edges[i].start.y();
		double x = edges[i].start.x();
		double w = 0;
		if (edges[i].m == 0) {
			w = 0;
		}
		else {
			w = (double)1.0 / edges[i].m;
		}
		edges[i].delta_y = delta_y;
		edges[i].x = x;
		edges[i].w = w;

		TH[(edges[i].start.y() - ymin)].push_back(edges[i]);
	}

	QVector<Edge> ZAH;
	// Scan convert each scanline
	int y = ymin;

	for (int i = 0; i < ymax - ymin; i++) {
		// Add active edges from TH to ZAH
		if (!TH[i].empty()) {

			for (const Edge& edge : TH[i]) {
				ZAH.push_back(edge);
			}
		}

		std::sort(ZAH.begin(), ZAH.end(), [](const Edge& e1, const Edge& e2) {
			return e1.x < e2.x;
			});

		// Fill scanline between pairs of edges in ZAH
		for (int j = 0; j < ZAH.size(); j += 2) {

			if (j + 1 < ZAH.size() && ZAH[j].x != ZAH[j + 1].x) {

				for (int k = round(ZAH[j].x); k <= round(ZAH[j + 1].x); k++) {
					if (isInside(k, y)) {
						setPixel(k, y, color);
					}
				}
			}
		}
		// Update x-coordinates and decrement delta_y for remaining edges in ZAH
		for (int j = 0; j < ZAH.size(); j++) {
			if (ZAH[j].delta_y == 0) {
				ZAH.erase(ZAH.begin() + j);
				j--;
			}
		}

		for (int j = 0; j < ZAH.size(); j++) {
			ZAH[j].delta_y--;
			ZAH[j].x += ZAH[j].w;
		}
		y++;
	}
}
void ViewerWidget::Rotation(Layer* layer, int Angle) {
	double angle = Angle * M_PI / 180;
	QPoint center = layer->Get_Point(0);
	for (int i = 1; i < layer->Get_Points().size(); i++) {
		int x = center.x() + (layer->Get_Point(i).x() - center.x()) * cos(angle) - (layer->Get_Point(i).y() - center.y()) * sin(angle);
		int y = center.y() + (layer->Get_Point(i).x() - center.x()) * sin(angle) + (layer->Get_Point(i).y() - center.y()) * cos(angle);
		QPoint p(x, y);
		layer->Set_Point(i, p);
	}
}
void ViewerWidget::Scale(Layer* layer, double Sx, double Sy) {
	QPoint center = layer->Get_Point(0);
	for (int i = 1; i < layer->Get_Points().size(); i++) {
		int x = center.x() + (layer->Get_Point(i).x() - center.x()) * Sx;
		int y = center.y() + (layer->Get_Point(i).y() - center.y()) * Sy;
		QPoint p(x, y);
		layer->Set_Point(i, p);
	}
}
void ViewerWidget::Shear(Layer* layer, double shx) {
	QPoint center = layer->Get_Point(0);
	for (int i = 1; i < layer->Get_Points().size(); i++) {
		int x = layer->Get_Point(i).x() + shx * layer->Get_Point(i).y();
		int y = layer->Get_Point(i).y();
		QPoint p(x, y);
		layer->Set_Point(i, p);
	}
}
void ViewerWidget::Fill(Layer* layer) {
	QVector<QPoint> points = layer->Get_Points();
	if (points.size() < 3) {
		return;
	}
	QColor color = layer->Get_Fill_Color();
	ScanLine(points, color);
}
void ViewerWidget::Fill(QVector<QPoint> points,QColor color) {
	ScanLine(points, color);

}
void ViewerWidget::FillCircle(Layer* layer) {
	//fill circle with color
	int radius = sqrt(pow(layer->Get_Points()[1].x() - layer->Get_Points()[0].x(), 2) + pow(layer->Get_Points()[1].y() - layer->Get_Points()[0].y(), 2));
	for (int i = -radius; i <= radius; i++) {
		for (int j = -radius; j <= radius; j++) {
			if (i * i + j * j <= radius * radius) {
				if (isInside(layer->Get_Points()[0].x() + i, layer->Get_Points()[0].y() + j)) {
					setPixel(layer->Get_Points()[0].x() + i, layer->Get_Points()[0].y() + j, layer->Get_Fill_Color());
				}
			}
		}
	}
}
bool ViewerWidget::Save_Data() {
	QFile file = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "File not opened";
		return false;
	}
	QTextStream out(&file);
	out << img->width() << " " << img->height() << "\n";
	out << Layers.size() << "\n";
	for (int i = 0; i < Layers.size(); i++) {
		Layer layer = Layers[i];
		out << layer.Get_LayerID() << ";";
		out << layer.Get_Layer_Depth() << ";";
		out << layer.Get_Type() << ";";
		out << layer.Get_Points().size() << ";";
		for (int j = 0; j < layer.Get_Points().size(); j++) {
			out << layer.Get_Point(j).x() << "," << layer.Get_Point(j).y() << ",";
		}
		out << ";";
		out << layer.Get_Border_Color().red() << "," << layer.Get_Border_Color().green() << "," << layer.Get_Border_Color().blue() << ";";
		out << layer.Get_Fill_Color().red() << "," << layer.Get_Fill_Color().green() << "," << layer.Get_Fill_Color().blue() << ";";
		out << layer.Get_Fill_State() << "\n";
	}
	return true;
}
bool ViewerWidget::Load_Data() {
	//choose saving file
	QFile file = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "File not opened";
		return false;
	}
	Clear_Data();
	QTextStream in(&file);
	QString line = in.readLine();
	QStringList list = line.split(" ");
	int width = list[0].toInt();
	int height = list[1].toInt();
	changeSize(width, height);
	int layers = in.readLine().toInt();
	for (int i = 0; i < layers; i++) {
		Layer layer;
		QString line = in.readLine();
		QStringList list = line.split(";");
		layer.Set_LayerID(list[0].toInt());
		layer.Set_Layer_Depth(list[1].toInt());
		layer.Set_Type(list[2].toInt());
		int points = list[3].toInt();
		QStringList pointsList = list[4].split(",");
		for (int j = 0; j < 2*points - 1; j+=2 ) {
			QPoint p(pointsList[j].toFloat(), pointsList[j + 1].toFloat());
			layer.Add_Point(p);
		}
		QStringList borderList = list[5].split(",");
		QColor border(borderList[0].toInt(), borderList[1].toInt(), borderList[2].toInt());
		layer.Set_Border_Color(border);
		QStringList fillList = list[6].split(",");
		QColor fill(fillList[0].toInt(), fillList[1].toInt(), fillList[2].toInt());
		layer.Set_Fill_Color(fill);
		layer.Set_Fill_State(list[7].toInt());
		Layers.push_back(layer);
	}

	return true;
}
void ViewerWidget::Clear_Data() {
	Layers.clear();
	clear();
	update();
}