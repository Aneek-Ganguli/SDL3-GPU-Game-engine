#include <stdio.h>
#include <string.h>
#include <SDL3/SDL.h>
// #define CGLTF_IMPLEMENTATION
// #include <cgltf/cgltf.h>

#include "Window.h"
#include "Entity.h"
#include "VertexData.h"


void createEntity(struct VertexData *vertexData, size_t vertices_count,
                  Uint32 *indices, size_t indices_count,
                  const char* fileName, struct Window *window, struct Entity *e)
{
    // Reset everything
    *e = (struct Entity){0};

    const Uint32 vertexSize = (Uint32)(sizeof(struct VertexData) * vertices_count);
    const Uint32 indexSize  = (Uint32)(sizeof(Uint32) * indices_count);

    e->verticiesCount = vertices_count;
    e->indiciesCount  = indices_count;

    // --- GPU buffers ---
    e->vertexBuffer = createBuffer(vertexSize, SDL_GPU_BUFFERUSAGE_VERTEX, window);
    e->indexBuffer  = createBuffer(indexSize,  SDL_GPU_BUFFERUSAGE_INDEX,  window);
    if (!e->vertexBuffer || !e->indexBuffer) {
        printf("Error creating vertex/index buffer: %s\n", SDL_GetError());
        return;
    }

    // --- Staging for vertex+index (one big upload) ---
    e->transferBuffer = createTransferBuffer(vertexSize + indexSize, window);
    if (!e->transferBuffer) {
        printf("Error creating transfer buffer: %s\n", SDL_GetError());
        return;
    }

    void *transferMem = SDL_MapGPUTransferBuffer(window->device, e->transferBuffer, false);
    if (!transferMem) {
        printf("Error mapping transfer buffer: %s\n", SDL_GetError());
        return;
    }
    memcpy(transferMem, vertexData, vertexSize);
    memcpy((char*)transferMem + vertexSize, indices, indexSize);
    SDL_UnmapGPUTransferBuffer(window->device, e->transferBuffer);

    e->vertexTransferBufferLocation = createTransferBufferLocation(e->transferBuffer, 0);
    e->indexTransferBufferLocation  = createTransferBufferLocation(e->transferBuffer, vertexSize);

    e->vertexBufferRegion = createBufferRegion(vertexSize, e->vertexBuffer);
    e->indexBufferRegion  = createBufferRegion(indexSize,  e->indexBuffer);

    // --- Texture load + GPU texture ---
    e->surface = loadImage(fileName, 4);
    if (!e->surface) {
        printf("Failed to load texture image '%s'\n", fileName);
        return;
    }

    e->texture = createTexture(e->surface, window);
    if (!e->texture) {
        printf("Error creating GPU texture: %s\n", SDL_GetError());
        return;
    }

    // --- Texture staging + upload info ---
    const Uint32 texBytes = (Uint32)(e->surface->w * e->surface->h * 4);
    e->textureTransferBuffer = createTransferBuffer(texBytes, window);
    if (!e->textureTransferBuffer) {
        printf("Error creating texture staging buffer: %s\n", SDL_GetError());
        return;
    }

    void *texMem = SDL_MapGPUTransferBuffer(window->device, e->textureTransferBuffer, false);
    if (!texMem) {
        printf("Error mapping texture staging buffer: %s\n", SDL_GetError());
        return;
    }
    if (!e->surface->pixels) {
        printf("Loaded surface has NULL pixels\n");
        SDL_UnmapGPUTransferBuffer(window->device, e->textureTransferBuffer);
        return;
    }
    memcpy(texMem, e->surface->pixels, texBytes);
    SDL_UnmapGPUTransferBuffer(window->device, e->textureTransferBuffer);

    e->textureRegion = (SDL_GPUTextureRegion){0};
    e->textureRegion.texture = e->texture;
    e->textureRegion.w = e->surface->w;
    e->textureRegion.h = e->surface->h;
    e->textureRegion.d = 1;

    e->textureTransferInfo = (SDL_GPUTextureTransferInfo){0};
    e->textureTransferInfo.transfer_buffer = e->textureTransferBuffer;
    e->textureTransferInfo.offset = 0;

    e->textureSamplerBinding.texture = e->texture;
    e->textureSamplerBinding.sampler = window->sampler;

    // --- Perform uploads (must be inside an active copy pass) ---
    if (!window->copyPass) {
        printf("WARNING: upload called without active copy pass!\n");
    }
    uploadBuffer(&e->vertexTransferBufferLocation, &e->vertexBufferRegion, window);
    uploadBuffer(&e->indexTransferBufferLocation,  &e->indexBufferRegion,  window);
    uploadTexture(e->textureTransferInfo, e->textureRegion, window);

    // --- Bindings for draw ---
    e->vertexBufferBinding = createBufferBinding(e->vertexBuffer);
    e->indexBufferBinding  = createBufferBinding(e->indexBuffer);

    printf("Entity ready: %zu verts, %zu indices, texture %dx%d\n",
           vertices_count, indices_count, e->surface->w, e->surface->h);
}

