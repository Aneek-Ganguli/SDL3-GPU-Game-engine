#include <stdio.h>
#include <cglm/cglm.h>

#include "Component.h"

Transform3D initializeTransform3D(vec3 position,vec3 scale,vec3 rotate,float rotateAngle){
    Transform3D t3D;
    t3D.rotateAngle = rotateAngle,
    glm_vec3_copy(position,t3D.position);
    glm_vec3_copy(scale,t3D.scale);
    glm_vec3_copy(rotate,t3D.rotate);
    return t3D;
}