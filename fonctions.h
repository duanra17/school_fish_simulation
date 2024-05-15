
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse
};

void initialisation(struct poisson* P, double x_max, double y_max);

double distance(struct poisson A, struct poisson B);

double modulo360(double A);

int dans_angle_mort(struct poisson A, struct poisson B, double alpha);

double arg_dist_poissons(struct poisson P1, struct poisson P2);

double repulsion(int* indices_zr, int N, struct poisson* banc, int zone);

double attraction(int* indices_za, int N, struct poisson* banc);

double orientation(int* indices_zo, int N, struct poisson* banc);

int traitement(int* indices_za, int* indices_zr, int* indices_zo, double* dir_temp, int N, struct poisson* banc, int indP, int zone);

double gaussienne(double mu, double sigma);

void mur(struct poisson* P, double s, double tau, double x_max, double y_max);

int zones(struct poisson P, double s,double tau, double x_max, double y_max);

void render(SDL_Renderer *renderer, SDL_Texture **texture, struct poisson* banc, int N, SDL_Rect * barres, SDL_Rect *glisseurs);

void init_barres(SDL_Rect *barres, double x_max, double y_max);

void init_glisseurs(SDL_Rect *glisseurs, SDL_Rect *barres);