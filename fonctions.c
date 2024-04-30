/*Contient les fonctions utiles à la simulation*/

#include <stdlib.h>
#include "math.h"
#include <time.h>

#ifndef  M_PI
    #define  M_PI  3.1415926535897932384626433
#endif

/*Poissons indicés de 0 à N-1*/
struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse (angle entre 0 et 360°)
};

//Initialisation
void initialisation(struct poisson *P, double x_max, double y_max){
    /*Initialise la position et l'orientation du poisson aléatoirement*/
    // On se place dans un rectangle [0;x_max]*[0;y_max]
    P->x = (double)rand()/(double)RAND_MAX * x_max;
    P->y = (double)rand()/(double)RAND_MAX * y_max;
    P->dir = (double)rand()/(double)RAND_MAX * 360;
}

//Distance entre 2 poissons
double distance(struct poisson A, struct poisson B){
    return sqrt( (B.x-A.x)*(B.x-A.x) + (B.y-A.y)*(B.y-A.y) );
}

// Fonction qui ramène les angles entre 0° et 360°
double modulo360(double A){
    if (A>360){
        while(A>360){ 
            A = A-360;
        }
    } else if (A<0){
        while(A<0){ 
            A = A+360;
        }
    }
    return A;
}

//Test de l'angle mort
int dans_angle_mort(struct poisson A, struct poisson B, double alpha){
    // Entrée : poissons A et B et l'angle de vision alpha.
    // Sortie : int indiquant si le poisson B est dans l'angle mort de A (renvoie 1) ou pas (renvoie 0).
    
    // Calcul des angles
    double pseudo_min = modulo360(A.dir - (alpha/2));
    double pseudo_max = modulo360(A.dir + (alpha/2));
    
    double beta = atan((B.y-A.y)/(B.x-A.x)) * 360 / (2*M_PI); // angle de B dans un repère polaire
    if (B.x - A.x < 0){
        // Car arctangente renvoie une valeur entre -pi/2 et pi/2. Il manque donc le cas où beta est entre pi/2 et 3*pi/2
        beta = beta + 180;
    }
    beta = modulo360(beta);

    // Distinction de cas pour la condition sur l'angle beta
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
    // indices_zr: Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZR du poisson i
    // Renvoie la nouvelle direction du poisson, dans le cas de la répulsion
    double tmp = 0;
    int compt = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_zr[j]*(banc[j].dir); // +180 ~ *-1
                compt++;
            }

            tmp = modulo360(tmp/compt + 180); 

            return(tmp);
}

double attraction(int* indices_za, int N, struct poisson* banc){
    // indices_za: Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZA du poisson i
    // Renvoie la nouvelle direction du poisson, dans le cas de l'attraction
    int compt = 0;
    double tmp = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_za[j]*(banc[j].dir);
                compt++;
            }
            tmp = modulo360(tmp/compt);
            while(tmp>360){
                tmp = tmp-360;
            }
            return(tmp);
}

double orientation(int* indices_zo, int N, struct poisson* banc){ 
    // indices_zo: Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZO du poisson i
    // Renvoie la nouvelle direction du poisson, dans le cas de l'orientation

    // Théoriquement, on aurait besoin de la vitesse, mais la vitesse du poisson et sa direction sont colinéaires. 
    // On n'a donc pas besoin de la prendre en compte. 
    // Cette fonction entre dans le cas d'une même vitesse pour tous les poissons, sinon il faut créer une liste des vitesses en amont. 
    int compt = 0;
    double tmp = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_zo[j]*(banc[j].dir);
                compt++;
            }
            tmp = modulo360(tmp/compt + 180); // Signe -
            
            return(tmp);

    
}
// --------------------------------------------------------------------------

int traitement(int* indices_za, int* indices_zr, int* indices_zo, double* dir_temp, int N, struct poisson* banc, int indP){
    // Ne renvoie rien (d'intéressant).

    /* indices_za, indices_zr et indices_zo indiquent la position des poissons par rapport aux zones
    du poisson en question.*/

/*     double d_r = 0;
    double d_o = 0;
    double d_a = 0;

    double tmp = 0; // Variable temporaire */

    // On teste si les poissons sont dans les zones de répulsion, d'attraction et d'orientation
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

    return(0); //Pour retirer un Warning à la compilation
}

double gaussienne(double mu, double sigma){
    // On utilise la méthode de Box-Muller pour simuler une loi gaussienne
    double U1 = ((double)rand())/(double)RAND_MAX;
    double U2 = ((double)rand())/(double)RAND_MAX;

    double X = cos(2*M_PI*U1)*sqrt(-2*log(U2));

    double N;

    N = mu + sigma*X; // N suit une loi normale de moyenne mu et de variance sigma².

    // Pour que la direction soit comprise entre 0 et 360.
    N = modulo360(N);

    return N;
}