//
// Created by daniel on 15.04.16.
//

#include "Instance.h"
#include "Mesh.h"


void Instance::setPosition(glm::vec3 position) {
    mPosition = position;
}

void Instance::updateModelMatrix(){
    mModelMatrix = glm::translate(glm::mat4(1.0f), mPosition) * mRotation;
    if(mParent != NULL){
        mModelMatrix =  mParent->mModelMatrix * mModelMatrix;
    }
    for(auto child:mChildren){
        child->updateModelMatrix();
    }
}

void Instance::addChild(Instance *child) {
    child->mParent = this;
    mChildren.push_back(child);
}

void Instance::rotate(glm::vec3 axis, float angle) {
    mRotation = glm::rotate(mRotation, angle, axis);
}

const glm::mat4 &Instance::getModelMatrix() {
    return mModelMatrix;
}

void Instance::render() {
    mMesh.render();
}

Instance::Instance(Mesh mesh) {
    mMesh = mesh;
}





