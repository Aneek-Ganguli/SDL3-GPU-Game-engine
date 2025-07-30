#include <stdio.h>
#include <SDL3/SDL.h>
#include "Window.h"
#include "Entity.h"
#include "VertexData.h"

void createEntity(struct VertexData *vertexData, size_t verticies_count, Uint32 *indicies,
                  size_t indicies_count, const char* fileName,struct Window *window, struct Entity *e){
    
    int vertexSize = sizeof(struct VertexData) * verticies_count;
    int indexSize = sizeof(Uint32) * indicies_count;

    e->verticiesCount = verticies_count;
    e->indiciesCount = indicies_count;

    e->vertexBuffer = createBuffer(vertexSize,
                                   SDL_GPU_BUFFERUSAGE_VERTEX, window);
        
    if(!e->vertexBuffer){
        printf("Error creating vertex buffer: %s\n",SDL_GetError());
    }

    //tetxture

    

    e->surface = loadImage(fileName,4);
    e->texture =SDL_CreateGPUTexture(window->device, &(SDL_GPUTextureCreateInfo){
		.type = SDL_GPU_TEXTURETYPE_2D_ARRAY,
		.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
		.width = e->surface->w,
		.height = e->surface->h,
		.layer_count_or_depth = 2,
		.num_levels = 1,
		.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
	});
    e->textureTransferBuffer = createTransferBuffer(e->surface->w * e->surface->h*4,window);
    e->textureTransferMem = SDL_MapGPUTransferBuffer(window->device, e->textureTransferBuffer, false);
	memcpy(e->textureTransferMem, e->surface->pixels, e->surface->w* e->surface->h  * 4);
    SDL_UnmapGPUTransferBuffer(window->device, e->textureTransferBuffer);

    e->textureRegion.texture = e->texture;
	e->textureRegion.w = e->surface->w;
	e->textureRegion.h = e->surface->h;
    e->textureRegion.d = 1;

    window->sampler = createGPUSampler(window);

    //data 
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

    //texture
    e->textureSamplerBinding.texture = e->texture;
	e->textureSamplerBinding.sampler = window->sampler;

    // e->textureTransferInfo;
	e->textureTransferInfo.transfer_buffer = e->textureTransferBuffer;
	e->textureTransferInfo.offset = 0;

    uploadBuffer(&e->vertexTransferBufferLocation, &e->vertexBufferRegion, window);
    uploadBuffer(&e->indexTransferBufferLocation, &e->indexBufferRegion, window);
    uploadTexture(e->textureTransferInfo,e->textureRegion,window);
    // uploadBuffer(&e->text)

    e->vertexBufferBinding = createBufferBinding(e->vertexBuffer);
    e->indexBufferBinding = createBufferBinding(e->indexBuffer);
}

void drawEntity(struct UBO* ubo,size_t size,struct Window* window,struct Entity* e){
    SDL_BindGPUVertexBuffers(window->renderPass, 0, &e->vertexBufferBinding, 1);
    SDL_BindGPUIndexBuffer(window->renderPass, &e->indexBufferBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
    SDL_PushGPUVertexUniformData(window->commandBuffer, 0, &ubo, sizeof(ubo));
    SDL_BindGPUFragmentSamplers(window->renderPass, 0,&e->textureSamplerBinding,1);
    SDL_DrawGPUIndexedPrimitives(window->renderPass, e->indiciesCount, 1, 0, 0, 0);
}
