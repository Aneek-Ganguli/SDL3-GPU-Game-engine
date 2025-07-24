#pragma once
#include <SDL3/SDL.h>
#include <cglm/cglm.h>

struct VertexData {
    vec3 position;
    SDL_FColor color;
};

struct UBO {
	mat4 mvp;
};

struct Vec2{
    float x,y;
};

struct Vec2 normalize(float x, float y);