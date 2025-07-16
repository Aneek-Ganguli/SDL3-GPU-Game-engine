#include <stdio.h>
#include <SDL3/SDL.h>
#include <cglm/cglm.h>

#include "Window.h"

struct UBO {
	mat4 mvp;
};

int main(){
    if(!SDL_Init(SDL_INIT_VIDEO )){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }
    struct Window window = createWindow();

    int width, height;
	SDL_GetWindowSize(window.window, &width, &height);

    
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
        struct UBO ubo;
        glm_mat4_copy(mvp, ubo.mvp);

        rotation += rotationSpeed * deltaTime;
        glm_mat4_identity(model);
        glm_translate(model, (vec3){0.0f, 0.0f, -10.0f});
        glm_rotate(model, rotation, (vec3){0.0f, 1.0f, 0.0f});
        
        newFrame(&window);

        SDL_PushGPUVertexUniformData(window.commandBuffer, 0, &ubo, sizeof(ubo));
        SDL_DrawGPUPrimitives(window.renderPass, 3, 1, 0, 0);
        
        endFrame(&window); 
    }
    
    SDL_Quit();
    return 0;
}