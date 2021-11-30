#include "class/MMV_Viewer/MMV_Viewer.h"





int main(int argc, char** argv) {
	MMV_Viewer runner;
	runner.run();
	runner.quit();
	return 0;
}

/*
couleur pixel carte relief
g(p) = 0.5 + 0.35*( (na p dot l + 1) / 2) + 0.15* (-delta)
*/
