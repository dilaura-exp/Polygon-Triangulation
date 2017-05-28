#include "PolygonTriangulation.h"
#include <math.h>


PolygonTriangulation::PolygonTriangulation(MainProgram *mainProgram) {
	this->mainProgram = mainProgram;
}

PolygonTriangulation::~PolygonTriangulation() {

}

void PolygonTriangulation::draw() {
	glColor3f(1, 1, 1);
	for (int i = 0; i < polygon->faces.size(); i++) {
		Edge *currentEdge = polygon->faces[i]->startingEdge;
		glBegin(GL_LINE_LOOP);
		do {
			Vertex *vertex = currentEdge->originVertex;
			glVertex3f(vertex->coordinates->x * polygon->polygonScale, vertex->coordinates->y * polygon->polygonScale, 0);
			currentEdge = currentEdge->next;
		} while (currentEdge != polygon->faces[i]->startingEdge);
		glEnd();
	}
	glPointSize(8);
	glBegin(GL_POINTS);
	for (int i = 0; i<turnVertices.size(); i++) {
		if (turnVertices[i]->vertex->type == EventType::START) {
			glColor3f(0.9f, 0.25f, 0.25f);
		}
		else if (turnVertices[i]->vertex->type == EventType::SPLIT) {
			glColor3f(0.25f, 0.9f, 0.25f);
		}
		else if (turnVertices[i]->vertex->type == EventType::END) {
			glColor3f(0.25f, 0.25f, 0.9f);
		}
		else if (turnVertices[i]->vertex->type == EventType::MERGE) {
			glColor3f(0.9f, 0.5f, 0.9f);
		}
		glVertex3f(turnVertices[i]->vertex->coordinates->x*polygon->polygonScale, turnVertices[i]->vertex->coordinates->y*polygon->polygonScale, 0);
	}
	if (debugVertex != nullptr) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(debugVertex->coordinates->x*polygon->polygonScale, debugVertex->coordinates->y*polygon->polygonScale, 0);
	}
	glEnd();

	glBegin(GL_LINES);
	for (int i = 0; i < status.size(); i++) {
		glColor3f(0.4f, 0.7f, 0.2f);
		glVertex3f(status[i]->edge->originVertex->coordinates->x*polygon->polygonScale, status[i]->edge->originVertex->coordinates->y*polygon->polygonScale, 0);
		glVertex3f(status[i]->edge->next->originVertex->coordinates->x*polygon->polygonScale, status[i]->edge->next->originVertex->coordinates->y*polygon->polygonScale, 0);
	}
	glEnd();

	if (debugEdge != nullptr) {
		glBegin(GL_LINES);
			glColor3f(0.4f, 0.7f, 0.2f);
			glVertex3f(debugEdge->originVertex->coordinates->x*polygon->polygonScale, debugEdge->originVertex->coordinates->y*polygon->polygonScale, 0);
			glVertex3f(debugEdge->next->originVertex->coordinates->x*polygon->polygonScale, debugEdge->next->originVertex->coordinates->y*polygon->polygonScale, 0);
		glEnd();
	}
}

void PolygonTriangulation::initPolygon() {
	
}

void PolygonTriangulation::initVertexTypes() {
	bool upward = polygon->edges[1]->originVertex->coordinates->y > polygon->edges[0]->originVertex->coordinates->y;
	int j = 0;	//turn vertex index
	for (int i = 0; i < polygon->edges.size()+1; i++) {
		Edge *edge;
		if (i == polygon->edges.size()) {
			edge = polygon->edges[0];
		}
		else {
			edge = polygon->edges[i];
		}
		if (upward) {
			if (edge->next->originVertex->coordinates->y < edge->originVertex->coordinates->y) {
				upward = false;
				TurnVertex *previous = nullptr;
				if (j > 0) previous = turnVertices[j - 1];
				turnVertices.push_back(new TurnVertex(edge->originVertex, nullptr, previous));
				determineVertexType(*turnVertices[j]->vertex, *edge->previous->originVertex, *edge->next->originVertex);
				if (j > 0) turnVertices[j - 1]->next = turnVertices[j];
				j++;
			}
		}
		else {
			if (edge->next->originVertex->coordinates->y > edge->originVertex->coordinates->y) {
				upward = true;
				TurnVertex *previous = nullptr;
				if (j > 0) previous = turnVertices[j - 1];
				turnVertices.push_back(new TurnVertex(edge->originVertex, nullptr, previous));
				determineVertexType(*turnVertices[j]->vertex, *edge->previous->originVertex, *edge->next->originVertex);
				if (j > 0) turnVertices[j - 1]->next = turnVertices[j];
				j++;
			}
		}
	}
	turnVertices[turnVertices.size() - 1]->next = turnVertices[0];
	turnVertices[0]->previous = turnVertices[turnVertices.size() - 1];
}

