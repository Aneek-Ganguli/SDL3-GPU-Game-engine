#pragma once
#include <SDL3/SDL.h>
#include <cglm/cglm.h>

struct Vec2{
    float x,y;
};

struct VertexData {
    vec3 position;
    struct Vec2 texCoords;
    SDL_FColor color;
};

struct UBO {
	mat4 mvp;
};

struct Vec2 normalize(float x, float y);