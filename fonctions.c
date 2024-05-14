/*Contient les fonctions utiles à la simulation*/

#include <stdlib.h>
#include "math.h"
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "fonctions.h"

#ifndef  M_PI
    #define  M_PI  3.1415926535897932384626433
#endif


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

double arg_dist_poissons(struct poisson P1, struct poisson P2){
    // Renvoie l'argument du vecteur qui relie le poisson P1 au poisson P2.

    double theta = (360/(2*M_PI))*acos( (P2.x-P1.x) / sqrt((P2.x-P1.x)*(P2.x-P1.x) + (P2.y-P1.y)*(P2.y-P1.y)));

    if(P2.y>=P1.y){
        return theta;
    }else{
        return modulo360(-theta);
    }
}

// --------------------------------------------------------------------------
// Fonctions nécessaires au traitement



double repulsion(int* indices_zr, int N, struct poisson* banc, int zone){
    // indices_zr: Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZR du poisson i
    // Renvoie la nouvelle direction du poisson, dans le cas de la répulsion
    double tmp = 0;
    int compt = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_zr[j]*(arg_dist_poissons(banc[N], banc[j])); 
                compt++;
            }

            if(zone == 1){
                tmp = tmp + 10*90;
                compt = compt + 10;
            }
            if(zone == 2){
                tmp = tmp + 10*270;
                compt = compt + 10;
            }
            if(zone == 3){
                tmp = tmp + 10*180;
                compt = compt + 10;
            }
            if(zone == 4){
                compt = compt + 10;
            }

            tmp = modulo360(tmp/compt + 180); // +180 ~ *-1

            return(tmp);
}

double attraction(int* indices_za, int N, struct poisson* banc){
    // indices_za: Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZA du poisson i
    // Renvoie la nouvelle direction du poisson, dans le cas de l'attraction
    int compt = 0;
    double tmp = 0;
            for(int j=0; j<N; j++){
                tmp = tmp + indices_za[j]*(arg_dist_poissons(banc[N], banc[j]));
                compt++;
            }
            tmp = modulo360(tmp/compt);
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
            tmp = modulo360(tmp/compt);
            
            return(tmp);

    
}
// --------------------------------------------------------------------------

int traitement(int* indices_za, int* indices_zr, int* indices_zo, double* dir_temp, int N, struct poisson* banc, int indP, int zone){
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
        if(indices_zr[i] || zone!=0){// Si au moins un poisson est dans la zone de répulsion, on ne fait pas les autres boucles.
            dir_temp[indP] = repulsion(indices_zr, N, banc, zone);
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

// void mur(struct poisson* P, double s,double tau, double x_max, double y_max){
//     // La fonction détermine si un poisson est proche d'un mur (bord de la zone dessinée à l'écran) et le réoriente.
//     double d_bord = s*tau; //Distance pour laquelle, au pas suivant, le poisson risque de sortir.

//     if(P->y<d_bord){
//         if(P->dir<180){
//             P->dir = modulo360(-(P->dir));
//         }
//     }
//     if(P->y>y_max-d_bord){
//         if(P->dir>180){
//             P->dir = modulo360(-(P->dir));
//         }
//     }

//     if(P->x<d_bord){
//         if(P->dir>90 && P->dir<270){
//             P->dir = modulo360(180-(P->dir));
//         }
//     }
//     if(P->x>x_max-d_bord){
//         if(P->dir<90 || P->dir>270){
//             P->dir = modulo360(180-(P.dir));
//         }
//     }
// }

int mur(struct poisson P, double s,double tau, double x_max, double y_max){
    // La fonction détermine si un poisson est proche d'un mur (bord de la zone dessinée à l'écran) et le réoriente.
    double d_bord = 20*s*tau; //Distance pour laquelle, au pas suivant, le poisson risque de sortir.

    if(P.y<d_bord){
        return 1;
    }else if(P.y>y_max-d_bord){
        return 2;
    }else if(P.x<d_bord){
        return 3;
    }else if(P.x>x_max-d_bord){
        return 4;
    }else{
        return 0;
    }
    return 0;
}


// --------------------------------------------------------------------------
// Fonctions d'affichage graphique


// Fenêtre
void render(SDL_Renderer *renderer, SDL_Texture **texture, struct poisson* banc, int N, SDL_Rect *barres, SDL_Rect *glisseurs){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    // Poissons
    SDL_Rect * rects = malloc(sizeof(SDL_Rect)*N);
    for (int i=0; i<N; ++i){
        rects[i].x = banc[i].x; 
        rects[i].y = banc[i].y;
        rects[i].w = 30;
        rects[i].h = 30;
        SDL_RenderCopyEx (renderer,*texture, NULL, &rects[i], banc[i].dir, NULL, SDL_FLIP_NONE);
    }  

    // Barres
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    for (int i=0; i<6; ++i){
        SDL_RenderFillRect(renderer,&barres[i]);     
    }

    // Glisseurs
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i=0; i<6; ++i){
        SDL_RenderFillRect(renderer,&glisseurs[i]);
    }

    // Affichage
    SDL_RenderPresent(renderer);
}

void init_barres(SDL_Rect *barres, double x_max, double y_max){
    // Definition de la position des barres
    for (int i = 0; i<6; ++i){
        barres[i].h = 20;
        barres[i].x = x_max + 100; 
        barres[i].y = (i+1)*y_max/7 - barres[i].h/2;
        barres[i].w = 200;
    }
}

void init_glisseurs(SDL_Rect *glisseurs, double x_max, double y_max){
    // Definition de la position des barres
    for (int i = 0; i<6; ++i){
        glisseurs[i].h = 20;
        glisseurs[i].x = x_max + 100; // Variable
        glisseurs[i].y = (i+1)*y_max/7 - glisseurs[i].h/2;
        glisseurs[i].w = 30;
    }
}