bool isInOrder(Vertex *vertexFrom, Vertex *vertexTo) {
	int i = 0;
	Vertex *currentVertex = vertexFrom;
	while (currentVertex != vertexTo) {
		currentVertex = currentVertex->incidentEdge->next->originVertex;
		i++;
		if (i >= 31) {
			break;
		}
	}
	int j = 0;
	currentVertex = vertexTo;
	while (currentVertex != vertexFrom) {
		currentVertex = currentVertex->incidentEdge->next->originVertex;
		j++;
		if (j >= 31) {
			break;
		}
	}
	if (i < j) {
		return true;
	}
	return false;
}

void PolygonTriangulation::splitFace(Vertex *vertexFrom, Vertex *vertexTo) {
	if (!isInOrder(vertexFrom, vertexTo)) {
		Vertex *temp = vertexFrom;
		vertexFrom = vertexTo;
		vertexTo = temp;
	}
	Face *newFace = new Face();
	Edge* edgeFrom = vertexFrom->incidentEdge;
	Edge* edge2Previous = vertexTo->incidentEdge->previous;

	Edge *newEdge1 = new Edge(vertexFrom, nullptr, edgeFrom->incidentFace, vertexTo->incidentEdge, vertexFrom->incidentEdge->previous);
	newEdge1->previous->next = newEdge1;
	newEdge1->next->previous = newEdge1;

	Edge *newEdge2 = new Edge(vertexTo, newEdge1, newFace, edgeFrom, edge2Previous);
	newEdge2->previous->next = newEdge2;
	newEdge2->next->previous = newEdge2;
	newEdge1->twinEdge = newEdge2;

	polygon->faces.push_back(newFace);

	newFace->startingEdge = newEdge2;
	newEdge1->incidentFace->startingEdge = newEdge1;
}

Status* PolygonTriangulation::searchDirectlyLeftStatus(Vertex *vertex) {
	for (int i = status.size()-1; i >= 0; i--) {
		if (status[i] == nullptr) {
			cout << "NULL";
		}
		if (vertex->coordinates->x >= status[i]->edge->originVertex->coordinates->x) {
			if (vertexPositionToEdge(vertex, status[i]->edge) == 1) {
				return status[i];
			}
		}
	}
	return status[0];
}

EventType PolygonTriangulation::determineVertexType(Vertex &vertex, Vertex &previous, Vertex &next) {
	if (vertex < next && vertex < previous) {
			if (next.coordinates->x >= vertex.coordinates->x &&
				previous.coordinates->x <= vertex.coordinates->x) {
				vertex.type = EventType::END;
				return EventType::END;
			}
			else {
				vertex.type = EventType::MERGE;
				return EventType::MERGE;
			}
	}
	else if (next < vertex && previous < vertex) {
		if (next.coordinates->x <= vertex.coordinates->x &&
			previous.coordinates->x >= vertex.coordinates->x) {
			vertex.type = EventType::START;
			return EventType::START;
		}
		else {
			vertex.type = EventType::SPLIT;
			return EventType::SPLIT;
		}
	}
	return EventType::REGULAR;
}

