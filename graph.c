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
    SDL_Rect rect = {400, 400, 10, 10};
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, *texture,NULL, &rect);
    // SDL RenderCopyEx (renderer,∗texture, NULL, &destRect,angle, NULL, SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);
}

// v o i d l o a d T e x t u r e ( SDL Renderer ∗ r e n d e r e r , SDL Texture ∗∗ t e x t u r e ) {
// SDL Surface ∗ s u r f a c e = IMG Load ( ”/mnt/ c / U s e r s / g u i l l / workspace / IN104 / f i s h b i t m a p. png ” ) ;
// i f ( s u r f a c e == NULL) {
// f p r i n t f ( s t d e r r , ” F a i l e d t o l o a d image : %s \n ” , IMG GetError ( ) ) ;
// SDL Quit ( ) ;
// e x i t ( 1 ) ;
// }
// ∗ t e x t u r e = SDL CreateTextureFromSurface ( r e n d e r e r , s u r f a c e ) ;

// S D L F r e e S u r f a c e ( s u r f a c e ) ;
// i f ( ∗ t e x t u r e == NULL) {
// f p r i n t f ( s t d e r r , ” F a i l e d t o c r e a t e t e x t u r e : %s \n ” , SDL GetError ( ) ) ;
// SDL Quit ( ) ;
// e x i t ( 1 ) ;
// }
// }

int affichage(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "SDL initianlization, failed: %s\n", SDL_GetError());
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
    // Comm à recopier

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