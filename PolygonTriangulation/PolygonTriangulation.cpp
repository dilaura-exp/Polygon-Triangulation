#include "PolygonTriangulation.h"
#include <math.h>

#define PI (3.141592653589793)

PolygonTriangulation::PolygonTriangulation() {
	idGenerate = 0;
	polygonScale = 0.08f;
	//cout << "status awal: " << status.size() << endl;
	//status.clear();
}

PolygonTriangulation::~PolygonTriangulation() {

}

void PolygonTriangulation::draw() {
	for (int i = 0; i < faces.size(); i++) {
		//cout << "face: " << i << endl;
		Edge *currentEdge = faces[i]->startingEdge;
		glBegin(GL_LINE_LOOP);
		do {
			//cout << currentEdge->id << endl;
			Vertex *vertex = currentEdge->originVertex;
			glVertex3f(vertex->coordinates->x * polygonScale, vertex->coordinates->y * polygonScale, 0);
			currentEdge = currentEdge->next;
		} while (currentEdge != faces[i]->startingEdge);
		glEnd();
	}
	glPointSize(8);
	glBegin(GL_POINTS);
	for (int i = 0; i<turnVertices.size(); i++) {
		if (turnVertices[i]->vertex->type == EventType::START) {
			glColor3f(1, 0, 0);
		}
		else if (turnVertices[i]->vertex->type == EventType::SPLIT) {
			glColor3f(0, 1, 0);
		}
		else if (turnVertices[i]->vertex->type == EventType::END) {
			glColor3f(0, 0, 1);
		}
		else if (turnVertices[i]->vertex->type == EventType::MERGE) {
			glColor3f(1, 0, 1);
		}
		glVertex3f(turnVertices[i]->vertex->coordinates->x*polygonScale, turnVertices[i]->vertex->coordinates->y*polygonScale, 0);
	}
	glEnd();
}