void PolygonTriangulation::makeMonotone(Polygon *polygon) {
	this->polygon = new Polygon(*polygon);
	initVertexTypes();
	for (int i = 0; i < polygon->vertices.size(); i++) {
		events.push(Event(polygon->vertices[i]->type, polygon->vertices[i]));
	}
	while (!events.empty()) {
		std::cout << std::endl << events.top().vertex->coordinates->x << " " << events.top().vertex->coordinates->y << std::endl;
		Event currentEvent = events.top();
		events.pop();
		checkVertexHandler(currentEvent);
		debugVertex = currentEvent.vertex;
		mainProgram->draw();
		getchar();
	}

	for (int i = 0; i < verticesToSplit.size(); i++) {
		for (int j = 0; j < 2; j++) {
			debugVertex = verticesToSplit[i][j];
			mainProgram->draw();
			getchar();
		}
		splitFace(verticesToSplit[i][0], verticesToSplit[i][1]);
	}
}

void PolygonTriangulation::checkVertexHandler(Event &event) {
	switch (event.type) {
		case EventType::START:
			handleStartVertex(event.vertex);
			break;
		case EventType::END:
			handleEndVertex(event.vertex);
			break;
		case EventType::SPLIT:
			handleSplitVertex(event.vertex);
			break;
		case EventType::MERGE:
			handleMergeVertex(event.vertex);
			break;
		case EventType::REGULAR:
			handleRegularVertex(event.vertex);
			break;
	}
}

void PolygonTriangulation::handleStartVertex(Vertex *vertex) {
	cout << "Start Vertex: " << vertex->incidentEdge->id << endl;
	Status *newStatus = new Status(vertex->incidentEdge, vertex);
	int j = 0;
	for (j = 0; j < status.size(); j++) {
		if (*status[j] > *newStatus) {
			status.insert(status.begin() + j, newStatus);
			cout << "   Insert status at " << j << endl;
			break;
		}
	}
	if (j == status.size()) {
		cout << "   Insert status at " << j << endl;
		status.push_back(newStatus);
	}
}

void PolygonTriangulation::handleEndVertex(Vertex *vertex) {
	Status *statusPreviousEdge = nullptr;
	int i = status.size() - 1;
	for (i = status.size() - 1; i >= 0; i--) {
		if (status[i]->edge == vertex->incidentEdge->previous) {
			statusPreviousEdge = status[i];
			break;
		}
	}
	if (statusPreviousEdge->helper->type == EventType::MERGE) {
		Vertex **v = new Vertex*[2];
		v[0] = vertex;
		v[1] = statusPreviousEdge->helper;
		verticesToSplit.push_back(v);
	}
	status.erase(status.begin() + i);
}

void PolygonTriangulation::handleSplitVertex(Vertex *vertex) {
	Status *leftStatus = searchDirectlyLeftStatus(vertex);
	Vertex **v = new Vertex*[2];
	v[0] = vertex;
	v[1] = leftStatus->helper;
	cout << leftStatus->helper->incidentEdge->id << endl;
	verticesToSplit.push_back(v);
	leftStatus->helper = vertex;
	Status *newStatus = new Status(vertex->incidentEdge, vertex);
	status.push_back(newStatus);
}

void PolygonTriangulation::handleMergeVertex(Vertex *vertex) {
	Status *statusPreviousEdge = nullptr;
	int i = status.size() - 1;
	for (i = status.size() - 1; i >= 0; i--) {
		if (status[i]->edge == vertex->incidentEdge->previous) {
			statusPreviousEdge = status[i];
			break;
		}
	}
	cout << vertex->incidentEdge->id << endl;
	cout << statusPreviousEdge->helper->incidentEdge->id << endl;
	if (statusPreviousEdge->helper->type == EventType::MERGE) {
		Vertex **v = new Vertex*[2];
		v[0] = vertex;
		v[1] = statusPreviousEdge->helper;
		verticesToSplit.push_back(v);
	}
	status.erase(status.begin() + i);
	Status *leftStatus = searchDirectlyLeftStatus(vertex);
	cout << leftStatus->helper->incidentEdge->id << endl;
	if (leftStatus->helper->type == EventType::MERGE) {
		Vertex **v = new Vertex*[2];
		v[0] = vertex;
		v[1] = leftStatus->helper;
		verticesToSplit.push_back(v);
	}
	leftStatus->helper = vertex;
}

