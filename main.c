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
    double alpha = 300; // Champ de perception (angle)
    double theta = 0.8; // Vitesse de rotation du poisson (°/ms)
    unsigned int tau = 20; // En ms
    double sigma2 = 1; // Variance de la gaussienne, correspond au tortillage (wiggle)
    double x_max = 900; // Bornes de la zone disponible
    double y_max = 900;

    double rr = 5; // Rayon de la zone de répulsion
    double ro = 30; // Rayon de la zone d'orientation
    double ra = 80; // Rayon de la zone d'attraction
    // On a : rr <= ro <= ra

    // Maximums et minimums des 6 paramètres variables
    /*
    double s_max;
    double s_min;
    double rr_max;
    double rr_min;
    double ro_max;
    double ro_min;
    double ra_max;
    double ra_min;
    double sigma2_max;
    double sigma2_min;
    double alpha_max;
    double alpha_min;
    */

    // Initialisation de l'aléatoire
    srand(time(NULL));

    // Création du banc de poissons
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

    // Banc de poissons
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface * surface = IMG_Load("poissonter.png");
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

    // Barres pour faire varier les paramètres
    SDL_Rect * barres = malloc(sizeof(SDL_Rect)*6);
    SDL_Rect * glisseurs = malloc(sizeof(SDL_Rect)*6);

    // Pour encadrer les poissons
    SDL_Rect aquarium = {0, 0, x_max+30, y_max+30};

    if (barres == NULL){
        printf("Erreur allocation mémoire \n ");
        SDL_Quit();
        exit(1);
    }
    if (glisseurs == NULL){
        printf("Erreur allocation mémoire \n ");
        SDL_Quit();
        exit(1);
    }
    init_barres(barres, x_max, y_max);
    init_glisseurs(glisseurs, x_max, y_max);
    

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

        // Modification des paramètres variables


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
            int zone = 0;
            zone = zones(banc[i], s, tau, x_max, y_max);

            traitement(indices_za,indices_zr,indices_zo,dir_temp,N,banc,i,zone); // On modifie la direction temporaire du i-ème poisson.
        }
        for (int i=0; i<N; ++i){
            // Modification de la direction de chaque poisson
            double nouvelle_dir = modulo360(dir_temp[i] + gaussienne(0,sigma2));
            if (fabs(nouvelle_dir - banc[i].dir) < theta*tau || fabs(nouvelle_dir - banc[i].dir) > 360 - theta*tau){
                banc[i].dir = nouvelle_dir;
            }
            else{
                if(fabs(nouvelle_dir - banc[i].dir)<=180){
                    banc[i].dir = modulo360(banc[i].dir + theta*tau);
                }else{
                    banc[i].dir = modulo360(banc[i].dir - theta*tau);
                }
            }

            // On vérifie que le poisson ne fonce pas dans le mur
            mur(&banc[i], s, tau, x_max, y_max);


            // Modification de la position des poissons
            banc[i].x = banc[i].x + s*tau*cos(2*M_PI * banc[i].dir / 360);
            banc[i].y = banc[i].y + s*tau*sin(2*M_PI * banc[i].dir / 360);
            
        }

        // Render the updated positions
        render(renderer, &texture, banc, N, barres, glisseurs, aquarium);
        
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