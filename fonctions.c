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
    P.dir = (float)rand()/(float)RAND_MAX * 2 * 360;
}

//Distance entre 2 poissons
float distance(struct poisson A, struct poisson B){
    return sqrt( (B.x-A.x)*(B.x-A.x) + (B.y-A.y)*(B.y-A.y) );
}

//Test de l'angle mort
int dans_angle_mort(struct poisson A, struct poisson B){
// On calcule les angles
    if(){ //Le poisson est dans l'angle mort
        return 1; 
    }else{
        return 0;
    }
}

void traitement(int* indices_za, int* indices_zr, int* indices_zo, int indP, double* dir_temp, int N){
    float d_r = 0;
    float d_o = 0;
    float d_a = 0;

    for(int i=0; i<N, i++){
        
    }

}

