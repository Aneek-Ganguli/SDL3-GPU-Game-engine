#include <stdio.h>
#include <math.h>

#include  "VertexData.h"

struct Vec2 normalize(float x, float y){
    struct Vec2 ndc;
    ndc.x = (2.0f * x / 800.0f) - 1.0f;
    ndc.y = 1.0f - (2.0f * y / 600.0f);  // flip y-axis
    return ndc;
}