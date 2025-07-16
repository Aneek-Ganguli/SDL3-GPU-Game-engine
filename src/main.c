#include <stdio.h>
#include <SDL3/SDL.h>

#include "Window.h"

int main(){
    if(!SDL_Init(SDL_INIT_VIDEO )){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }
    struct Window window = createWindow();

    bool running = true;
    SDL_Event event;
    while (running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }
        newFrame(&window);

        endFrame(&window); 
    }
    
    SDL_Quit();
    return 0;
}