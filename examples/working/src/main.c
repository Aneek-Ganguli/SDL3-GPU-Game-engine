#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cglm/cglm.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// #define CGLTF_IMPLEMENTATION
// #include <cgltf/cgltf.h>

#include "Window.h"
#include "Entity.h"
#include "Component.h"
#include "VertexData.h"

int main(void){
    
    // IMG_Init(IMG_INIT_PNG);

    struct Window window = createWindow("Version 1.0.0",800,600);

    window.vertexShader = loadShader(&window, "shader.vert", 0, 1, 0, 0);

    window.fragmentShader = loadShader(&window, "shader.frag", 1, 0, 0, 0);

    // IMPORTANT: pipeline’s vertex layout must match the struct we use below.
    createGraphicsPipeline( &window,70.0f);

    int width, height;
    SDL_GetWindowSize(window.window, &width, &height);

    Uint32* indices; //= {0,1,2, 2,1,3};
    unsigned int verticiesNum, indiciesNum;
    VertexData* verticies = load_model("../bin/res/viking_room.obj",&indices,&verticiesNum,&indiciesNum,1);

    startCopyPass(&window);
    struct Entity ent;

    //createEntity(verticies, verticiesNum, indices, indiciesNum, "res/viking_room.png", (vec3){1,1,1},&window, &ent);
    createEntity(verticies,verticiesNum,indices,(size_t)indiciesNum,"res/viking_room.png",(vec3){0,-1,-3},(vec3){1,1,1},&window,&ent);
    endCopyPass(&window); 

    mat4 P;
    glm_perspective(glm_rad(70.0f), (float)width/height, 0.1f, 1000.0f, P);

    
    
    Uint64 last = SDL_GetPerformanceCounter();
    float rot = 0.0f;

    bool running = true;
    SDL_Event event;
    while (running) {
        Uint64 now = SDL_GetPerformanceCounter();
        float dt = (float)(now - last) / SDL_GetPerformanceFrequency();
        last = now;
        mat4 M;
        glm_mat4_identity(M);
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }

        
        rot += glm_rad(60.0f) * dt;

        // printf("Frame Time: %f\n",dt);

        
        glm_translate(M, (vec3){0.0f, -1.0f, -3.0f});
        glm_rotate(M, rot, (vec3){0.0f, 1.0f, 0.0f});
        glm_rotate_x(M, glm_rad(-90.0f), M);

        struct UBO ubo;
        glm_mat4_mul(P, M, ubo.mvp);

        newFrame(&window);
        drawEntity(&window, &ent);
        endFrame(&window);
    }

    destroyEntity(&ent,&window);
    cleanUp(&window);
    return 0;
}
