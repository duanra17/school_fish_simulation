/*Fonctions utiles à l'affichage graphique*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define IMAGE_WIDTH 1200
#define IMAGE_HEIGHT 800

// Fenêtre
void render(SDL_Renderer *renderer, SDL_Texture **texture){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect rect = {300, 300, 10, 10};
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, *texture,NULL, &rect);
    // SDL RenderCopyEx (renderer,∗texture, NULL, &destRect,angle, NULL, SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);
}

int affichage(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "SDL initialization, failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("N-Body Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, IMAGE_WIDTH, IMAGE_HEIGHT, SDL_WINDOW_SHOWN);
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
    SDL_Texture *texture;
    // loadTexture(renderer, &texture); // Replace "particle.png" with your image file

    SDL_Event event;
    int quit = 0;
    while (!quit){
        while (SDL_PollEvent(&event) != 0){
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Render the updated positions
        render(renderer, &texture);

        //Delay to control the frame rate
        //SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}