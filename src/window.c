#include <stdio.h>
#include <SDL3/SDL.h>
#include "Window.h"

struct Window createWindow(){
    struct SDL_Window* s_window = NULL;
    s_window = SDL_CreateWindow("Goo Goo Ga Ga Bitch",800,600,SDL_WINDOW_VULKAN);
    if(s_window == NULL){
        printf("Error creating window: %s\n",SDL_GetError());
    }

    struct SDL_GPUDevice* s_device = NULL;
    s_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV,true,NULL);
    if(s_device == NULL){
        printf("Error creating gpu device: %s: \n",SDL_GetError());
    }

    SDL_ClaimWindowForGPUDevice(s_device,s_window);



    return (struct Window){
            .window = s_window,
            .device = s_device
        };
}

void newFrame(struct Window *window){
    window->commandBuffer = SDL_AcquireGPUCommandBuffer(window->device);
    if(window->commandBuffer == NULL){
        printf("Error aquire command buffer: %s: \n",SDL_GetError());
    }
    
    if(!SDL_WaitAndAcquireGPUSwapchainTexture(window->commandBuffer,window->window,&window->swapchainTexture,NULL,NULL)){
        printf("Error acquire swapchain texture: %s\n",SDL_GetError());
    }

    window->renderPass =  SDL_BeginGPURenderPass(window->commandBuffer,
        &(struct SDL_GPUColorTargetInfo ){
            .texture = window->swapchainTexture,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
            .clear_color = {0.0f,1.0f,1.0f,1.0f}
        },
        1,
        NULL
    );
    if(window->renderPass == NULL){
        printf("Error begain render pass: %s \n",SDL_GetError());
    }

}

void endFrame(struct Window *window){
    SDL_EndGPURenderPass(window->renderPass);
    if(!SDL_SubmitGPUCommandBuffer(window->commandBuffer)){
        printf("Error submit command buffer: %s\n",SDL_GetError());
    }
}