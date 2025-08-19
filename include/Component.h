#pragma onec
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "Window.h"
#include "VertexData.h"

typedef struct{
    vec3 position;
}Position3D; 

Position3D createPosition3D(vec3 position);
// ok so i want to get the z and calculate thhe maximum x,y and clamp it
