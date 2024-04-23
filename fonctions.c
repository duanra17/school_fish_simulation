/*Contient les fonctions du projet*/
/*
Attention!
Poissons indicés de 0 à N-1

*/
#include <stdlib.h>
#include "math.h"
#include <time.h>

#ifndef  M_PI
    #define  M_PI  3.1415926535897932384626433
#endif

struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse
};

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
int dans_angle_mort(struct poisson A, struct poisson B, double alpha){
    // On calcule les angles
    float pseudo_min = (A.dir - (alpha/2))%360;
    float pseudo_max = (A.dir + (alpha/2))%360;
    int dans_angle_mort;
    float beta = atan((B.y-A.y)/(B.x-A.x)) * 360 / (2*M_PI);
    if (B.x - A.x < 0){
        // Car arctangente renvoie une valeur entre -pi/2 et pi/2. Il manque donc le cas où beta est entre pi/2 et 3*pi/2
        beta = beta + 180;
    }
    beta = beta%360;
    // On distingue 3 cas ( cas 1 et 3 confondus): 
    // 0 < dir < alpha/2
    // alpha/2 < dir < 360° - alpha/2
    // 360° - alpha/2 < dir < 360°
    // Cela influe sur quel est le plus grand entre pseudo_max et pseudo_min 
    // (pseudo_max est toujours le plus grand si on n'effectue pas le modulo 360)

        if((A.dir < (alpha/2))||(A.dir > 360 - (alpha/2))){ 
            if ((pseudo_max < beta) && (beta < pseudo_min)){
                return 1;
            }
            else {return 0;}
        }
        else{// Cas alpha/2 < A.dir < 360° - alpha/2
            if ((pseudo_min < beta) && (beta < pseudo_max)){
                return 0;
            }
            else {return 1;}
        }
}

// --------------------------------------------------------------------------
// Fonctions nécessaires au traitement

double repulsion(int* indices_zr, int N, struct poisson* banc){
    double tmp;
    int compt = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_zr[j]*(banc[j].dir); // +180 ~ *-1
                compt++;
            }
            tmp = tmp/compt + 180; 
            while(tmp>360){ // Pour que la direction soit comprise entre 0 et 360.
                tmp = tmp-360;
            }
            return(tmp);
}

double attraction(int* indices_za, int N, struct poisson* banc){
    int compt = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_za[j]*(banc[j].dir);
                compt++;
            }
            tmp = tmp/compt;
            while(tmp>360){
                tmp = tmp-360;
            }
            return(tmp);
}

double orientation(int* indices_zo, int N, struct poisson* banc){ 
// théoriquement, on aurait besoin de la vitesse, mais la vitesse du poisson et sa direction sont colinéaires. 
// On n'a donc pas besoin de la prendre en compte. 
// Il faut modifier cette fonction dans le cas où les poissons ont des vitesses différentes, avec une liste des vitesses à créer en amont. 
    int compt = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_zo[j]*(banc[j].dir);
                compt++;
            }
            tmp = tmp/compt + 180; // Signe -
            while(tmp>360){ // Pour que la direction soit comprise entre 0 et 360.
                tmp = tmp-360;
            }
            return(tmp);

    
}
// --------------------------------------------------------------------------

int traitement(int* indices_za, int* indices_zr, int* indices_zo, double* dir_temp, int N, struct poisson* banc, int indP){
    /* indices_za, indices_zr et indices_zo indiquent la position des poissons par rapport aux zones
    du poisson en question.*/
    double d_r = 0;
    double d_o = 0;
    double d_a = 0;

    double tmp = 0; // Variable temporaire

    // Pour éviter de créer des boucles trop similaires, je crée trois autres fonctions : repulsion(), attraction() et orientation().

    // On teste si les poissons sont dans les zonesde répulsion, d'attraction et d'orientation
    // Les variables de test sont initialisées à 0
    int test_a = 0;
    int test_o = 0;

    for(int i=0; i<N; i++){
        if(indices_zr[i]){// Si au moins un poisson est dans la zone de répulsion, on ne fait pas les autres boucles.
            dir_temp[indP] = repulsion(indices_zr, N, banc);
            return(0);
        }
        if(indices_za[i]){ // Si au moins un poisson est dans la zone d'attraction
            test_a = 1;
        }
        if(indices_zo[i]){ // Si au moins un poisson est dans la zone d'orientation
            test_o = 1;
        }
    }
    // Si on est sorti de cette boucle, c'est qu'aucun poisson n'est dans la zone de répulsion. 
    // On applique le traitement en fonction des résultats des tests précédents.
    if(test_a && !test_o){
        dir_temp[indP] = attraction(indices_za, N, banc);
        return(0);
    }
    if(test_o && !test_a){
        dir_temp[indP] = orientation(indices_zo, N, banc);
        return(0);
    }
    if(test_a && test_o){
        dir_temp[indP] = (attraction(indices_za, N, banc) + orientation(indices_zo, N, banc))/2;
        return(0);
    }
}

float gaussienne(double mu, double sigma){
    // On utilise la méthode de Box-Muller pour simuler une loi gaussienne
    float U1 = ((float)rand()-(float)RAND_MAX/2)/(float)RAND_MAX;
    float U2 = ((float)rand()-(float)RAND_MAX/2)/(float)RAND_MAX;

    float X = cos(2*M_PI*U1)*sqrt(-2*log(U2));

    float N;

    N = mu + sigma*X; // N suit une loi normale de moyenne mu et de variance sigma².

    // Pour que la direction soit comprise entre 0 et 360.
    if (N>360){
        while(N>360){ 
            N = N-360;
        }
    } else if (N<0){
        while(N>360){ 
            N = N+360;
        }
    }

    return N;
}
