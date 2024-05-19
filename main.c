#include <stdlib.h>
#include <math.h>
#include "fonctions.h"
#include <time.h>
#include <stdio.h>

#ifndef  M_PI
    #define  M_PI  3.1415926535897932384626433
#endif




int main(){

    // Initialisation des paramètres fixes
    int N = 80; // Nombre de poissons (Indicés de 0 à N-1)
    double tau = 0.1; // En s
    double x_max = 1000; // Bornes de la zone disponible
    double y_max = 800;

    // Paramètres variables
    /*
    para[0]: s // Norme de la vitesse des poissons (longueur/s)
    para[1]: rr // Rayon de la zone de répulsion
    para[2]: ro // Rayon de la zone d'orientation
    para[3]: ra // Rayon de la zone d'attraction
        Il faudrait : rr <= ro <= ra
    para[4]: sigma2 // Variance de la gaussienne, correspond au tortillage (wiggle)
    para[5]: alpha // Champ de perception (angle)
    para[6]: theta // Vitesse de rotation du poisson (°/s)
    */

    double  para[7]; // Liste des paramètres variables
    //      para[7]    = {  s,   rr,   ro,   ra, sigma2, alpha, theta }
    double para_max[7] = {100,   10,  150,  150,    10,   359,   100 }; // Maximums des paramètres
    double para_min[7] = { 10,    0,    0,    0,      0,   200,    10 };  // Minimums des paramètres

    for (int i=0; i<7; ++i){
        para[i] = para_max[i];
    }

    // Initialisation de l'aléatoire
    srand(time(NULL));

    // Création du banc de poissons
    struct poisson* banc = malloc(sizeof(struct poisson)*N);

    for(int i = 0; i<N; i++){
        initialisation(&banc[i],x_max,y_max);
    }
    double* dir_temp = malloc(sizeof(double)*N);// Liste des directions des poissons à l'instant suivant
    if (dir_temp == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        return 1;
    }

    int* indices_za = malloc(sizeof(int)*N); // Liste de 1 ou 0 indiquant si le poisson du même indice est dans la ZA du poisson i
    int* indices_zr = malloc(sizeof(int)*N); // idem pour ZR
    int* indices_zo = malloc(sizeof(int)*N); // idem pour ZO


//--------------------------------------------------------------------------------------------------------------------------------


    // Initialisation de l'affichage 
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "SDL initialization, failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("N-Body Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x_max+320, y_max, SDL_WINDOW_SHOWN);
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

    // Noms des barres
    SDL_Surface ** surfaces_nom = malloc(sizeof(SDL_Surface*)*9);
    SDL_Texture ** textures_nom = malloc(sizeof(SDL_Texture*)*9); 
    // Liste des textures des noms des barres (indices 0-7), le moins (8) et le plus (9).
    if (surfaces_nom == NULL || textures_nom == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        return 1;
    }    
    surfaces_nom[0] = IMG_Load("paramètre0.bmp");
    surfaces_nom[1] = IMG_Load("paramètre1.bmp");    
    surfaces_nom[2] = IMG_Load("paramètre2.bmp");    
    surfaces_nom[3] = IMG_Load("paramètre3.bmp");    
    surfaces_nom[4] = IMG_Load("paramètre4.bmp");    
    surfaces_nom[5] = IMG_Load("paramètre5.bmp");    
    surfaces_nom[6] = IMG_Load("paramètre6.bmp");    
    surfaces_nom[7] = IMG_Load("Moins.bmp");
    surfaces_nom[8] = IMG_Load("Plus.bmp"); 

    for (int i=0; i<9; ++i){
        if (surfaces_nom[i] == NULL){
            fprintf(stderr, "Failed to load image: %s \n", IMG_GetError());
            SDL_Quit();
            exit(1);
        }   

        textures_nom[i] = SDL_CreateTextureFromSurface(renderer, surfaces_nom[i]);
        SDL_FreeSurface(surfaces_nom[i]);
        if (textures_nom[i] == NULL){
            fprintf(stderr, "Failed to create texture: %s \n ", SDL_GetError());
            SDL_Quit();
            exit(1);
        }
    }

    free(surfaces_nom);

    SDL_Rect * noms = malloc(sizeof(SDL_Rect)*7);
    SDL_Rect * signes = malloc(sizeof(SDL_Rect)*2*7); // Indices pairs --> moins. Indices impairs --> plus.
    if (noms == NULL || signes == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        return 1;
    }

    // Barres et glisseurs pour faire varier les paramètres
    SDL_Rect * barres = malloc(sizeof(SDL_Rect)*7);
    SDL_Rect * glisseurs = malloc(sizeof(SDL_Rect)*7);
    
    // Pour encadrer les poissons
    SDL_Rect aquarium = {0, 0, x_max+30, y_max+30};

    if (barres == NULL || glisseurs == NULL){
        printf("Erreur allocation mémoire \n ");
        SDL_Quit();
        exit(1);
    }

    init_barres(barres, x_max, y_max, noms, signes);
    init_glisseurs(glisseurs, barres);


