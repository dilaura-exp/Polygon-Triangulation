#include "DCEL.h"
#include <iostream>
#define PI (3.141592653589793)

Polygon::Polygon() {
	polygonScale = 0.08f;
}

Polygon::~Polygon() {

}

void Polygon::initPolygon() {
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
		/*std::string name = "Edge-" + std::to_string(idGenerate++);
		cout << name;
		edges[i]->id = name;*/
		if (i > 0) edges[i - 1]->next = edges[i];

		vertices[i]->incidentEdge = edges[i];
	}
	edges[edges.size() - 1]->next = edges[0];
	edges[0]->previous = edges[edges.size() - 1];
	faces[0]->startingEdge = edges[0];
}