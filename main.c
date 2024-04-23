#include "fonctions.h"
#include <stdlib.h>
#include "math.h"


int main(){

int N; //Nombre de poissons (Indicés de 0 à N-1)
double s; //Norme de la vitesse des poissons
double alpha; //Champ de perception (angle)
double ra; //Rayon de la zone d'attraction
double rr; //Rayon de la zone de répulsion
double ro; //Rayon de la zone d'orientation
// On a : rr <= ro <= ra
double x_max; //Bornes de la zone disponible
double y_max;
int Tmax; //Temps maximal de la simulation

//Création du banc de poissons
struct poisson* banc = malloc(sizeof(struct poisson)*N);
for(int i = 0; i<N; ++i){
    struct poisson P;
    initialisation(P,x_max,y_max);
    banc[i] = P;
}

//Boucle temporelle
int t=0;
while (t<Tmax){
    // TODO : Si nécessaire, attendre un peu
    double* dir_temp[N]; // Liste des directions des poissons à pour l'instant suivant
    //Boucle sur tous les poissons i
    for (int i=0; i<N; ++i){
        int indices_za[N]; //Liste des poissons dans la ZA du poisson i
        int indices_zr[N]; //idem pour ZR
        int indices_zo[N]; //idem pour ZO
        for (int j=0; j<N; ++j){
            indices_za[j] = 0;
            indices_zr[j] = 0;
            indices_zo[j] = 0;
        }
        //Boucle sur congénères j du poisson i
        for (int j=0; j<N; ++j){
            if (j==i){
                ++j;
            }
            if (j<N){
                if (!dans_angle_mort(banc[i],banc[j],alpha)){
                    // Le poisson j est visible par le poisson i
                    if (distance(banc[i],banc[j])<rr){
                        // Le poisson j est dans la ZR du poisson i
                        indices_zr[j] = 1;
                    }
                    else if (distance(banc[i],banc[j])<ro){
                        // Le poisson j est dans la ZO du poisson i
                        indices_zo[j] = 1;
                    }
                    else if (distance(banc[i],banc[j])<ra){
                        // Le poisson j est dans la ZA du poisson i
                        indices_za[j] = 1;
                    //Autre cas : le poisson j n'est dans aucune zone voisine du poisson i
                    }
                }
            }
        }
        traitement(indices_za,indices_zr,indices_zo,dir_temp,N,banc,i); // On modifie la direction temporaire du i-ème poisson.
        
    }

    
    for (int i=0; i<N; ++i){
        // Modification de la direction de chaque poisson
        banc[i].dir = dir_temp[i] + gaussienne(0,10);
        // Modification de la position des poissons
        banc[i].x = banc[i].x + s*cos(2*M_PI * banc[i].dir / 360);
        banc[i].y = banc[i].y + s*sin(2*M_PI * banc[i].dir / 360);
    }

    t++;
}



}