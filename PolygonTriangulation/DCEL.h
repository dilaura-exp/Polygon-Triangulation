#pragma once
#include <vector>

struct Edge;

struct Coordinates {
	float x;
	float y;

	Coordinates(float x, float y) {
		this->x = x;
		this->y = y;
	}
};

struct Vertex {
	Coordinates coordinates;
	Edge *incidentEdge;
};

struct Face {
	std::vector<Edge*> edges;
};

struct Edge {
	Vertex *originVertex;
	Edge *twinEdge;
	Face *incidentFace;
	Edge *next;
	Edge *previous;
};

