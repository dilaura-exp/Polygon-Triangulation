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

	cout << turnVertices[0]->previous->vertex->coordinates->x << " " << turnVertices[0]->previous->vertex->coordinates->y << " YHA\n";
}

void PolygonTriangulation::splitFace(Vertex *vertexFrom, Vertex *vertexTo) {
	Face *newFace = new Face();
	Edge* edgeFrom = vertexFrom->incidentEdge;

	Edge *newEdge1 = new Edge(vertexFrom, nullptr, polygon->faces[polygon->faces.size() - 1], vertexTo->incidentEdge, vertexFrom->incidentEdge->previous);
	newEdge1->previous->next = newEdge1;
	newEdge1->next->previous = newEdge1;
	//newEdge1->id = "Edge-" + std::to_string(idGenerate++);
	vertexFrom->incidentEdge = newEdge1;

	Edge *newEdge2 = new Edge(vertexTo, newEdge1, newFace, edgeFrom, vertexTo->incidentEdge->previous);
	newEdge2->previous->next = newEdge2;
	newEdge2->next->previous = newEdge2;
	//newEdge2->id = "Edge-" + std::to_string(idGenerate++);
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

void PolygonTriangulation::makeMonotone(Polygon *polygon) {
	this->polygon = polygon;
	initVertexTypes();
	for (int i = 0; i < polygon->vertices.size(); i++) {
		events.push(Event(polygon->vertices[i]->type, polygon->vertices[i]));
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


void PolygonTriangulation::triangulateMonotone(Polygon *polygon) {
	this->polygon = polygon;

}