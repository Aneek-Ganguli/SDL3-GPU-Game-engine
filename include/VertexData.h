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

void perspective_bounds(float fovy, float aspect, float z,
                        float *xmin, float *xmax,
                        float *ymin, float *ymax);

vec2* normalize(float x, float y);
