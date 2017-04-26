#pragma once
#include <vector>

class Edge;

class Coordinates {
public:
	float x;
	float y;

	Coordinates(float x, float y) {
		this->x = x;
		this->y = y;
	}
};

class Vertex {
public:
	Coordinates *coordinates;
	Edge *incidentEdge;

	Vertex(Coordinates *coordinates, Edge *incidentEdge) {
		this->coordinates = coordinates;
		this->incidentEdge = incidentEdge;
	}
};

class Face {
public:
	std::vector<Edge*> edges;
	Face() {}
};

class Edge {
public:
	Vertex *originVertex;
	Edge *twinEdge;
	Face *incidentFace;
	Edge *next;
	Edge *previous;

	Edge(Vertex *originVertex, Edge *twinEdge, Face *incidentFace, Edge *next, Edge *previous) {
		this->originVertex = originVertex;
		this->twinEdge = twinEdge;
		this->incidentFace = incidentFace;
		this->next = next;
		this->previous = previous;
	}
};