void PolygonTriangulation::handleRegularVertex(Vertex *vertex) {
	//if the interior of P lies to the right of vi
	if (vertex->incidentEdge->next->originVertex->coordinates->y <= vertex->coordinates->y) {
		Status *statusPreviousEdge = nullptr;
		int i = status.size() - 1;
		for (i = status.size() - 1; i >= 0; i--) {
			if (status[i]->edge == vertex->incidentEdge->previous) {
				statusPreviousEdge = status[i];
				break;
			}
		}
		if (statusPreviousEdge->helper->type == EventType::MERGE) {
			Vertex **v = new Vertex*[2];
			v[0] = vertex;
			v[1] = statusPreviousEdge->helper;
			verticesToSplit.push_back(v);
		}
		status.erase(status.begin() + i);
		Status *newStatus = new Status(vertex->incidentEdge, vertex);
		status.push_back(newStatus);
	}
	else {
		Status *leftStatus = searchDirectlyLeftStatus(vertex);
		if (leftStatus->helper->type == EventType::MERGE) {
			Vertex **v = new Vertex*[2];
			v[0] = vertex;
			v[1] = leftStatus->helper;
			verticesToSplit.push_back(v);
		}
		leftStatus->helper = vertex;
	}
}

int PolygonTriangulation::vertexPositionToEdge(Vertex *vertex, Edge *edge) {
	Coordinates *point = vertex->coordinates;
	Coordinates *lineStart = edge->originVertex->coordinates;
	Coordinates *lineEnd = edge->next->originVertex->coordinates;
	float positionValue = (lineEnd->x - lineStart->x) * (point->y - lineStart->y) - (lineEnd->y - lineStart->y) * (point->x - lineStart->x);
	if (positionValue > 0) {
		return 1;
	}
	else if (positionValue < 0) {
		return -1;
	}
	else {
		return 0;
	}
}

bool comparePointerToEdge(Edge *a, Edge *b) {
	return (*a < *b);
}

void PolygonTriangulation::triangulateMonotone() {
	for (int i = 0; i < polygon->faces.size(); i++) {
		vector<Edge*> sortedEdges;
		Edge* currentEdge = polygon->faces[i]->startingEdge;
		do {
			cout << currentEdge->id << endl;
			sortedEdges.push_back(currentEdge);
			currentEdge = currentEdge->next;
		} while (currentEdge != polygon->faces[i]->startingEdge);
		sort(sortedEdges.begin(), sortedEdges.end(), comparePointerToEdge);
		reverse(sortedEdges.begin(), sortedEdges.end());
		initVertexDirections(sortedEdges);

		stack<Edge*> s;
		s.push(sortedEdges[0]);
		s.push(sortedEdges[1]);
		for (int j = 2; j < sortedEdges.size() - 1; j++) {
			if (sortedEdges[j]->originVertex->chainDirection != s.top()->originVertex->chainDirection) {
				while (s.size() > 1) {
					Vertex **v = new Vertex*[2];
					v[0] = sortedEdges[j]->originVertex;
					v[1] = s.top()->originVertex;
					verticesToSplit2.push_back(v);
					s.pop();
				}
				s.pop();
				s.push(sortedEdges[j - 1]);
				s.push(sortedEdges[j]);
			}
			else {
				Edge *lastEdge = s.top();
				s.pop();
				while (!s.empty()) {
					lastEdge = s.top();
					if (isInsidePolygon(polygon->faces[i], sortedEdges[j], s.top())) {
						/*if (s.top()->incidentEdge->next->originVertex == sortedVertices[j] || s.top()->incidentEdge->previous->originVertex == sortedVertices[j]) {
							s.pop();
						}*/
						Vertex **v = new Vertex*[2];
						v[0] = sortedEdges[j]->originVertex;
						v[1] = s.top()->originVertex;
						verticesToSplit2.push_back(v);
						s.pop();
					}
					else {
						break;
					}
				}
				s.push(lastEdge);
				s.push(sortedEdges[j]);
			}
		}
		while (!s.empty()) {
			Vertex **v = new Vertex*[2];
			v[0] = sortedEdges[sortedEdges.size() - 1]->originVertex;
			v[1] = s.top()->originVertex;
			verticesToSplit2.push_back(v);
			s.pop();
		}
	}
	for (int i = 0; i < verticesToSplit2.size(); i++) {
		splitFace(verticesToSplit2[i][0], verticesToSplit2[i][1]);
	}
	debugVertex = nullptr;
	debugEdge = nullptr;
}

