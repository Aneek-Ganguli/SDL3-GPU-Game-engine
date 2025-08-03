#pragma once
#include <SDL3/SDL.h>
#include <cglm/cglm.h>

typedef struct Vec2{
    float x,y;
} Vec2;

struct VertexData {
    vec3 position;
    SDL_FColor color;
    vec2 texCoords;
};

struct UBO {
	mat4 mvp;
};

struct Vec2 normalize(float x, float y);