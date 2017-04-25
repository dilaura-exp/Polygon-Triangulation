#pragma once
#include <DCEL.h>
#include <vector>
#include <queue>

#include "BinarySearchTree.h"

enum EventType {
	START, END, SPLIT, MERGE, REGULAR
};

class Event {
public:
	EventType type;
	Vertex *vertex;
};

class Status {
public:
	Edge *edge;
	Vertex *helper;
};

class PolygonTriangulation {
public:
	PolygonTriangulation();
	~PolygonTriangulation();

	void makeMonotone();

private:
	std::vector<Vertex*> vertices;
	std::priority_queue<Event*> events;
	BinarySearchTree<Status*> status;

	void checkVertexHandler(Event *event);
	void handleStartVertex(Vertex *vertex);
	void handleEndVertex(Vertex *vertex);
	void handleSplitVertex(Vertex *vertex);
	void handleMergeVertex(Vertex *vertex);
	void handleRegularVertex(Vertex *vertex);
};