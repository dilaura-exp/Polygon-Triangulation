#include <time.h>
#include "MainProgram.h"

int main(int argc, char** argv) {

	srand(time(NULL));
	MainProgram mainProgram;
	mainProgram.run();

	return 0;
}