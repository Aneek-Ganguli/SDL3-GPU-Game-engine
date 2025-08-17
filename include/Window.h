#pragma once
#include <stdio.h>
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
// #define CGLTF_IMPLEMENTATION
// #include <cgltf/cgltf.h>

#include "VertexData.h"

static const char* path = NULL;

typedef struct Window{
    SDL_Window *window;
    SDL_GPUDevice *device;
    SDL_GPUCommandBuffer *commandBuffer;
    SDL_GPUCommandBuffer *copyCommandBuffer;
    SDL_GPURenderPass *renderPass;
    SDL_GPUTexture *swapchainTexture;
    SDL_GPUGraphicsPipeline *pipeline;
    SDL_GPUShader *vertexShader;
    SDL_GPUShader *fragmentShader;
    SDL_GPUCopyPass *copyPass;
    SDL_GPUSampler* sampler;
    int width,height;
    mat4 P;
} Window;

struct Window createWindow(const char* title,int width,int height);
void setShader(SDL_GPUShader *vertexShader, SDL_GPUShader *fragmentShader, Window* window);
void newFrame(struct Window *window);
void endFrame(struct Window *window);
SDL_GPUShader *load_shader(Window* window, const char *filename, SDL_GPUShaderStage stage,
                           Uint32 sampler_count, Uint32 uniform_buffer_count, Uint32 storage_buffer_count,
                           Uint32 storage_texture_count);
SDL_GPUBuffer *createBuffer(Uint32 size, SDL_GPUBufferUsageFlags usage, struct Window *window);
void startCopyPass(struct Window *window);
void endCopyPass(struct Window *window);
SDL_GPUTransferBuffer *createTransferBuffer(Uint32 size, struct Window *window);
void *createTransferMem(SDL_GPUTransferBuffer *transferBuffer, void *data, Uint32 size, struct Window *window);
SDL_GPUTransferBufferLocation createTransferBufferLocation(SDL_GPUTransferBuffer *transferBuffer, Uint32 offset);
SDL_GPUBufferRegion createBufferRegion(Uint32 size, SDL_GPUBuffer *buffer);
void uploadBuffer(SDL_GPUTransferBufferLocation *transferBufferLocation, SDL_GPUBufferRegion *bufferRegion, struct Window *window);
SDL_GPUBufferBinding createBufferBinding(SDL_GPUBuffer *buffer);
SDL_Surface* loadImage(const char* imageFilename, int desiredChannels);
SDL_GPUTexture* createTexture(SDL_Surface* surface,Window* window);
SDL_GPUSampler* createGPUSampler(Window* window);
void uploadTexture(SDL_GPUTextureTransferInfo textureTransferInfo,SDL_GPUTextureRegion textureRegion,Window* window);
void cleanUp(Window* window);
