#pragma once
#include <stdio.h>
#include <SDL3/SDL.h>
#include "Window.h"

struct Entity{
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUTransferBuffer* vertexTransferBuffer;
    void* vertexMemory;
    SDL_GPUTransferBufferLocation vertexTransferBufferLocation;
    SDL_GPUBufferRegion vertexBufferRegion;
    vec3* verticies;
    SDL_GPUBufferBinding vertexBufferBinding;
};

struct Entity createEntity(vec3 verticies[], size_t verticies_count, struct Window* window);