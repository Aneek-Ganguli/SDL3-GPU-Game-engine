#include <stdio.h>
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include "Window.h"


SDL_GPUShader* load_shader(
    SDL_GPUDevice* device,
    const char* filename,
    SDL_GPUShaderStage stage,
    Uint32 sampler_count,
    Uint32 uniform_buffer_count,
    Uint32 storage_buffer_count,
    Uint32 storage_texture_count) {

    if (!SDL_GetPathInfo(filename, NULL)) {
        fprintf(stdout, "File (%s) does not exist.\n", filename);
        return NULL;
    }

    const char* entrypoint;
    SDL_GPUShaderFormat backend_formats = SDL_GetGPUShaderFormats(device);
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
    if (backend_formats & SDL_GPU_SHADERFORMAT_SPIRV) {
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    }

    size_t code_size;
    void* code = SDL_LoadFile(filename, &code_size);
    if (code == NULL) {
        fprintf(stderr, "ERROR: SDL_LoadFile(%s) failed: %s\n", filename, SDL_GetError());
        return NULL;
    }

    SDL_GPUShaderCreateInfo shader_info;
    shader_info.code = (const Uint8*)code;
    shader_info.code_size = code_size;
    shader_info.entrypoint = entrypoint;
    shader_info.format = format;
    shader_info.stage = stage;
    shader_info.num_samplers = sampler_count;
    shader_info.num_uniform_buffers = uniform_buffer_count;
    shader_info.num_storage_buffers = storage_buffer_count;
    shader_info.num_storage_textures = storage_texture_count;

    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shader_info);

    if (shader == NULL) {
        fprintf(stderr, "ERROR: SDL_CreateGPUShader failed: %s\n", SDL_GetError());
        SDL_free(code);
        return NULL;
    }
    SDL_free(code);
    return shader;
}

struct Window   createWindow(){
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

    SDL_GPUShader* frag = load_shader(s_device,"C:/Users/Reall/Desktop/SDL3 GPU c/bin/shader.spv.frag",SDL_GPU_SHADERSTAGE_FRAGMENT,
    0,0,0,0);
    SDL_GPUShader* vert = load_shader(s_device,"C:/Users/Reall/Desktop/SDL3 GPU c/bin/shader.spv.vert",SDL_GPU_SHADERSTAGE_VERTEX,
    0,1,0,0);

    SDL_GPUVertexInputState vertexInput = (SDL_GPUVertexInputState){
                .num_vertex_buffers = 1,
                .num_vertex_attributes = 1,
                .vertex_buffer_descriptions = &(SDL_GPUVertexBufferDescription){
                    .slot = 0,
                    .pitch = sizeof(vec3),
                    .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                    // .instance_step_rate = 0
                },
                .vertex_attributes = &(SDL_GPUVertexAttribute){
                    .location = 0,
                    .offset = 0,
                    .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                    .buffer_slot = 0
                }
    }; 

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(s_device,
        &(SDL_GPUGraphicsPipelineCreateInfo){
            .vertex_shader = vert,
            .fragment_shader = frag,
            .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
            .target_info = (SDL_GPUGraphicsPipelineTargetInfo ){
                .num_color_targets = 1,
                .color_target_descriptions = &(SDL_GPUColorTargetDescription){
                    .blend_state = (SDL_GPUColorTargetBlendState ){
                        .enable_blend = false
                    },
                    .format = SDL_GetGPUSwapchainTextureFormat(s_device, s_window)
                }
            },
            .vertex_input_state = vertexInput
        }
    );

    return (struct Window){
            .window = s_window,
            .device = s_device,
            .vertexShader = vert,
            .fragmentShader = frag,
            .pipeline = pipeline
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



    SDL_BindGPUGraphicsPipeline(window->renderPass,window->pipeline);

}

void endFrame(struct Window *window){
    SDL_EndGPURenderPass(window->renderPass);
    if(!SDL_SubmitGPUCommandBuffer(window->commandBuffer)){
        printf("Error submit command buffer: %s\n",SDL_GetError());
    }
}

SDL_GPUBuffer* createBuffer(Uint32 size,SDL_GPUBufferUsageFlags usage ,struct Window* window){

    return SDL_CreateGPUBuffer(window->device,
        &(SDL_GPUBufferCreateInfo){
            .usage = usage,
            .size = size
        }
    );
}

void startCopyPass(struct Window* window){
    window->copyCommandBuffer = SDL_AcquireGPUCommandBuffer(window->device);
    window->copyPass = SDL_BeginGPUCopyPass(window->copyCommandBuffer);
    if(window->copyPass == NULL){
        printf("Error begain copy pass: %s\n",SDL_GetError());
    }
}

void endCopyPass(struct Window* window){
    SDL_EndGPUCopyPass(window->copyPass);
}

SDL_GPUTransferBuffer* createTransferBuffer(Uint32 size,struct Window* window){
    return SDL_CreateGPUTransferBuffer(window->device,
        &(SDL_GPUTransferBufferCreateInfo){
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = size
        }
    );
}

void* createTransferMem(SDL_GPUTransferBuffer* transferBuffer,void* data,struct Window* window){
    void* vertexMem = SDL_MapGPUTransferBuffer(window->device, transferBuffer,false);
	memcpy(vertexMem, data, sizeof(data));
    return vertexMem;
}

SDL_GPUTransferBufferLocation createTransferBufferLocation(SDL_GPUTransferBuffer* transferBuffer){
    return (SDL_GPUTransferBufferLocation){
        .transfer_buffer = transferBuffer
    };
}

SDL_GPUBufferRegion createBufferRegion(Uint32 size,SDL_GPUBuffer* buffer){
    return (SDL_GPUBufferRegion){
        .buffer = buffer,
        .size = size
    };
}

void uploadBuffer(SDL_GPUTransferBufferLocation* transferBufferLocation, SDL_GPUBufferRegion* bufferRegion, struct Window* window){
    SDL_UploadToGPUBuffer(window->copyPass, transferBufferLocation, bufferRegion, false);
}

SDL_GPUBufferBinding createBufferBinding(SDL_GPUBuffer* buffer){

    return (SDL_GPUBufferBinding){
        .buffer = buffer,
        .offset = 0
    };
}