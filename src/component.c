#include <stdio.h>
#include <cglm/cglm.h>

#include "Component.h"

Position3D createPosition3D(vec3 position){
    float minX,maxX,minY,maxY;
    perspective_bounds(glm_rad(fov),(float)windowWidth/windowHeight,position[2],&minX,&maxX,&minY,&maxY);
    Position3D p3D;
    glm_vec3_copy(position,p3D.position);
    if(p3D.position[0]>maxX){
        p3D.position[0] = maxX;
    }

    if(p3D.position[0]<minX){
        p3D.position[0] = minX;
    }

    if(p3D.position[1]>maxY){
        p3D.position[1] = maxY;
    }

    if(p3D.position[1]>minY){
        p3D.position[1] = minY;
    }

    printf("%f %f \n",p3D.position[0],p3D.position[1]);

    return p3D;
}