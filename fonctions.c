/*Contient les fonctions du projet*/
/*
Attention!
Poissons indicés de 0 à N-1

*/
#include <stdlib.h>
#include "math.h"

//Initialisation
void initialisation(struct poisson P, double x_max, double y_max){
    /*Initialise la position et l'orientation du poisson aléatoirement*/
    srand(time(NULL));
    P.x = (float)rand()/(float)RAND_MAX * x_max;
    P.y = (float)rand()/(float)RAND_MAX * y_max;
    P.dir = (float)rand()/(float)RAND_MAX * 2 * M_PI;
}