#pragma once
#include <stdio.h>
#include <SDL3/SDL.h>

struct Window{
    SDL_Window* window;
    SDL_GPUDevice* device;
    SDL_GPUCommandBuffer* commandBuffer;
    SDL_GPURenderPass* renderPass;
    SDL_GPUTexture* swapchainTexture;
};

struct Window createWindow();
void newFrame(struct Window *window);
void endFrame(struct Window* window);