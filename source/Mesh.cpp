//
// Created by daniel on 15.04.16.
//

#include "Mesh.h"
extern "C"{
    #include "OBJParser.h"
}



bool Mesh::loadObj(std::string filename) {

    int success;
    obj_scene_data data;


    success = parse_obj_scene(&data, const_cast<char*>(filename.c_str()));
    if(!success){
        return false;
    }

    GLfloat* vertex_buffer_data;
    GLushort* index_buffer_data;

    /*  Copy mesh data from structs into appropriate arrays */
    int vert = data.vertex_count;
    mFaceCount = data.face_count;

    vertex_buffer_data = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    index_buffer_data = (GLushort*) calloc (mFaceCount*3, sizeof(GLushort));

    /* Vertices */
    for(int i=0; i<vert; i++) {
        vertex_buffer_data[i*3] = (GLfloat)(*data.vertex_list[i]).e[0];
        vertex_buffer_data[i*3+1] = (GLfloat)(*data.vertex_list[i]).e[1];
        vertex_buffer_data[i*3+2] = (GLfloat)(*data.vertex_list[i]).e[2];
    }

    /* Indices */
    for(int i=0; i<mFaceCount; i++) {
        index_buffer_data[i*3] = (GLushort)(*data.face_list[i]).vertex_index[0];
        index_buffer_data[i*3+1] = (GLushort)(*data.face_list[i]).vertex_index[1];
        index_buffer_data[i*3+2] = (GLushort)(*data.face_list[i]).vertex_index[2];
    }

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, data.vertex_count*3*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW);


    glGenBuffers(1, &mIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.face_count*3*sizeof(GLushort), index_buffer_data, GL_STATIC_DRAW);

    free(vertex_buffer_data);
    free(index_buffer_data);

    return true;
}

void Mesh::render() {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // TODO: Move to SetupDataBuffer?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);

    // glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLES, mFaceCount*3, GL_UNSIGNED_SHORT, 0);
    glDisableVertexAttribArray(0);

}



