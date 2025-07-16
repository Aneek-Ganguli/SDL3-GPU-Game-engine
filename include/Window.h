#pragma once
#include <stdio.h>
#include <SDL3/SDL.h>

struct Window{
    SDL_Window* window;
    SDL_GPUDevice* device;
    SDL_GPUCommandBuffer* commandBuffer;
    SDL_GPURenderPass* renderPass;
    SDL_GPUTexture* swapchainTexture;
    SDL_GPUGraphicsPipeline* pipeline;
    SDL_GPUShader* vertexShader;
    SDL_GPUShader* fragmentShader;
};

struct Window createWindow();
void newFrame(struct Window *window);
void endFrame(struct Window* window);
SDL_GPUShader* load_shader(
    SDL_GPUDevice* device,
    const char* filename,
    SDL_GPUShaderStage stage,
    Uint32 sampler_count,
    Uint32 uniform_buffer_count,
    Uint32 storage_buffer_count,
    Uint32 storage_texture_count);