#include "PolygonTriangulation.h"

PolygonTriangulation::PolygonTriangulation() {

}

PolygonTriangulation::~PolygonTriangulation() {

}

void PolygonTriangulation::makeMonotone() {

	while (!events.empty()) {
		Event *currentEvent = (events.top());
		events.pop();
		checkVertexHandler(currentEvent);
	}
}

void PolygonTriangulation::checkVertexHandler(Event *event) {
	switch (event->type) {
		case EventType::START:
			handleStartVertex(event->vertex);
			break;
		case EventType::END:
			handleEndVertex(event->vertex);
			break;
		case EventType::SPLIT:
			handleSplitVertex(event->vertex);
			break;
		case EventType::MERGE:
			handleMergeVertex(event->vertex);
			break;
		case EventType::REGULAR:
			handleRegularVertex(event->vertex);
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