#pragma once
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "Window.h"
#include "VertexData.h"

typedef struct Transform3D{
    vec3 position,scale,rotate;
    mat4 M;
    float rotateAngle;
    UBO transform;
}Transform3D;

Transform3D initializeTransform3D(vec3 position,vec3 scale,vec3 rotate,float rotateAngle);

// ok so i want to get the z and calculate thhe maximum x,y and clamp it
