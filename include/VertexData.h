#pragma once
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
// #define CGLTF_IMPLEMENTATION
// #include <cgltf/cgltf.h>

typedef struct VertexData {
    vec3 position;
    vec2 texCoords;
    SDL_FColor color;
}VertexData;

typedef struct UBO {
	mat4 mvp;
} UBO;

typedef struct Transform {
    vec3 position;
    vec3 rotation; // radians
    vec3 scale;
} Transform;

vec2* normalize(float x, float y);
