#define MAINPROGRAM
#include "variables.h"
#include "readfile.h"
#include "raytrace.h"
#include <time.h>


int main(int argc, char* argv[]){
	if (argc < 2) {
		cerr << "Usage: transforms scenefile [grader input (optional)]\n";
		exit(-1);
	}
	clock_t start = clock();
	readfile(argv[1]);
	rayTrace();
	clock_t end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;
	cout << time << "\n";

	return 0;
}