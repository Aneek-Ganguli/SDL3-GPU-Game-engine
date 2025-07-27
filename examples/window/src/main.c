#include <stdio.h>
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include <stb/stb_image.h>
#include <SDL3_image/SDL_image.h>

#include "Window.h"
#include "Entity.h"
#include "VertexData.h"

int main(){
    if(!SDL_Init(SDL_INIT_VIDEO)){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);    

    Window window = createWindow();

    SDL_Event event;
    bool running = true;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }
    }

    SDL_Quit();
    return 0;
}