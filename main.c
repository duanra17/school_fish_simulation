#include "fonctions.h"
#include <stdlib.h>
#include "math.h"



struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse
}




int main(){

int N; //Nombre de poissons (Indicés de 0 à N-1)
double s; //Norme de la vitesse des poissons
double percep; //Champ de perception
double ra; //Rayon de la zone d'attraction
double rr; //Rayon de la zone de répulsion
double ro; //Rayon de la zone d'orientation
double x_max; //Bornes de la zone disponible
double y_max;

//Création du banc de poissons
struct poisson* banc = malloc(sizeof(struct poisson)*N);
for(int i = 0; i<N){
    struct poisson P;
    initialisation(P,x_max,y_max);
    banc[i] = P;
}




}