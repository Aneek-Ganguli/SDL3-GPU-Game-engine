#include <stdio.h>
#include <SDL3/SDL.h>
#include <cglm/cglm.h>

#include "Window.h"

struct UBO {
	mat4 mvp;
};

int main(){
    
    if(!SDL_Init(SDL_INIT_VIDEO)){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

    struct Window window = createWindow();

    int width, height;
	SDL_GetWindowSize(window.window, &width, &height);


	vec3 vertices[] = {
		{-1.0f, -1.0f, 0.0f, },
		{ 1.0f, -1.0f, 0.0f, },
		{0.0f, 1.0f, 0.0f }
	};
    
    SDL_GPUBuffer* vertexBuffer = createBuffer(sizeof(vertices),SDL_GPU_BUFFERUSAGE_VERTEX,&window);
    if(!vertexBuffer){
        printf("Error creating vertex buffer: %s\n",SDL_GetError());
    }


    SDL_GPUTransferBuffer* vertexTransferBuffer = createTransferBuffer(sizeof(vertices),&window);
    if(!vertexTransferBuffer){
        printf("Error creating vertex transfer buffer: %s\n",SDL_GetError());
    }

    void* vertexMemory = createTransferMem(vertexTransferBuffer,vertices,sizeof(vertices),&window);
    if(!vertexMemory){
        printf("Error creating transfer memory: %s\n",SDL_GetError());
    }
	SDL_UnmapGPUTransferBuffer(window.device, vertexTransferBuffer);


	const float rotationSpeed = glm_rad(90.0f);

	float rotation = 0.0f;

	// ...existing code...
    mat4 Projection;
    glm_perspective(glm_rad(70.0f), (float)width / height, 0.000001f, 10000.0f, Projection);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){0.0f, 0.0f, -10.0f});
    glm_rotate(model, rotation, (vec3){0.0f, 1.0f, 0.0f});

	Uint64 lastTime = SDL_GetPerformanceCounter();


    startCopyPass(&window);

    SDL_GPUTransferBufferLocation vertexTransferBufferLocation = createTransferBufferLocation(vertexTransferBuffer);
    
    SDL_GPUBufferRegion vertexBufferRegion = createBufferRegion(sizeof(vertices),vertexBuffer);

    uploadBuffer(&vertexTransferBufferLocation,&vertexBufferRegion,&window);

    endCopyPass(&window);

    SDL_GPUBufferBinding vertexBufferBinding = createBufferBinding(vertexBuffer);

    bool running = true;
    SDL_Event event;
    while (running){

        Uint64 currentTime = SDL_GetPerformanceCounter();
		float deltaTime = (float)(currentTime - lastTime) / SDL_GetPerformanceFrequency();
		lastTime = currentTime;

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }

        mat4 mvp;
        glm_mat4_mul(Projection, model, mvp);
        struct UBO ubo = {0};
        glm_mat4_copy(mvp, ubo.mvp);

        rotation += rotationSpeed * deltaTime;
        glm_mat4_identity(model);
        glm_translate(model, (vec3){0.0f, 0.0f, -10.0f});
        glm_rotate(model, rotation, (vec3){0.0f, 1.0f, 0.0f});
        
        newFrame(&window);

        SDL_BindGPUVertexBuffers(window.renderPass,0,&vertexBufferBinding,1);
        SDL_PushGPUVertexUniformData(window.commandBuffer, 0, &ubo, sizeof(ubo));
        SDL_DrawGPUPrimitives(window.renderPass, 3, 1, 0, 0);
        
        endFrame(&window); 
    }
    
    SDL_Quit();
    return 0;
}