void PolygonTriangulation::initVertexDirections(vector<Edge*> &sortedEdges) {
	Edge *currentEdge = sortedEdges[0];
	currentEdge->originVertex->chainDirection = 0;
	currentEdge = currentEdge->next;
	while (currentEdge != sortedEdges[sortedEdges.size()-1]) {
		currentEdge->originVertex->chainDirection = -1;
		currentEdge = currentEdge->next;
	}
	currentEdge = sortedEdges[sortedEdges.size()-1];
	currentEdge->originVertex->chainDirection = 0;
	currentEdge = currentEdge->next;
	while (currentEdge != sortedEdges[0]) {
		currentEdge->originVertex->chainDirection = 1;
		currentEdge = currentEdge->next;
	}
}

float PolygonTriangulation::distance(Coordinates *a, Coordinates *b) {
	return sqrt(pow(b->x - a->x, 2) + pow(b->y - a->y, 2));
}
bool PolygonTriangulation::checkRange(Coordinates *p, Coordinates *p1, Coordinates *p2) {
	if (distance(p, p1) <= 0.1f || distance(p, p2) <= 0.1f) {
		return false;
	}

	bool intersectY = false;
	if (p1->y > p2->y) {
		if (p->y > p2->y && p->y < p1->y) {
			intersectY = true;
		}
	}
	else {
		if (p->y > p1->y && p->y < p2->y) {
			intersectY = true;
		}
	}

	bool intersectX = false;
	if (p1->x > p2->x) {
		if (p->x > p2->x && p->x < p1->x) {
			intersectX = true;
		}
	}
	else {
		if (p->x > p1->x && p->x < p2->x) {
			intersectX = true;
		}
	}

	return (intersectX && intersectY);
}
bool PolygonTriangulation::isIntersecting(Coordinates *a1, Coordinates *a2, Coordinates *b1, Coordinates *b2) {
	float m1 = (a2->y - a1->y) / (a2->x - a1->x);
	float c1 = a1->y - m1* a1->x;
	float m2 = (b2->y - b1->y) / (b2->x - b1->x);
	float c2 = b1->y - m2*b1->x;
	float x = (c2 - c1) / (m1 - m2);
	float y = m1 * x + c1;
	Coordinates intersectionPoint(x, y);
	if (checkRange(&intersectionPoint, b1, b2)
		&& checkRange(&intersectionPoint, a1, a2)) {
		cout << "OUTSIDE" << endl;
		return true;
	}
}

float PolygonTriangulation::calculateAngle(Coordinates *a, Coordinates *b, Coordinates *c) {
	float by = b->y - a->y;
	float bx = b->x - a->x;
	float ay = b->y - c->y;
	float ax = b->x - c->x;
	float angle = (atan2(by, bx) - atan2(ay, ax)) * 180 / 3.141592653589793;

	cout << "\nANGLE: " << angle << endl;
	return angle;
}

bool PolygonTriangulation::isInsidePolygon(Face *face, Edge *e1, Edge *e2) {
	debugVertex = e1->originVertex;
	mainProgram->draw();
	getchar();

	debugVertex = e2->originVertex;
	mainProgram->draw();

	if (e1->originVertex->coordinates->x < e2->originVertex->coordinates->x) {
		Edge *temp = e1;
		e1 = e2;
		e2 = temp;
	}

	float angle = calculateAngle(e1->originVertex->coordinates, e2->originVertex->coordinates, e2->previous->originVertex->coordinates);
	if ((angle > 0 && angle < 180) || (angle < -180 && angle > -360)) {
		cout << "OUTSIDE" << endl;
		getchar();
		return false;
	}

	Edge *currentEdge = face->startingEdge;
	do {
		if (isIntersecting(e1->originVertex->coordinates, e2->originVertex->coordinates, currentEdge->originVertex->coordinates, currentEdge->next->originVertex->coordinates)) {
			debugEdge = currentEdge;
			mainProgram->draw();
			getchar();
			return false;
		}
		currentEdge = currentEdge->next;
	} while (currentEdge != face->startingEdge);

	cout << "INSIDE" << endl;
	getchar();
	return true;
}