#include "PolygonTriangulation.h"
#include <math.h>


PolygonTriangulation::PolygonTriangulation() {
	//idGenerate = 0;
}

PolygonTriangulation::~PolygonTriangulation() {

}

void PolygonTriangulation::draw() {
	glColor3f(1, 1, 1);
	for (int i = 0; i < polygon->faces.size(); i++) {
		//cout << "face: " << i << endl;
		Edge *currentEdge = polygon->faces[i]->startingEdge;
		glBegin(GL_LINE_LOOP);
		do {
			//cout << currentEdge->id << endl;
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
	if (highlighter != nullptr) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(highlighter->coordinates->x*polygon->polygonScale, highlighter->coordinates->y*polygon->polygonScale, 0);
	}
	glEnd();

	glBegin(GL_LINES);
		for (int i = 0; i < status.size(); i++) {
			glColor3f(0.4f, 0.7f, 0.2f);
			glVertex3f(status[i]->edge->originVertex->coordinates->x*polygon->polygonScale, status[i]->edge->originVertex->coordinates->y*polygon->polygonScale, 0);
			glVertex3f(status[i]->edge->next->originVertex->coordinates->x*polygon->polygonScale, status[i]->edge->next->originVertex->coordinates->y*polygon->polygonScale, 0);
		}
	glEnd();
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
				//events.push(Event(determineVertexType(*turnVertices[j - 1]), edge->originVertex));
			} else {
				//events.push(Event(EventType::REGULAR, edge->originVertex));
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
				//events.push(Event(determineVertexType(*turnVertices[j-1]), edge->originVertex));
			}
			else {
				//events.push(Event(EventType::REGULAR, edge->originVertex));
			}
		}
	}
	turnVertices[turnVertices.size() - 1]->next = turnVertices[0];
	turnVertices[0]->previous = turnVertices[turnVertices.size() - 1];
}

void PolygonTriangulation::splitFace(Vertex *vertexFrom, Vertex *vertexTo) {
	Face *newFace = new Face();
	Edge* edgeFrom = vertexFrom->incidentEdge;

	Edge *newEdge1 = new Edge(vertexFrom, nullptr, polygon->faces[polygon->faces.size() - 1], vertexTo->incidentEdge, vertexFrom->incidentEdge->previous);
	newEdge1->previous->next = newEdge1;
	newEdge1->next->previous = newEdge1;
	newEdge1->id = "Edge-" + Edge::generateId();
	vertexFrom->incidentEdge = newEdge1;

	Edge *newEdge2 = new Edge(vertexTo, newEdge1, newFace, edgeFrom, vertexTo->incidentEdge->previous);
	newEdge2->previous->next = newEdge2;
	newEdge2->next->previous = newEdge2;
	newEdge2->id = "Edge-" + Edge::generateId();
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
	this->polygon = polygon;
	initVertexTypes();
	for (int i = 0; i < polygon->vertices.size(); i++) {
		events.push(Event(polygon->vertices[i]->type, polygon->vertices[i]));
	}
}

void PolygonTriangulation::nextEvent() {
	if (!events.empty()) {
		std::cout << std::endl << events.top().vertex->coordinates->x << " " << events.top().vertex->coordinates->y << std::endl;
		Event currentEvent = events.top();
		events.pop();
		checkVertexHandler(currentEvent);
		highlighter = currentEvent.vertex;
		if (events.empty()) {
			for (int i = 0; i < verticesToSplit.size(); i++) {
				splitFace(verticesToSplit[i][0], verticesToSplit[i][1]);
			}
			cout << "face count: " << polygon->faces.size() << endl;
			triangulateMonotone(polygon);
		}
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
		cout << "Previouz Edge" << endl;
		Vertex **v = new Vertex*[2];
		v[0] = vertex;
		v[1] = statusPreviousEdge->helper;
		verticesToSplit.push_back(v);
		cout << vertex->incidentEdge->id << " and " << statusPreviousEdge->helper->incidentEdge->id << endl;
	}
	status.erase(status.begin() + i);
	Status *leftStatus = searchDirectlyLeftStatus(vertex);
	cout << leftStatus->helper->incidentEdge->id << endl;
	if (leftStatus->helper->type == EventType::MERGE) {
		cout << "Left Ztatuz" << endl;
		Vertex **v = new Vertex*[2];
		v[0] = vertex;
		v[1] = leftStatus->helper;
		verticesToSplit.push_back(v);
		cout << vertex->incidentEdge->id << " and " << leftStatus->helper->incidentEdge->id << endl;
	}
	leftStatus->helper = vertex;
}

void PolygonTriangulation::handleRegularVertex(Vertex *vertex) {
	//if the interior of P lies to the right of vi
	if (vertex->incidentEdge->next->originVertex->coordinates->y <= vertex->coordinates->y) {
		cout << "LEFT" << endl;
		Status *statusPreviousEdge = nullptr;
		int i = status.size() - 1;
		for (i = status.size() - 1; i >= 0; i--) {
			if (status[i]->edge == vertex->incidentEdge->previous) {
				statusPreviousEdge = status[i];
				break;
			}
		}
		cout << "Type: " << statusPreviousEdge->helper->type << endl;
		if (statusPreviousEdge->helper->type == EventType::MERGE) {
			Vertex **v = new Vertex*[2];
			v[0] = vertex;
			v[1] = statusPreviousEdge->helper;
			verticesToSplit.push_back(v);
			cout << vertex->incidentEdge->id << " and " << statusPreviousEdge->helper->incidentEdge->id << endl;
		}
		status.erase(status.begin() + i);
		Status *newStatus = new Status(vertex->incidentEdge, vertex);
		status.push_back(newStatus);
	}
	else {
		Status *leftStatus = searchDirectlyLeftStatus(vertex);
		cout << "Type: " << leftStatus->helper->type << endl;
		if (leftStatus->helper->type == EventType::MERGE) {
			Vertex **v = new Vertex*[2];
			v[0] = vertex;
			v[1] = leftStatus->helper;
			verticesToSplit.push_back(v);
			cout << vertex->incidentEdge->id << " and " << leftStatus->helper->incidentEdge->id << endl;
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

void PolygonTriangulation::triangulateMonotone(Polygon *polygon) {
	this->polygon = polygon;
	vector<Vertex> sortedVertices;
	for (int i = 0; i < polygon->vertices.size(); i++) {
		sortedVertices.push_back(*polygon->vertices[i]);
	}
	sort(sortedVertices.begin(), sortedVertices.end());
	reverse(sortedVertices.begin(), sortedVertices.end());

	
}

void initVertexDirections();