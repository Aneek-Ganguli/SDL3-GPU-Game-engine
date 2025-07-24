#include <stdio.h>
#include <SDL3/SDL.h>
#include "Window.h"
#include "Entity.h"
#include "VertexData.h"

void createEntity(struct VertexData* vertexData, size_t verticies_count,Uint32* indicies,
    size_t indicies_count,  struct Window* window,struct Entity* e){
    
    int vertexSize = sizeof(struct VertexData) * verticies_count;
    int indexSize = sizeof(Uint32) * indicies_count;

    e->vertexBuffer = createBuffer(vertexSize,
                                   SDL_GPU_BUFFERUSAGE_VERTEX, window);
        
    if(!e->vertexBuffer){
        printf("Error creating vertex buffer: %s\n",SDL_GetError());
    }

    


    e->indexBuffer = createBuffer(indexSize,SDL_GPU_BUFFERUSAGE_INDEX,window);

    e->vertexData = vertexData;
    e->indicies = indicies;

    e->transferBuffer = createTransferBuffer(
        vertexSize + indexSize,
        window);
    
    if(!e->transferBuffer){
        printf("Error creating vertex transfer buffer: %s\n",SDL_GetError());
    }

    e->transferMem = SDL_MapGPUTransferBuffer(window->device, e->transferBuffer, false);

    memcpy(e->transferMem, vertexData, vertexSize);

    memcpy((char*)e->transferMem + vertexSize,
           indicies, indexSize);

    // e->transferMem = createTransferMem(e->transferBuffer, vertexData, 
    //                                    sizeof(struct VertexData) * verticies_count,
    //                                    window);
  
    if(!e->transferMem){
        printf("Error creating transfer memory: %s\n",SDL_GetError());
    }
    // SDL_UnmapGPUTransferBuffer(window->device, e->transferBuffer);

    e->vertexTransferBufferLocation = createTransferBufferLocation(e->transferBuffer,0);
    e->vertexBufferRegion = createBufferRegion(vertexSize,
                                               e->vertexBuffer);

    //indicies
    e->indexTransferBufferLocation = createTransferBufferLocation(e->transferBuffer,
        vertexSize);

    e->indexBufferRegion = createBufferRegion(indexSize,e->indexBuffer);

    uploadBuffer(&e->vertexTransferBufferLocation, &e->vertexBufferRegion, window);
    uploadBuffer(&e->indexTransferBufferLocation, &e->indexBufferRegion, window);

    e->vertexBufferBinding = createBufferBinding(e->vertexBuffer);
    e->indexBufferBinding = createBufferBinding(e->indexBuffer);
}