#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cglm/cglm.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Window.h"
#include "VertexData.h"

SDL_GPUShader* loadShader(
	Window* window,
	const char* shaderFilename,
	Uint32 samplerCount,
	Uint32 uniformBufferCount,
	Uint32 storageBufferCount,
	Uint32 storageTextureCount
) {
	// Auto-detect the shader stage from the file name for convenience
	SDL_GPUShaderStage stage;
	if (SDL_strstr(shaderFilename, ".vert"))
	{
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
	}
	else if (SDL_strstr(shaderFilename, ".frag"))
	{
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	}
	else
	{
		SDL_Log("Invalid shader stage!");
		return NULL;
	}

	char fullPath[256];
	SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(window->device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	const char *entrypoint;

	if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.spv","../bin/shader", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.msl", "../bin/shader", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main0";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s.dixil","../bin/shader", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_DXIL;
		entrypoint = "main";
	} else {
		SDL_Log("%s", "Unrecognized backend shader format!");
		return NULL;
	}

	size_t codeSize;
	void* code = SDL_LoadFile(fullPath, &codeSize);
	if (code == NULL)
	{
		SDL_Log("Failed to load shader from disk! %s", fullPath);
		return NULL;
	}

	SDL_GPUShaderCreateInfo shaderInfo = {
		.code = code,
		.code_size = codeSize,
		.entrypoint = entrypoint,
		.format = format,
		.stage = stage,
		.num_samplers = samplerCount,
		.num_uniform_buffers = uniformBufferCount,
		.num_storage_buffers = storageBufferCount,
		.num_storage_textures = storageTextureCount
	};
	SDL_GPUShader* shader = SDL_CreateGPUShader(window->device, &shaderInfo);
	if (shader == NULL)
	{
		SDL_Log("Failed to create shader!");
		SDL_free(code);
		return NULL;
	}

	SDL_free(code);
	return shader;
}

struct Window createWindow(const char* title,int width,int height){
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
    }
    struct SDL_Window* s_window = NULL;
    s_window = SDL_CreateWindow(title,width,height,SDL_WINDOW_VULKAN);
    if(s_window == NULL){
        printf("Error creating window: %s\n",SDL_GetError());
    }

    struct SDL_GPUDevice* s_device = NULL;
    s_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV,true,NULL);
    if(s_device == NULL){
        printf("Error creating gpu device: %s: \n",SDL_GetError());
    }

    SDL_ClaimWindowForGPUDevice(s_device,s_window);

    path = SDL_GetBasePath();

    // mat4 *P;
    // glm_perspective(glm_rad(70.0f), (float)width/height, 0.1f, 1000.0f, P);

    return (struct Window){
        .window = s_window,
        .device = s_device,
        .width = width,
        .height = height,
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
    if(window->copyCommandBuffer == NULL){
        printf("Error creating copy command buffer: %s\n",SDL_GetError());
    }
    window->copyPass = SDL_BeginGPUCopyPass(window->copyCommandBuffer);
    if(window->copyPass == NULL){
        printf("Error begain copy pass: %s\n",SDL_GetError());
    }
}

void endCopyPass(struct Window* window){
    SDL_EndGPUCopyPass(window->copyPass);
    if(!SDL_SubmitGPUCommandBuffer(window->copyCommandBuffer)){
        printf("Error submit copy pass: %s\n",SDL_GetError());
    }
}

SDL_GPUTransferBuffer* createTransferBuffer(Uint32 size,struct Window* window){
    return SDL_CreateGPUTransferBuffer(window->device,
        &(SDL_GPUTransferBufferCreateInfo){
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = size
        }
    );
}