void PolygonTriangulation::initPolygon() {
	vertices.push_back(new Vertex(new Coordinates(-9, -4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-6, -4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-6, -3), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-5, -3), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-5, -4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(9, -4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(9, 0), nullptr));
	vertices.push_back(new Vertex(new Coordinates(8, 0), nullptr));
	vertices.push_back(new Vertex(new Coordinates(8, 1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(9, 1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(9, 4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(5, 4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(5, 1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(6, 1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(6, 0), nullptr));
	vertices.push_back(new Vertex(new Coordinates(4, 0), nullptr));
	vertices.push_back(new Vertex(new Coordinates(4, 4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(2, 4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(2, 0), nullptr));
	vertices.push_back(new Vertex(new Coordinates(0, 0), nullptr));
	vertices.push_back(new Vertex(new Coordinates(0, 1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(1, 1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(1, 4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-5, 4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-5, 1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-2, 1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-2, 0), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-5, 0), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-5, -1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-6, -1), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-6, 4), nullptr));
	vertices.push_back(new Vertex(new Coordinates(-9, 4), nullptr));

	double rotation = 45 * PI / 180;
	for (int i = 0; i < vertices.size(); i++) {
		Coordinates *coordinate = vertices[i]->coordinates;
		Coordinates newCoordinate = *coordinate;
		newCoordinate.x = coordinate->x * cos(rotation) - coordinate->y * sin(rotation);
		newCoordinate.y = coordinate->x * sin(rotation) + coordinate->y * cos(rotation);
		coordinate->x = newCoordinate.x;
		coordinate->y = newCoordinate.y;
	}

	faces.push_back(new Face());

	for (int i = 0; i < vertices.size(); i++) {
		Edge *previous = nullptr;
		if (i > 0) previous = edges[i - 1];
		edges.push_back(new Edge(vertices[i], nullptr, faces[0], nullptr, previous));
		std::string name = "Edge-" + std::to_string(idGenerate++);
		cout << name;
		edges[i]->id = name;
		if (i > 0) edges[i - 1]->next = edges[i];

		vertices[i]->incidentEdge = edges[i];
		//faces[0]->startingEdge.push_back(edges[i]);
	}
	edges[edges.size() - 1]->next = edges[0];
	edges[0]->previous = edges[edges.size() - 1];
	faces[0]->startingEdge = edges[0];
	//cout << "ASD: " << edges[0]->originVertex->coordinates->x << endl;
}

void PolygonTriangulation::initVertexTypes() {
	bool upward = edges[1]->originVertex->coordinates->y > edges[0]->originVertex->coordinates->y;
	int j = 0;	//turn vertex index
	for (int i = 0; i < edges.size()+1; i++) {
		Edge *edge;
		if (i == edges.size()) {
			edge = edges[0];
		}
		else {
			edge = edges[i];
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

	cout << turnVertices[0]->previous->vertex->coordinates->x << " " << turnVertices[0]->previous->vertex->coordinates->y << " YHA\n";
}

void PolygonTriangulation::splitFace(Vertex *vertexFrom, Vertex *vertexTo) {
	Face *newFace = new Face();
	Edge* edgeFrom = vertexFrom->incidentEdge;

	Edge *newEdge1 = new Edge(vertexFrom, nullptr, faces[faces.size() - 1], vertexTo->incidentEdge, vertexFrom->incidentEdge->previous);
	newEdge1->previous->next = newEdge1;
	newEdge1->next->previous = newEdge1;
	newEdge1->id = "Edge-" + std::to_string(idGenerate++);
	vertexFrom->incidentEdge = newEdge1;

	Edge *newEdge2 = new Edge(vertexTo, newEdge1, newFace, edgeFrom, vertexTo->incidentEdge->previous);
	newEdge2->previous->next = newEdge2;
	newEdge2->next->previous = newEdge2;
	newEdge2->id = "Edge-" + std::to_string(idGenerate++);
	newEdge1->twinEdge = newEdge2;

	faces.push_back(newFace);

	newFace->startingEdge = newEdge2;
	newEdge1->incidentFace->startingEdge = newEdge1;
}

Status* PolygonTriangulation::searchDirectlyLeftStatus(Vertex *vertex) {
	for (int i = status.size()-1; i >= 0; i--) {
		if (status[i] == nullptr) {
			cout << "NULL";
		}
		if (vertex->coordinates->x >= status[i]->edge->originVertex->coordinates->x) {
			return status[i];
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

void PolygonTriangulation::makeMonotone() {
	initVertexTypes();
	for (int i = 0; i < vertices.size(); i++) {
		events.push(Event(vertices[i]->type, vertices[i]));
	}
	while (!events.empty()) {
		std::cout << events.top().vertex->coordinates->x << " " << events.top().vertex->coordinates->y << std::endl;
		Event currentEvent = events.top();
		events.pop();
		checkVertexHandler(currentEvent);
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
	int i = 0;
	for (i = 0; i < status.size(); i++) {
		if (status[i]->edge == vertex->incidentEdge->previous) {
			statusPreviousEdge = status[i];
			break;
		}
	}
	if (statusPreviousEdge == nullptr) {
		cout << "status previous edge not found.";
		return;
	}
	if (statusPreviousEdge->helper->type == EventType::MERGE) {
		splitFace(vertex, statusPreviousEdge->helper);
	}
	status.erase(status.begin() + i);
}

void PolygonTriangulation::handleSplitVertex(Vertex *vertex) {
	Status *leftStatus = searchDirectlyLeftStatus(vertex);
	splitFace(vertex, leftStatus->helper);
	leftStatus->helper = vertex;
	Status *newStatus = new Status(vertex->incidentEdge, vertex);
	status.push_back(newStatus);
}

void PolygonTriangulation::handleMergeVertex(Vertex *vertex) {
	Status *statusPreviousEdge = nullptr;
	int i = 0;
	for (i = 0; i < status.size(); i++) {
		if (status[i]->edge == vertex->incidentEdge->previous) {
			statusPreviousEdge = status[i];
			break;
		}
	}
	if (statusPreviousEdge == nullptr) {
		cout << "status previous edge not found.";
		return;
	}
	if (statusPreviousEdge->helper->type == EventType::MERGE) {
		splitFace(vertex, statusPreviousEdge->helper);
	}
	status.erase(status.begin() + i);
	Status *leftStatus = searchDirectlyLeftStatus(vertex);
	if (leftStatus->helper->type == EventType::MERGE) {
		splitFace(vertex, leftStatus->helper);
	}
	statusPreviousEdge->helper = vertex;
}

void PolygonTriangulation::handleRegularVertex(Vertex *vertex) {
	//if the interior of P lies to the right of vi
	if (vertex->incidentEdge->next->originVertex->coordinates->y <= vertex->coordinates->y) {
		Status *statusPreviousEdge = nullptr;
		int i = 0;
		for (i = 0; i < status.size(); i++) {
			if (status[i]->edge == vertex->incidentEdge->previous) {
				statusPreviousEdge = status[i];
				break;
			}
		}
		if (statusPreviousEdge == nullptr) {
			cout << "status previous edge not found.";
			return;
		}
		if (statusPreviousEdge->helper->type == EventType::MERGE) {
			splitFace(vertex, statusPreviousEdge->helper);
		}
		status.erase(status.begin() + i);
		Status *newStatus = new Status(vertex->incidentEdge, vertex);
		status.push_back(newStatus);
	}
	else {
		Status *leftStatus = searchDirectlyLeftStatus(vertex);
		if (leftStatus->helper->type == EventType::MERGE) {
			splitFace(vertex, leftStatus->helper);
		}
		leftStatus->helper = vertex;
	}
}


void PolygonTriangulation::triangulateMonotone() {

}