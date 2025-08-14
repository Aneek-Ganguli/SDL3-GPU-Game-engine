#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cglm/cglm.h>

#include "Window.h"
#include "Entity.h"
#include "VertexData.h"

int main(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }
    // IMG_Init(IMG_INIT_PNG);

    

    struct Window window = createWindow();

    SDL_GPUShader* vert = load_shader(window.device, "../bin/shader/shader.spv.vert",
        SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, 0, 0);
    SDL_GPUShader* frag = load_shader(window.device, "../bin/shader/shader.spv.frag",
        SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, 0, 0);

    // IMPORTANT: pipeline’s vertex layout must match the struct we use below.
    setShader(vert, frag, &window);

    int width, height;
    SDL_GetWindowSize(window.window, &width, &height);

    // Positions + UVs; colors are ignored by the shaders above.
    struct VertexData vertices[] = {
        {{-0.5f,  0.5f, 0.0f}, {0, 1}, {1,1,1,1}},
        {{ 0.5f,  0.5f, 0.0f}, {1, 1}, {1,1,1,1}},
        {{-0.5f, -0.5f, 0.0f}, {0, 0}, {1,1,1,1}},
        {{ 0.5f, -0.5f, 0.0f}, {1, 0}, {1,1,1,1}},
    };
    Uint32 indices[] = {0,1,2, 2,1,3};

    startCopyPass(&window);
    struct Entity ent;
    createEntity(vertices, 4, indices, 6, "res/meow.png", &window, &ent);
    endCopyPass(&window);

    mat4 P;
    glm_perspective(glm_rad(70.0f), (float)width/height, 0.1f, 1000.0f, P);

    Uint64 last = SDL_GetPerformanceCounter();
    float rot = 0.0f;

    bool running = true;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) if (ev.type == SDL_EVENT_QUIT) running = false;

        Uint64 now = SDL_GetPerformanceCounter();
        float dt = (float)(now - last) / SDL_GetPerformanceFrequency();
        last = now;
        rot += glm_rad(60.0f) * dt;

        mat4 M;
        glm_mat4_identity(M);
        glm_translate(M, (vec3){0.0f, 0.0f, -3.0f});
        glm_rotate(M, rot, (vec3){0.0f, 1.0f, 0.0f});

        struct UBO ubo;
        glm_mat4_mul(P, M, ubo.mvp);

        newFrame(&window);
        drawEntity(&ubo, sizeof(ubo), &window, &ent);
        endFrame(&window);
    }

    SDL_Quit();
    return 0;
}
