//
// Created by daniel on 15.04.16.
//

#ifndef PEXC1_ANIMATED_MOBILE_INSTANCE_H
#define PEXC1_ANIMATED_MOBILE_INSTANCE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "Mesh.h"


#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <vector>

class Instance {
public:
    Instance(Mesh mesh);
    void updateModelMatrix();
    void rotate(glm::vec3 axis, float angle);
    void setPosition(glm::vec3 position);
    void addChild(Instance* child);
    void render();

    const glm::mat4 &getModelMatrix();

private:
    glm::mat4 mModelMatrix;
    glm::mat4 mRotation;
    glm::vec3 mPosition;
    //glm::quat mOrientation;
    Instance* mParent;
    std::vector<Instance*> mChildren;

    Mesh mMesh;
};


#endif //PEXC1_ANIMATED_MOBILE_INSTANCE_H
