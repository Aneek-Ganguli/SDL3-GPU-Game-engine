#include <stdio.h>
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include <stb/stb_image.h>
#include <SDL3_image/SDL_image.h>

#ifdef _WIN32         // For Windows-specific headers
#include <direct.h>   // For _getcwd on Windows
#define getcwd _getcwd
#endif


#include "Window.h"
#include "Entity.h"
#include "VertexData.h"


int main(){
    
    if(!SDL_Init(SDL_INIT_VIDEO)){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

    char current_path[1000];
    if (getcwd(current_path, sizeof(current_path)) != NULL) {
        printf("Current directory: %s\n", current_path);
    } else {
        perror("getcwd() error"); // Print an error message if getcwd fails
        return 1; // Indicate an error
    }

    struct Window window = createWindow();
    SDL_GPUShader* frag = load_shader(window.device,"../bin/shader.spv.frag",SDL_GPU_SHADERSTAGE_FRAGMENT,
    0,0,0,0);
    if(frag ==NULL){
        return -1;
    }

    SDL_GPUShader* vert = load_shader(window.device,"../bin/shader.spv.vert",SDL_GPU_SHADERSTAGE_VERTEX,
    0,1,0,0);
    
    setShader(vert,frag,&window);

    int width, height;
	SDL_GetWindowSize(window.window, &width, &height);


	// vec3 vertices[] = {
	// 	{-1.0f, -1.0f, 0.0f, },
	// 	{ 1.0f, -1.0f, 0.0f, },
	// 	{0.0f, 1.0f, 0.0f }
	// };

    // struct VertexData vertexData[] = {
	// 	{-1.0f, -1.0f, 0.0f,{1.0f,0.0f,0.0f,1.0f} },
	// 	{ 1.0f, -1.0f, 0.0f,{0.0f,1.0f,0.0f,1.0f} },
	// 	{ 0.0f,  1.0f, 0.0f,{0.0f,0.0f,1.0f,1.0f} }
	// };

    

    struct VertexData vertices[] = {
        {{-0.5f,  0.5f, 0.0f },{0,1} , {1.0f, 0.0f, 0.0f, 1.0f}}, // Triangle 1
        {{ 0.5f,  0.5f, 0.0f },{1,1} , {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.0f },{0,0} , {0.0f, 0.0f, 1.0f, 1.0f}},
        {{ 0.5f, -0.5f, 0.0f },{1,0} , {1.0f, 1.0f, 0.0f, 1.0f}}, // Triangle 2
    };

    Uint32 indices[] = {
        0, 1, 2,
        2, 1, 3
    };

	const float rotationSpeed = glm_rad(90.0f);

	float rotation = 0.0f;

	// ...existing code...
    mat4 Projection;
    glm_perspective(glm_rad(70.0f), (float)width / height, 0.000001f, 10000.0f, Projection);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){0.0f, 0.0f, -10.0f});
    //glm_rotate(model, rotation, (vec3){0.0f, 1.0f, 0.0f});

	Uint64 lastTime = SDL_GetPerformanceCounter();

    

    startCopyPass(&window);

    struct Entity ent;
    createEntity(vertices,sizeof(vertices)/sizeof(vertices[0]),
                 indices,sizeof(indices)/sizeof(indices[0]),
                 &window,&ent);
    
    endCopyPass(&window);


    mat4 mvp;
    struct UBO ubo = {0};

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
            if(event.type == SDL_EVENT_KEY_DOWN){
                glm_mat4_mul(Projection, model, mvp);
            
                glm_mat4_copy(mvp, ubo.mvp);

                rotation += rotationSpeed * deltaTime;
                glm_mat4_identity(model);
                glm_translate(model, (vec3){0.0f, 0.0f, -10.0f});
                glm_rotate(model, rotation, (vec3){0.0f, 1.0f, 0.0f});
            }
        }
        newFrame(&window);
        drawEntity(&ubo,sizeof(ubo),&window,&ent);
        endFrame(&window); 
    }
    
    SDL_Quit();
    return 0;
}