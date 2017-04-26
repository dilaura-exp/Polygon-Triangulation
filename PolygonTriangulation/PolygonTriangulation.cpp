#include "PolygonTriangulation.h"
#include <math.h>

#define PI (3.141592653589793)

PolygonTriangulation::PolygonTriangulation() {
	polygonScale = 0.08f;
}

PolygonTriangulation::~PolygonTriangulation() {

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
		if (i > 0) edges[i - 1]->next = edges[i];

		faces[0]->edges.push_back(edges[i]);
	}
	edges[edges.size() - 1]->next = edges[0];
	edges[0]->previous = edges[edges.size() - 1];
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
				if (j > 0) turnVertices[j - 1]->next = turnVertices[j];
				j++;
				events.push(Event(EventType::START, edge->originVertex));
			} else {
				events.push(Event(EventType::REGULAR, edge->originVertex));
			}
		}
		else {
			if (edge->next->originVertex->coordinates->y > edge->originVertex->coordinates->y) {
				upward = true;
				TurnVertex *previous = nullptr;
				if (j > 0) previous = turnVertices[j - 1];
				turnVertices.push_back(new TurnVertex(edge->originVertex, nullptr, previous));
				if (j > 0) turnVertices[j - 1]->next = turnVertices[j];
				j++;
				events.push(Event(EventType::START, edge->originVertex));
			}
			else {
				events.push(Event(EventType::REGULAR, edge->originVertex));
			}
		}
	}
	turnVertices[turnVertices.size() - 1]->next = turnVertices[0];
	turnVertices[0]->previous = turnVertices[turnVertices.size() - 1];
}

void PolygonTriangulation::draw() {
	glBegin(GL_LINE_LOOP);
		for (int i = 0; i < faces.size(); i++) {
			for (int j = 0; j < faces[i]->edges.size(); j++) {
				Vertex *vertex = faces[i]->edges[j]->originVertex;
				glVertex3f(vertex->coordinates->x * polygonScale, vertex->coordinates->y * polygonScale, 0);
			}
		}
	glEnd();
	glPointSize(8);
	glBegin(GL_POINTS);
		for (int i=0; i<turnVertices.size(); i++) {
			glVertex3f(turnVertices[i]->vertex->coordinates->x*polygonScale, turnVertices[i]->vertex->coordinates->y*polygonScale, 0);
		}
	glEnd();
}

void PolygonTriangulation::makeMonotone() {
	initVertexTypes();
	for (int i = 0; i < vertices.size(); i++) {

	}
	while (!events.empty()) {
		std::cout << events.top().vertex->coordinates->x << " " << events.top().vertex->coordinates->y << std::endl;
		Event currentEvent = events.top();
		events.pop();
		checkVertexHandler(currentEvent);
	}
}

EventType PolygonTriangulation::determineVertexType(TurnVertex *turnVertex) {

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

}

void PolygonTriangulation::handleEndVertex(Vertex *vertex) {

}

void PolygonTriangulation::handleSplitVertex(Vertex *vertex) {

}

void PolygonTriangulation::handleMergeVertex(Vertex *vertex) {

}

void PolygonTriangulation::handleRegularVertex(Vertex *vertex) {

}