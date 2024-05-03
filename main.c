#include <stdlib.h>
#include <math.h>
#include "fonctions.h"
#include <time.h>
#include <stdio.h>

#ifndef  M_PI
    #define  M_PI  3.1415926535897932384626433
#endif

int main(){

    int N = 100; // Nombre de poissons (Indicés de 0 à N-1)
    double s = 0.1; // Norme de la vitesse des poissons (longueur/ms)
    double alpha = 20; // Champ de perception (angle)
    double theta = 30; // Vitesse de rotation du poisson (°/ms)
    unsigned int tau = 10; // En ms
    double x_max = 1000; // Bornes de la zone disponible
    double y_max = 1000;

    double rr = x_max*1/1000; // Rayon de la zone de répulsion
    double ro = x_max*2/1000; // Rayon de la zone d'orientation
    double ra = x_max*3/1000; // Rayon de la zone d'attraction
    // On a : rr <= ro <= ra


    srand(time(NULL));

    //Création du banc de poissons
    struct poisson* banc = malloc(sizeof(struct poisson)*N);

    for(int i = 0; i<N; i++){
        initialisation(&banc[i],x_max,y_max);
    }

    double* dir_temp = malloc(sizeof(double)*N);// Liste des directions des poissons à l'instant suivant
    
    if (dir_temp == NULL) { //Test du malloc
    printf("Erreur d'allocation de mémoire\n");
    return 1;
        }

    int* indices_za = malloc(sizeof(int)*N); // Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZA du poisson i
    int* indices_zr = malloc(sizeof(int)*N); // idem pour ZR
    int* indices_zo = malloc(sizeof(int)*N); // idem pour ZO

    // Initialisation de l'affichage 
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "SDL initialization, failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("N-Body Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x_max, y_max, SDL_WINDOW_SHOWN);
    if(window == NULL){
        fprintf(stderr, "Window creation failed : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface * surface = IMG_Load("poisson.png");
    if (surface == NULL){
        fprintf(stderr, "Failed to load image: %s \n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL){
        fprintf(stderr, "Failed to create texture: %s \n ", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // Boucle temporelle
    SDL_Event event;
    int quit = 0;
    while (!quit){
        // Fermeture de la fenêtre
        while (SDL_PollEvent(&event) != 0){
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
        // Remplissage de dir_temp, permet de garder les mêmes valeurs des poissons à l'instant t.
        for (int i=0; i<N; ++i){
            dir_temp[i] = banc[i].dir;
        }
        
        // Boucle sur tous les poissons i
        for (int i=0; i<N; ++i){
            
            for (int j=0; j<N; ++j){
                indices_za[j] = 0;
                indices_zr[j] = 0;
                indices_zo[j] = 0;
            }
            // Boucle sur congénères j du poisson i
            for (int j=0; j<N; ++j){
                if (j==i){
                    ++j;
                }else{
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
            double nouvelle_dir = dir_temp[i] + gaussienne(0,5);
            if (nouvelle_dir - banc[i].dir < theta*tau){
                banc[i].dir = nouvelle_dir;
            }
            else{
                banc[i].dir = theta*tau;
            }
            
            // Modification de la position des poissons
            banc[i].x = banc[i].x + s*tau*cos(2*M_PI * banc[i].dir / 360);
            banc[i].y = banc[i].y + s*tau*sin(2*M_PI * banc[i].dir / 360);
            
        }
        // Render the updated positions
        render(renderer, &texture, banc, N);
        
        //Delay to control the frame rate
        SDL_Delay(tau); // en ms
    }
    // Fin de la boucle temporelle
    free(dir_temp);
    free(banc);
    free(indices_za);
    free(indices_zo);
    free(indices_zr);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}