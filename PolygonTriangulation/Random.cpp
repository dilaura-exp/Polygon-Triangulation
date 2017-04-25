#include "Random.h"
#include <cstdlib>

float randomRange(float low, float high) {
	return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
}

int randomRange(int low, int high) {
	return rand() % (high - low) + low;
}