#pragma once
#include <stdio.h>
#include <SDL3/SDL.h>
#include <cglm/cglm.h>

struct Window{
    SDL_Window* window;
    SDL_GPUDevice* device;
    SDL_GPUCommandBuffer* commandBuffer;
    SDL_GPUCommandBuffer* copyCommandBuffer;
    SDL_GPURenderPass* renderPass;
    SDL_GPUTexture* swapchainTexture;
    SDL_GPUGraphicsPipeline* pipeline;
    SDL_GPUShader* vertexShader;
    SDL_GPUShader* fragmentShader;
    SDL_GPUCopyPass* copyPass;
};



struct Window createWindow();
void newFrame(struct Window *window);
void endFrame(struct Window* window);
SDL_GPUShader* load_shader(SDL_GPUDevice* device,const char* filename,SDL_GPUShaderStage stage,
                            Uint32 sampler_count,Uint32 uniform_buffer_count,Uint32 storage_buffer_count,
                            Uint32 storage_texture_count);
SDL_GPUBuffer* createBuffer(Uint32 size,SDL_GPUBufferUsageFlags usage ,struct Window* window);
void startCopyPass(struct Window* window);
void endCopyPass(struct Window* window);
SDL_GPUTransferBuffer* createTransferBuffer(Uint32 size,struct Window* window);
void* createTransferMem(SDL_GPUTransferBuffer* transferBuffer, void* data,Uint32 size, struct Window* window);
SDL_GPUTransferBufferLocation createTransferBufferLocation(SDL_GPUTransferBuffer* transferBuffer);
SDL_GPUBufferRegion createBufferRegion(Uint32 size,SDL_GPUBuffer* buffer);
void uploadBuffer(SDL_GPUTransferBufferLocation* transferBufferLocation, SDL_GPUBufferRegion* bufferRegion, struct Window* window);
SDL_GPUBufferBinding createBufferBinding(SDL_GPUBuffer* buffer);
