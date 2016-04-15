//
// Created by daniel on 15.04.16.
//

#ifndef PEXC1_ANIMATED_MOBILE_MESH_H
#define PEXC1_ANIMATED_MOBILE_MESH_H
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>


class Mesh {
public:
    bool loadObj(std::string filename);
    void render();
private:
    GLuint mVBO;
    GLuint mIBO;
    int mFaceCount;
};


#endif //PEXC1_ANIMATED_MOBILE_MESH_H
