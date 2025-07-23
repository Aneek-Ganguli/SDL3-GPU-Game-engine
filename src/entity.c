#include <stdio.h>
#include <SDL3/SDL.h>
#include "Window.h"
#include "Entity.h"
#include "VertexData.h"

struct Entity createEntity(struct VertexData* vertexData, size_t verticies_count, struct Window* window){
    SDL_GPUBuffer* vertexBuffer = createBuffer(sizeof(struct VertexData) * verticies_count, SDL_GPU_BUFFERUSAGE_VERTEX, window);
    if(!vertexBuffer){
        printf("Error creating vertex buffer: %s\n",SDL_GetError());
    }


    SDL_GPUTransferBuffer* vertexTransferBuffer = createTransferBuffer(sizeof(struct VertexData) * verticies_count, window);
    if(!vertexTransferBuffer){
        printf("Error creating vertex transfer buffer: %s\n",SDL_GetError());
    }

    void* vertexMemory = createTransferMem(vertexTransferBuffer, vertexData, sizeof(struct VertexData) * verticies_count, window);
    if(!vertexMemory){
        printf("Error creating transfer memory: %s\n",SDL_GetError());
    }
    SDL_UnmapGPUTransferBuffer(window->device, vertexTransferBuffer);

    SDL_GPUTransferBufferLocation vertexTransferBufferLocation = createTransferBufferLocation(vertexTransferBuffer);
    
    SDL_GPUBufferRegion vertexBufferRegion = createBufferRegion(sizeof(struct VertexData) * verticies_count, vertexBuffer);

    uploadBuffer(&vertexTransferBufferLocation, &vertexBufferRegion, window);

    SDL_GPUBufferBinding vertexBufferBinding = createBufferBinding(vertexBuffer);

    return (struct Entity){
        .vertexData = vertexData,
        .vertexBuffer = vertexBuffer,
        .vertexTransferBuffer = vertexTransferBuffer,
        .vertexMemory = vertexMemory,
        .vertexTransferBufferLocation = vertexTransferBufferLocation,
        .vertexBufferRegion = vertexBufferRegion,
        .vertexBufferBinding = vertexBufferBinding
    };
}