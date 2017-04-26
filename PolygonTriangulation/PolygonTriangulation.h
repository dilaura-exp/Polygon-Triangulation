#pragma once
#include <DCEL.h>
#include <GL/glew.h>
#include <vector>
#include <queue>
#include <iostream>

#include "BinarySearchTree.h"

class Event {
public:
	EventType type;
	Vertex *vertex;

	Event(EventType type, Vertex *vertex) {
		this->type = type;
		this->vertex = vertex;
	}
};

struct EventComparator {
	bool operator()(const Event& event1, const Event& event2) {
		if (event1.vertex->coordinates->y != event2.vertex->coordinates->y) {
			return event1.vertex->coordinates->y < event2.vertex->coordinates->y;
		}
		else {
			return event1.vertex->coordinates->x > event2.vertex->coordinates->x;
		}
	}
};

class Status {
public:
	Edge *edge;
	Vertex *helper;

	Status(Edge *edge, Vertex *helper) {
		this->edge = edge;
		this->helper = helper;
	}
};

class TurnVertex {
public:
	Vertex *vertex;
	TurnVertex *next;
	TurnVertex *previous;

	TurnVertex(Vertex *vertex, TurnVertex *next, TurnVertex* previous) {
		this->vertex = vertex;
		this->next = next;
		this->previous = previous;
	}
};

class PolygonTriangulation {
public:
	PolygonTriangulation();
	~PolygonTriangulation();

	void initPolygon();
	void initVertexTypes();
	void draw();
	void makeMonotone();

private:
	float polygonScale;
	std::vector<Vertex*> vertices;
	std::vector<Edge*> edges;
	std::vector<Face*> faces;
	std::priority_queue<Event, std::vector<Event>, EventComparator> events;
	BinarySearchTree<Status*> status;
	std::vector<TurnVertex*> turnVertices;

	EventType determineVertexType(Vertex &vertex, Vertex &previous, Vertex &next);
	void checkVertexHandler(Event &event);
	void handleStartVertex(Vertex *vertex);
	void handleEndVertex(Vertex *vertex);
	void handleSplitVertex(Vertex *vertex);
	void handleMergeVertex(Vertex *vertex);
	void handleRegularVertex(Vertex *vertex);
};