void drawEntity(struct UBO* ubo, size_t uboSize, struct Window* window, struct Entity* e)
{
    SDL_BindGPUVertexBuffers(window->renderPass, 0, &e->vertexBufferBinding, 1);
    SDL_BindGPUIndexBuffer(window->renderPass, &e->indexBufferBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    // Push the actual UBO bytes (ubo is already a pointer)
    SDL_PushGPUVertexUniformData(window->commandBuffer, 0, ubo, (Uint32)uboSize);

    SDL_BindGPUFragmentSamplers(window->renderPass, 0, &e->textureSamplerBinding, 1);
    SDL_DrawGPUIndexedPrimitives(window->renderPass, (Uint32)e->indiciesCount, 1, 0, 0, 0);
}

VertexData* load_model(
    const char* path,
    unsigned int** out_indices,
    unsigned int* out_vertex_count,
    unsigned int* out_index_count,
    float scale // <-- new parameter
) {
    const struct aiScene* scene = aiImportFile(
        path,
        aiProcess_Triangulate | aiProcess_FlipUVs 
    );

    if (!scene) {
        printf("ERROR::ASSIMP::%s\n", aiGetErrorString());
        *out_vertex_count = 0;
        *out_index_count = 0;
        return NULL;
    }

    unsigned int totalVertices = 0;
    unsigned int totalIndices = 0;

    // Count
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        const struct aiMesh* mesh = scene->mMeshes[i];
        totalVertices += mesh->mNumVertices;
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            totalIndices += mesh->mFaces[j].mNumIndices;
        }
    }

    VertexData* vertices = malloc(sizeof(VertexData) * totalVertices);
    *out_indices = malloc(sizeof(unsigned int) * totalIndices);

    unsigned int vertexOffset = 0;
    unsigned int indexOffset = 0;

    // Fill data
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        const struct aiMesh* mesh = scene->mMeshes[i];

        // Vertices
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            vertices[vertexOffset + j].position[0] = mesh->mVertices[j].x * scale;
            vertices[vertexOffset + j].position[1] = mesh->mVertices[j].y * scale;
            vertices[vertexOffset + j].position[2] = mesh->mVertices[j].z * scale;

            if (mesh->mTextureCoords[0]) {
                vertices[vertexOffset + j].texCoords[0] = mesh->mTextureCoords[0][j].x;
                vertices[vertexOffset + j].texCoords[1] = mesh->mTextureCoords[0][j].y;
            } else {
                vertices[vertexOffset + j].texCoords[0] = 0.0f;
                vertices[vertexOffset + j].texCoords[1] = 0.0f;
            }

            vertices[vertexOffset + j].color.r = 1.0f;
            vertices[vertexOffset + j].color.g = 1.0f;
            vertices[vertexOffset + j].color.b = 1.0f;
            vertices[vertexOffset + j].color.a = 1.0f;
        }

        // Indices
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            const struct aiFace* face = &mesh->mFaces[j];
            for (unsigned int k = 0; k < face->mNumIndices; k++) {
                (*out_indices)[indexOffset++] = face->mIndices[k] + vertexOffset;
            }
        }

        vertexOffset += mesh->mNumVertices;
    }

    *out_vertex_count = totalVertices;
    *out_index_count = totalIndices;

    aiReleaseImport(scene);
    return vertices;
}

