#pragma once
#include <vector>
#include <list>
#include <string>

using namespace std;

extern enum EventType {
	REGULAR, START, END, SPLIT, MERGE
};

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
	EventType type;

	Vertex(Coordinates *coordinates, Edge *incidentEdge) {
		this->coordinates = coordinates;
		this->incidentEdge = incidentEdge;
		type = EventType::REGULAR;
	}
	bool operator < (const Vertex& vertex2) const {
		if (coordinates->y != vertex2.coordinates->y) {
			return coordinates->y < vertex2.coordinates->y;
		}
		else {
			return coordinates->x > vertex2.coordinates->x;
		}
	}
};

class Face {
public:
	Edge* startingEdge;
	Face() {}
};

class Edge {
public:
	std::string id;
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

class Polygon {
public:
	float polygonScale;
	vector<Vertex*> vertices;
	vector<Edge*> edges;
	vector<Face*> faces;

	Polygon();
	~Polygon();

	void initPolygon();
};