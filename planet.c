#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <plot.h>
#include <float.h>
#include <unistd.h>

typedef struct planetStr {
	char name[20];
	char color[20];
	double size;
	double mass;
	double xPosition;
	double yPosition;
	double xVelocity;
	double yVelocity;
	double newxVelocity;
	double newyVelocity;
	double newxPosition;
	double newyPosition;
	double orbit;
	int passedOrbit;
} planetInfo;

planetInfo* array;

void plotPlanets(plPlotter *plotter, int maxDist) {
	//sleep(.01);
	// turn screen black from previous plot
	//pl_pencolorname_r(plotter,"black");
	//pl_fillcolorname_r(plotter, "black");
	//pl_fbox_r(plotter, -maxDist, -maxDist, maxDist, maxDist);
	//pl_erase_r(plotter);

	// plot new stuff
	pl_pencolorname_r(plotter,"white");
	for (int i = 0; i < 5; i++) {
		pl_fillcolorname_r(plotter, array[i].color);
		pl_fcircle_r(plotter, array[i].xPosition, 
				array[i].yPosition, array[i].size * 150000000000);
	}
}

int main(int argc, char** argv) {

	/* plotter device structures */
  	plPlotter *plotter;
  	plPlotterParams *plotterParams;

  	/* create a plotter parametric structure */
  	plotterParams = pl_newplparams();
  	pl_setplparam(plotterParams, "BITMAPSIZE", "750x750");
  	pl_setplparam(plotterParams, "USE_DOUBLE_BUFFERING", "no");
	pl_setplparam(plotterParams, "BG_COLOR", "black");

	/* create the plotter device and open it */
  	if ((plotter = pl_newpl_r("X", stdin, stdout, stderr, 
			plotterParams)) == NULL) {
    	fprintf(stderr, "Couldn't create Xwindows plotter\n");
    	exit(1);
  	} else if (pl_openpl_r(plotter) < 0) {
    	fprintf(stderr, "Couldn't open Xwindows plotter\n");
    	exit(1);
  	}

	/* reads file to get info about planets */
	FILE* fd = fopen("planet.dat", "r");
	char buf[200];
	fgets(buf, 200, fd);
	int arrayPos = 0;
	double maxDistX = 0;
	double maxDistY = 0;
	array = malloc(sizeof(planetInfo) * 12);
	int pos = 0;
	while(fgets(buf, 200, fd) != NULL) {
		char* first = &buf[0];
		if (*first != '#') {
			double distX = 0;
			double distY = 0;
			sscanf(buf, "%s %s %lf %lf %lf %lf %lf %lf",
					array[arrayPos].name, array[arrayPos].color, 
					&array[arrayPos].size, &array[arrayPos].mass, 
					&array[arrayPos].xPosition, &array[arrayPos].yPosition, 
					&array[arrayPos].xVelocity, &array[arrayPos].yVelocity);	
			if (pos < 5) {
				distX = array[arrayPos].xPosition;			
				distY = array[arrayPos].yPosition;		
				arrayPos++;
				if (fabs(distX) > maxDistX) {
					maxDistX = fabs(distX);
				}
				if (fabs(distY) > maxDistY) {
					maxDistX = fabs(distX);
				}
			}
			pos++;
		}
	}

	double maxDist = 0;
	if (maxDistX > maxDistY) {
		maxDist = maxDistX * 1.3;
	} else {
		maxDist = maxDistY * 1.3;
	}

	/* set our coordinate space in the plotter window */
	pl_fspace_r(plotter, -maxDist, -maxDist, maxDist, maxDist);

	/* pick a type for the pen and the fill */
	pl_pentype_r(plotter,1);
	pl_filltype_r(plotter,1);

	double G = 6.67259 * pow(10,-11);
	double dt = 300.0;
	double ft = 24.0 * 3600 * 10000; //10000 later
	for (int p = 0; p < ft/dt; p++) {
		for (int i = 0; i < 5; i++) {
			double Ax = 0.0;
			double Ay = 0.0;
			for (int j = 0; j < 5; j++) {
				if (i != j) {
					double r = sqrt(pow(array[j].xPosition - 
							array[i].xPosition, 2) + pow(array[j].yPosition 
							- array[i].yPosition, 2));
					Ax += (array[j].mass * G * 
						(array[j].xPosition - array[i].xPosition)) / pow(r, 3);
					Ay += (array[j].mass * G * 
						(array[j].yPosition - array[i].yPosition)) / pow(r, 3);
				}
			}
			array[i].newxVelocity = array[i].xVelocity + dt * Ax;
			array[i].newyVelocity = array[i].yVelocity + dt * Ay;
			array[i].newxPosition = array[i].xPosition + 
					dt * array[i].xVelocity;
			array[i].newyPosition = array[i].yPosition + 
					dt * array[i].yVelocity;
			if (array[i].newyPosition < 0 && array[i].passedOrbit == 0) {
				array[i].passedOrbit = 1;
				printf("%i\n", p / (3600.0 * 24));
			}
		}
		for (int l = 0; l < 5; l++) {
			array[l].xVelocity = array[l].newxVelocity;
			array[l].yVelocity = array[l].newyVelocity;
			array[l].xPosition = array[l].newxPosition;
			array[l].yPosition = array[l].newyPosition;
		}
		if (p % 10 == 0) { 
			plotPlanets(plotter, maxDist); 
		}
	}
			
	/* close and cleanup the plotter stuff */
  	if (pl_closepl_r(plotter) < 0) {
    	fprintf(stderr, "Couldn't close plotter\n");
  	} else if (pl_deletepl_r(plotter) < 0) {
    	fprintf(stderr, "Couldn't delete plotter\n");
  	}
	return 0;
}
