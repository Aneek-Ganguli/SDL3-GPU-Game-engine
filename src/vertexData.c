#include <stdio.h>
#include <cglm/cglm.h>

#include  "VertexData.h"

vec2* normalize(float x, float y){
    vec2 ndc;
    ndc[0] = (2.0f * x / 800.0f) - 1.0f;
    ndc[0] = 1.0f - (2.0f * y / 600.0f);  // flip y-axis
    return &ndc;
}

void perspective_bounds(float fovy, float aspect, float z,
                        float *xmin, float *xmax,
                        float *ymin, float *ymax) {
    float h = 2.0f * z * tanf(fovy * 0.5f);
    float w = h * aspect;
    *xmin = -w / 2.0f;
    *xmax =  w / 2.0f;
    *ymin = -h / 2.0f;
    *ymax =  h / 2.0f;
}