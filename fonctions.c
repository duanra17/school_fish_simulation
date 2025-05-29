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
    P->x   = (double)rand()/(double)RAND_MAX * x_max;
    P->y   = (double)rand()/(double)RAND_MAX * y_max;
    P->dir = (double)rand()/(double)RAND_MAX * 360;
}

//Distance entre 2 poissons
double distance(struct poisson A, struct poisson B){
    return sqrt( (B.x-A.x)*(B.x-A.x) + (B.y-A.y)*(B.y-A.y) );
}

// Fonction qui ramène les angles entre 0° et 360°
double modulo360(double A){
    if (A>=360){
        while(A>=360){ 
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


// Renvoie l'argument du vecteur qui relie le poisson P1 au poisson P2.
double arg_dist_poissons(struct poisson P1, struct poisson P2){
    // Renvoie l'argument du vecteur qui relie le poisson P1 au poisson P2.

    double theta = (360/(2*M_PI))*acos( (P2.x-P1.x) / distance(P1,P2));

    if(P2.y>=P1.y){
        return theta;
    }else{
        return modulo360(-theta);
    }
}

// --------------------------------------------------------------------------
// Fonctions nécessaires au traitement


// Renvoie la nouvelle direction du poisson, dans le cas de la répulsion
double repulsion(int* indices_zr, int N, struct poisson* banc, int zone, int indP){
    // indices_zr: Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZR du poisson i
    // Renvoie la nouvelle direction du poisson, dans le cas de la répulsion
    double COS = 0;
    double SIN = 0;
    double tmp = 0;
            for(int j=0; j<N; j++){
                if( j != indP){
                    COS = COS + indices_zr[j]*( cos( (2*M_PI/360) * arg_dist_poissons(banc[indP], banc[j])) );
                    SIN = SIN + indices_zr[j]*( sin( (2*M_PI/360) * arg_dist_poissons(banc[indP], banc[j])) );   
                }
            }
                
            if(zone == 1){
                SIN = SIN-10;
            }
            if(zone == 2){
                SIN = SIN+10;
            }
            if(zone == 3){
                COS = COS-10;
            }
            if(zone == 4){
                COS = COS+10;
            }
            if(zone == 5){
                COS = COS + 10*cos(-M_PI/4);
                SIN = SIN + 10*sin(-M_PI/4);
            }
            if(zone == 6){
                COS = COS + 10*cos(M_PI/4);
                SIN = SIN + 10*sin(M_PI/4);
            }
            if(zone == 7){
                COS = COS + 10*cos(3*M_PI/4);
                SIN = SIN + 10*sin(3*M_PI/4);
            }
            if(zone == 8){
                COS = COS + 10*cos(-3*M_PI/4);
                SIN = SIN + 10*sin(-3*M_PI/4);
            }

            tmp = (360/(2*M_PI)) * atan2(SIN,COS);

            tmp = modulo360(tmp + 180); // +180 ~ *-1 en vecteur
            
            return(tmp);
}

// Renvoie la nouvelle direction du poisson, dans le cas de l'attraction
double attraction(int* indices_za, int N, struct poisson* banc, int indP){
    // indices_za: Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZA du poisson i
    // Renvoie la nouvelle direction du poisson, dans le cas de l'attraction
    double COS = 0;
    double SIN = 0;
    double tmp = 0;
            for(int j=0; j<N; j++){
                if( j != indP){
                    COS = COS + indices_za[j]*( cos( (2*M_PI/360) * arg_dist_poissons(banc[indP], banc[j])) );
                    SIN = SIN + indices_za[j]*( sin( (2*M_PI/360) * arg_dist_poissons(banc[indP], banc[j])) );
                }
            }
            tmp = (360/(2*M_PI)) * atan2(SIN,COS);
            
            tmp = modulo360(tmp);
            
            return(tmp);
}


// Renvoie la nouvelle direction du poisson, dans le cas de l'orientation
double orientation(int* indices_zo, int N, struct poisson* banc){ 
    // indices_zo: Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZO du poisson i
    // Renvoie la nouvelle direction du poisson, dans le cas de l'orientation

    // Théoriquement, on aurait besoin de la vitesse, mais la vitesse du poisson et sa direction sont colinéaires. 
    // On n'a donc pas besoin de la prendre en compte. 
    // Cette fonction entre dans le cas d'une même vitesse pour tous les poissons, sinon il faut créer une liste des vitesses en amont. 
    double COS = 0;
    double SIN = 0;
    double tmp = 0;
            for(int j=0; j<N; j++){
                COS = COS + indices_zo[j]*( cos( (2*M_PI/360) * banc[j].dir) );
                SIN = SIN + indices_zo[j]*( sin( (2*M_PI/360) * banc[j].dir) );   
            }
            tmp = (360/(2*M_PI)) * atan2(SIN,COS);

            tmp = modulo360(tmp);
            
            return(tmp);

    
}
// --------------------------------------------------------------------------

// Détermine la direction privilégiée du poisson
int traitement(int* indices_za, int* indices_zr, int* indices_zo, double* dir_temp, int N, struct poisson* banc, int indP, int zone){
    // On teste si les poissons sont dans les zones de répulsion, d'attraction et d'orientation
    // Les variables de test sont initialisées à 0
    int test_a = 0;
    int test_o = 0;

    for(int i=0; i<N; i++){
        if(indices_zr[i] || zone!=0){// Si au moins un poisson est dans la zone de répulsion, on ne fait pas les autres boucles.
            dir_temp[indP] = repulsion(indices_zr, N, banc, zone, indP);
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
        dir_temp[indP] = attraction(indices_za, N, banc, indP);
        return(0);
    }

    if(test_o && !test_a){
        dir_temp[indP] = orientation(indices_zo, N, banc);
        return(0);
    }


    if(test_a && test_o){
        dir_temp[indP] = (attraction(indices_za, N, banc, indP) + orientation(indices_zo, N, banc))/2;
        return(0);
    }

    return(0); //Pour retirer un Warning à la compilation
}

// Renvoie une valeur aléatoire générée à l'aide de la méthode de Box-Muller
double gaussienne(double mu, double sigma){
    // On utilise la méthode de Box-Muller pour simuler une loi gaussienne
    double U1 = ((double)rand())/(double)RAND_MAX;
    double U2 = ((double)rand())/(double)RAND_MAX;

    double X = cos(2*M_PI*U1)*sqrt(-2*log(U2));

    double N;

    N = mu + sigma*X; // N suit une loi normale de moyenne mu et de variance sigma².

    return N;
}


// La fonction détermine si un poisson est proche d'un mur (bord de la zone dessinée à l'écran) et le réoriente.
void mur(struct poisson* P, double s, double tau, double x_max, double y_max){
    // La fonction détermine si un poisson est proche d'un mur (bord de la zone dessinée à l'écran) et le réoriente.
    double d_bord = s*tau; //Distance pour laquelle, au pas suivant, le poisson risque de sortir.

    if(P->y<d_bord){
        if(P->dir>180){ // Les angles sont comptés dans le sens horaire
            P->dir = modulo360(-(P->dir));
        }
    }
    if(P->y>y_max-d_bord){ 
        if(P->dir<180){ // Les angles sont comptés dans le sens horaire
            P->dir = modulo360(-(P->dir));
        }
    }

    if(P->x<d_bord){
        if(P->dir>90 && P->dir<270){
            P->dir = modulo360(180-(P->dir));
        }
    }
    if(P->x>x_max-d_bord){
        if(P->dir<90 || P->dir>270){
            P->dir = modulo360(180-(P->dir));
        }
    }
}


// La fonction détermine si un poisson est proche d'un mur (bord de la zone dessinée à l'écran) et le réoriente.
int zones(struct poisson P, double s,double tau, double x_max, double y_max){
    // La fonction détermine si un poisson est proche d'un mur (bord de la zone dessinée à l'écran) et le réoriente.
    double d_bord = 5*s*tau; //Distance pour laquelle le poisson détecte le mur
    int zone = 0;
    if(P.y<d_bord){
        zone =  1;
    }if(P.y>y_max-d_bord){
        zone =  2;
    }else if(P.x<d_bord){
        zone =  3;
    }else if(P.x>x_max-d_bord){
        zone =  4;
    }else if(P.y<d_bord && P.x>x_max-d_bord){
        zone =  5;
    }else if(P.y>y_max-d_bord && P.x>x_max-d_bord){
        zone =  6;
    }else if(P.x<d_bord && P.y>y_max-d_bord){
        zone =  7;
    }else if(P.x<d_bord && P.y<d_bord){
        zone =  8;
    }

    //zone = 0; //Pour retirer cette fonctionalité

    return zone;
}


// --------------------------------------------------------------------------
// Fonctions d'affichage graphique


// Fenêtre
void render(SDL_Renderer *renderer, SDL_Texture **texture, struct poisson* banc, int N, SDL_Rect *barres, SDL_Rect *glisseurs, SDL_Rect aquarium, 
SDL_Rect *noms, SDL_Rect *signes, SDL_Texture ** textures_nom, SDL_Rect * rects){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    // Poissons
    for (int i=0; i<N; ++i){
        rects[i].x = banc[i].x; 
        rects[i].y = banc[i].y;
        rects[i].w = 30;
        rects[i].h = 30;
        SDL_RenderCopyEx (renderer,*texture, NULL, &rects[i], banc[i].dir, NULL, SDL_FLIP_NONE);
    }

    // Noms et signes
    for (int i=0; i<7; ++i){
        // Noms
        SDL_RenderCopy(renderer,textures_nom[i], NULL, &noms[i]);
        // Moins
        SDL_RenderCopy(renderer,textures_nom[7], NULL, &signes[2*i]);
        // Plus
        SDL_RenderCopy(renderer,textures_nom[8], NULL, &signes[2*i+1]);
    }

    // Barres
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    for (int i=0; i<7; ++i){
        SDL_RenderFillRect(renderer,&barres[i]);     
    }

    // Glisseurs
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i=0; i<7; ++i){
        SDL_RenderFillRect(renderer,&glisseurs[i]);
    }

    // Aquarium
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &aquarium);

    // Affichage
    SDL_RenderPresent(renderer);
}

// Definition de la position des barres
void init_barres(SDL_Rect *barres, double x_max, double y_max, SDL_Rect *noms, SDL_Rect *signes){
    for (int i = 0; i<7; ++i){
        // Definition des positions et taile des barres
        barres[i].h = 20;
        barres[i].x = x_max + 100; 
        barres[i].y = (i+1)*y_max/8 - barres[i].h/2;
        barres[i].w = 200;
        // Définition de la position et taille des noms des barres
        noms[i].h = 40;
        noms[i].x = barres[i].x; 
        noms[i].y = barres[i].y - noms[i].h ;
        // Définition de la position et taille des signes
            // Pour les moins
        signes[2*i].h = 40;
        signes[2*i].w = 40;
        signes[2*i].x = barres[i].x - 30 - signes[2*i].w/2; 
        signes[2*i].y = barres[i].y - signes[2*i].h/4;
            //Pour les plus
        signes[2*i+1].h = 40;
        signes[2*i+1].w = 40;
        signes[2*i+1].x = barres[i].x + barres[i].w + 5 + signes[2*i+1].w/2; 
        signes[2*i+1].y = barres[i].y - signes[2*i+1].h/4;
    }
    noms[0].w = 90;
    noms[1].w = 190;
    noms[2].w = 210;
    noms[3].w = 200;
    noms[4].w = 130;
    noms[5].w = 210;
    noms[6].w = 200;

}

// Definition de la position des glisseurs
void init_glisseurs(SDL_Rect *glisseurs, SDL_Rect *barres){
    // Definition de la position des glisseurs
    for (int i = 0; i<7; ++i){
        glisseurs[i].h = barres[i].h;
        glisseurs[i].w = 30;
        glisseurs[i].x = barres[i].x + barres[i].w - glisseurs[i].w/2; // Variable
        glisseurs[i].y = barres[i].y;
    }
}

