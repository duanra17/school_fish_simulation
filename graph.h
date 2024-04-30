#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

struct poisson{
    double x; //Position abscisses
    double y; //Position ordonnées
    double dir; //Direction de la vitesse
};

int affichage(struct poisson* banc, int N);

void render(SDL_Renderer *renderer, SDL_Texture **texture);

void  loadTexture(SDL_Renderer *renderer, SDL_Texture *texture, struct poisson* banc, int N);