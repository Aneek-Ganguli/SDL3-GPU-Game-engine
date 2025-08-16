#pragma once
#include <stdio.h>
#include <SDL3/SDL.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// #define CGLTF_IMPLEMENTATION
// #include <cgltf/cgltf.h>

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
    SDL_GPUTextureTransferInfo textureTransferInfo;
    SDL_GPUTextureRegion textureRegion;
    SDL_GPUTextureSamplerBinding textureSamplerBinding;
    vec3 position;
    UBO uboPosition;
} Entity;

void createEntity(struct VertexData *vertexData, size_t verticies_count, Uint32 *indicies,
                  size_t indicies_count, const char* fileName,vec3 position,struct Window *window, struct Entity *e);

void createEntityWithModel(const char* modelFileName,const char* textureFileName,Window* window, Entity* entity);

void drawEntity(struct Window *window, Entity *e);



VertexData* load_model(
    const char* path,
    unsigned int** out_indices,
    unsigned int* out_vertex_count,
    unsigned int* out_index_count,
    float scale // <-- new parameter
);