SDL_GPUTransferBufferLocation createTransferBufferLocation(SDL_GPUTransferBuffer* transferBuffer,Uint32 offset){
    return (SDL_GPUTransferBufferLocation){
        .transfer_buffer = transferBuffer,
        .offset = offset
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

void createGraphicsPipeline(Window* window,float Fov){
    SDL_GPUVertexInputState vertexInput = (SDL_GPUVertexInputState){
                .num_vertex_buffers = 1,
                .num_vertex_attributes = 3,
                .vertex_buffer_descriptions = &(SDL_GPUVertexBufferDescription){
                    .slot = 0,
                    .pitch = sizeof(struct VertexData),
                    .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                    // .instance_step_rate = 0
                },
                .vertex_attributes = (SDL_GPUVertexAttribute[]){
                    [0] = {
                        .location = 0,
                        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                        .offset = offsetof(struct VertexData,position),
                    },
                    [1] = {
                        .location = 1,
                        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                        .offset = offsetof(struct VertexData,texCoords),
                    },
                    [2] = {
                        .location = 2,
                        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                        .offset = offsetof(struct VertexData,color),
                    }
                }
    }; 

    window->pipeline = SDL_CreateGPUGraphicsPipeline(window->device,
        &(SDL_GPUGraphicsPipelineCreateInfo){
            .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
            .target_info = (SDL_GPUGraphicsPipelineTargetInfo ){
                .num_color_targets = 1,
                .color_target_descriptions = &(SDL_GPUColorTargetDescription){
                    .blend_state = (SDL_GPUColorTargetBlendState ){
                        .enable_blend = false
                    },
                    .format = SDL_GetGPUSwapchainTextureFormat(window->device, window->window)
                }
            },
            .vertex_input_state = vertexInput,
            .vertex_shader = window->vertexShader,
            .fragment_shader = window->fragmentShader
        }
    );
    if(window->pipeline == NULL){
        printf("Erro graphics pipeline :%s\n",SDL_GetError());
    }

    window->sampler = createGPUSampler(window);
    window->fov = Fov;
    windowWidth = window->width;
    windowHeight = window->height;
    fov = window->fov;

    glm_perspective(fov, (float)window->width/window->height, 0.1f, 1000.0f, window->P);
}

SDL_Surface* loadImage(const char* imageFilename, int desiredChannels){
		char fullPath[256];
	SDL_Surface *result;
	SDL_PixelFormat format;

	SDL_snprintf(fullPath, sizeof(fullPath), "%s/%s", path, imageFilename);

	result = IMG_Load(fullPath);
	if (result == NULL){
		SDL_Log("Failed to load BMP: %s", SDL_GetError());
		return NULL;
	}

	if (desiredChannels == 4){
		format = SDL_PIXELFORMAT_ABGR8888;
	}
	else{
		SDL_assert(!"Unexpected desiredChannels");
		SDL_DestroySurface(result);
		return NULL;
	}
	if (result->format != format){
		SDL_Surface *next = SDL_ConvertSurface(result, format);
		SDL_DestroySurface(result);
		result = next;
	}

	return result;
}

SDL_GPUTexture* createTexture(SDL_Surface* surface,Window* window){
    return SDL_CreateGPUTexture(window->device, &(SDL_GPUTextureCreateInfo){
		// .type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
		.width = surface->w,
		.height = surface->h,
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
    });

}

SDL_GPUSampler* createGPUSampler(Window* window){
    return SDL_CreateGPUSampler(window->device,&(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
}

void uploadTexture(SDL_GPUTextureTransferInfo textureTransferInfo,SDL_GPUTextureRegion textureRegion,Window* window){
    SDL_UploadToGPUTexture(window->copyPass,&textureTransferInfo,&textureRegion,false);
}

void cleanUp(Window* window){
    SDL_ReleaseGPUShader(window->device,window->vertexShader);
    SDL_ReleaseGPUShader(window->device,window->fragmentShader);
    SDL_ReleaseGPUGraphicsPipeline(window->device,window->pipeline);
    SDL_ReleaseGPUSampler(window->device,window->sampler);
    SDL_DestroyGPUDevice(window->device);
    SDL_DestroyWindow(window->window);
    SDL_Quit();
}


