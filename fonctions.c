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
    P.dir = (float)rand()/(float)RAND_MAX * 360;
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

/* --------------------------------
Fonctions nécessaires au traitement
*/
double repulsion(int* indices_zr, int N, struct poisson* banc){
    int compt = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_zr[j]*(banc[j].dir+180); // +180 ~ *-1
                compt++;
            }
            return(tmp/compt);
}

double attraction(int* indices_za, int N, struct poisson* banc){
    int compt = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_za[j]*(banc[j].dir);
                compt++;
            }
            return(tmp/compt);
}

double orientation(int* indices_zo, int N, struct poisson* banc){
    
}


void traitement(int* indices_za, int* indices_zr, int* indices_zo, double* dir_temp, int N, struct poisson* banc, int indP, float s){
    /* indices_za, indices_zr et indices_zo indiquent la position des poissons par rapport aux zones
    du poisson en question.
    
    J'ai ajouté le banc de poisson qu'on a oublié.*/
    double d_r = 0;
    double d_o = 0;
    double d_a = 0;

    double tmp = 0; // Variable temporaire

    // Pour éviter de créer des boucles trop similaires, je crée trois autres fonctions : repulsion(), attraction() et orientation().

    for(int i=0; i<N; i++){
        if(indices_zr[i]){// Si au moins un poisson est dans la zone de répulsion, on ne fait pas les autres boucles.
            dir_temp[indP] = repulsion(indices_zr, N, banc);
            return(0);
        }
    }
    // Si on est sorti de cette boucle, c'est qu'aucun poisson n'est dans la zone de répulsion. 

}

