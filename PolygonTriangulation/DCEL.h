#pragma once
#include <vector>

struct Edge;

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
	bool operator < (const Vertex *vertex2) {
		if (coordinates->y != vertex2->coordinates->y) {
			return coordinates->y < vertex2->coordinates->y;
		}
		else {
			return coordinates->x > vertex2->coordinates->x;
		}
	}
};

class Face {
public:
	std::vector<Edge*> edges;
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