//-------------------------------------------------------------------------------------------------------------------------------- 

    // Boucle temporelle
    SDL_Event event;
    int quit = 0;

    while (!quit){
        
        while (SDL_PollEvent(&event) != 0){
            // Fermeture de la fenêtre
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        
            // Modification des paramètres variables
            if (event.button.button == SDL_BUTTON_LEFT){

                for (int i=0; i<7; ++i){
                    float xs = event.button.x;// Coordonnées de la souris
                    float ys = event.button.y;// Coordonnées de la souris

                    // Si la souris se trouve dans la barre
                    if (barres[i].y<ys && barres[i].y+barres[i].h>ys && barres[i].x<xs && barres[i].x+barres[i].w>xs){
                        float pente = (para_max[i]-para_min[i])/(barres[i].w); // Pente de la relation linéaire entre abscisse et paramètre
                        para[i] = pente*(xs-barres[i].x) + para_min[i];
                        glisseurs[i].x = xs - glisseurs[i].w/2;
                    }
                }
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
                    if (!dans_angle_mort(banc[i],banc[j],para[5])){
                        // Le poisson j est visible par le poisson i
                        if (distance(banc[i],banc[j])<para[1]){
                            // Le poisson j est dans la ZR du poisson i
                            indices_zr[j] = 1;
                        }
                        else if (distance(banc[i],banc[j])<para[2]){
                            // Le poisson j est dans la ZO du poisson i
                            indices_zo[j] = 1;
                        }
                        else if (distance(banc[i],banc[j])<para[3]){
                            // Le poisson j est dans la ZA du poisson i
                            indices_za[j] = 1;
                        //Autre cas : le poisson j n'est dans aucune zone voisine du poisson i
                        }
                    }
                }
            }
            int zone = 0;
            zone = zones(banc[i], para[0], tau, x_max, y_max);

            traitement(indices_za,indices_zr,indices_zo,dir_temp,N,banc,i,zone); // On modifie la direction temporaire du i-ème poisson.
        }
        // Modification de la direction de chaque poisson
        for (int i=0; i<N; ++i){
            double nouvelle_dir = modulo360(dir_temp[i] + gaussienne(0,para[4]));
            if (fabs(nouvelle_dir - banc[i].dir) <= para[6]*tau || fabs(nouvelle_dir - banc[i].dir) >= 360 - para[6]*tau){
                // On est dans l'intervalle des rotations possibles pour le poisson
                // para[6] = theta
                banc[i].dir = nouvelle_dir;
            }
            else{
                // La rotation exigée est trop grande pour le poisson.
                // On fait tourner le poisson de l'angle de rotation maximal (theta*tau)

                if(nouvelle_dir - banc[i].dir>0 && nouvelle_dir - banc[i].dir<=180){
                    banc[i].dir = modulo360(banc[i].dir + para[6]*tau);
                
                }else if(nouvelle_dir - banc[i].dir>0 && nouvelle_dir - banc[i].dir>180){
                    banc[i].dir = modulo360(banc[i].dir - para[6]*tau);
                    
                }else if(nouvelle_dir - banc[i].dir<0 && fabs(nouvelle_dir - banc[i].dir)<=180){
                    banc[i].dir = modulo360(banc[i].dir - para[6]*tau);
                    
                }else if(nouvelle_dir - banc[i].dir<0 && fabs(nouvelle_dir - banc[i].dir)>180){
                    banc[i].dir = modulo360(banc[i].dir + para[6]*tau);
                    
                }

            }

            // On vérifie que le poisson ne fonce pas dans le mur
            mur(&banc[i], para[0], tau, x_max, y_max);


            // Modification de la position des poissons
            banc[i].x = banc[i].x + para[0]*tau*cos(2*M_PI * banc[i].dir / 360);
            banc[i].y = banc[i].y + para[0]*tau*sin(2*M_PI * banc[i].dir / 360);
            
        }

        // Render the updated positions
        render(renderer, &texture, banc, N, barres, glisseurs, aquarium, noms, signes, textures_nom);
        
        //Delay to control the frame rate
        SDL_Delay(10); // en ms
    }
    
    // Fin de la boucle temporelle
    free(banc);
    free(dir_temp);
    free(indices_za);
    free(indices_zo);
    free(indices_zr);
    free(textures_nom);
    free(noms);
    free(signes);
    free(barres);
    free(glisseurs);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}