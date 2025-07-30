#pragma once
#include <stdio.h>
#include <SDL3/SDL.h>

#include "Window.h"
#include "VertexData.h"

typedef struct Entity
{
    SDL_GPUBuffer *vertexBuffer;
    SDL_GPUTransferBuffer *transferBuffer;
    void *transferMem;
    SDL_GPUTransferBufferLocation vertexTransferBufferLocation;
    SDL_GPUBufferRegion vertexBufferRegion;
    struct VertexData *vertexData;
    SDL_GPUBufferBinding vertexBufferBinding;
    SDL_GPUBuffer *indexBuffer;
    Uint32 *indicies;
    SDL_GPUTransferBufferLocation indexTransferBufferLocation;
    SDL_GPUBufferRegion indexBufferRegion;
    SDL_GPUBufferBinding indexBufferBinding;
    int verticiesCount, indiciesCount;
    SDL_GPUTexture *texture;
    SDL_GPUTransferBuffer *textureTransferBuffer;
    SDL_Surface* surface;
    void* textureTransferMem;
    SDL_GPUTextureRegion textureRegion;
    SDL_GPUTextureSamplerBinding textureSamplerBinding;
    SDL_GPUTextureTransferInfo textureTransferInfo;
} Entity;

void createEntity(struct VertexData *vertexData, size_t verticies_count, Uint32 *indicies,
                  size_t indicies_count, const char* fileName,struct Window *window, struct Entity *e);

void drawEntity(struct UBO *ubo, size_t size, struct Window *window, Entity *e);