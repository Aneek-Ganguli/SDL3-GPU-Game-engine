#include <stdio.h>
#include <cglm/cglm.h>

#include  "VertexData.h"

vec2* normalize(float x, float y){
    vec2 ndc;
    ndc[0] = (2.0f * x / 800.0f) - 1.0f;
    ndc[0] = 1.0f - (2.0f * y / 600.0f);  // flip y-axis
    return &ndc;
}