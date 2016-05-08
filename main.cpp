#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GLM_FORCE_RADIANS

#include <glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "source/Mesh.h"
#include "source/Instance.h"

extern "C" {
    #include "source/LoadShader.h"
}


/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

std::vector<Instance*> instances;

glm::mat4 ProjectionMatrix; /* Perspective projection matrix */
glm::mat4 ViewMatrix;       /* Camera view matrix */
glm::mat4 ModelMatrix;

/* Model for camera movement */
float cameraAngle = 0.f;
float lx =0.f, lz = -1.f, ly = 0.f;
float x=0.f, z = 5.f;


/*void Mouse(int button, int state, int x, int y){
    if(state == GLUT_DOWN){

        switch(button)
        {
            case GLUT_LEFT_BUTTON:
                lz
                break;

            case GLUT_MIDDLE_BUTTON:
                axis = Yaxis;
                break;

            case GLUT_RIGHT_BUTTON:
                axis = Zaxis;
                break;
        }
        anim = GL_TRUE;
    }
}*/

void Keyboard(unsigned char key, int xx, int yy) {
    float fraction = 1.0f;
    switch( key ) {
        /* Activate model one or two */
        case 'w':
            x += lx * fraction;
            z += lz * fraction;
            break;

        case 'a':
            cameraAngle -= 0.1f;
            lx = sin(cameraAngle);
            lz = -cos(cameraAngle);
            break;
        case 's':
            x -= lx * fraction;
            z -= lz * fraction;
            break;
        case 'd':
            cameraAngle += 0.1f;
            lx = sin(cameraAngle);
            lz = -cos(cameraAngle);
            break;
        case 'q':
            cameraAngle += 0.1f;
            ly = sin(cameraAngle);
            break;
        case 'e':
            cameraAngle -= 0.1f;
            ly = sin(cameraAngle);
            break;
    }
    glutPostRedisplay();
}

void Keyboard2(int key, int xx, int yy){
    float fraction = 1.0f;
    switch(key){
        case GLUT_KEY_LEFT:
            cameraAngle -= 0.1f;
            lx = sin(cameraAngle);
            lz = -cos(cameraAngle);
            break;
        case GLUT_KEY_RIGHT:
            cameraAngle += 0.1f;
            lx = sin(cameraAngle);
            lz = -cos(cameraAngle);
            break;
        case GLUT_KEY_UP:
            x += lx * fraction;
            z += lz * fraction;
            break;
        case GLUT_KEY_DOWN:
            x -= lx * fraction;
            z -= lz * fraction;
            break;
    }
    glutPostRedisplay();
}

void Display() {
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    /* Associate program with uniform shader matrices */
    GLint projectionUniform = glGetUniformLocation(ShaderProgram, "ProjectionMatrix");
    if (projectionUniform == -1) {
        fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

    GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
    if (ViewUniform == -1) {
        fprintf(stderr, "Could not bind uniform ViewMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, glm::value_ptr(ViewMatrix));

    GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (RotationUniform == -1) {
        fprintf(stderr, "Could not bind uniform ModelMatrix\n");
        exit(-1);
    }

    for(auto& inst : instances){
        glUniformMatrix4fv(RotationUniform, 1, GL_FALSE, glm::value_ptr(inst->getModelMatrix()));
        inst->render();
    }


    /* Set state to only draw wireframe (no lighting used, yet) */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    /* Swap between front and back buffer */
    glutSwapBuffers();
}

void OnIdle()
{
    //float angle = fmod((glutGet(GLUT_ELAPSED_TIME) / 1000.0), 360.0);
    float angle = 2*M_PI/200;
    float angle2 = 2* M_PI/300;
    float angle3 = 2*M_PI/500;
    instances[0]->rotate(glm::vec3(0,1.f,0), angle);
    instances[4]->rotate(glm::vec3(0,1.f,0), angle2);
    instances[1]->rotate(glm::vec3(0,1.f,0), angle2);
    instances[2]->rotate(glm::vec3(0,1.f,0), angle3);
    instances[3]->rotate(glm::vec3(0,1.f,0), angle3);
    for(auto& inst:instances){
        inst->updateModelMatrix();
    }

    /*Set camera position */
    ViewMatrix = glm::lookAt(glm::vec3(x, 1.0f, z),                      //eye  position of camera viewpoint
                             glm::vec3(x+lx, 1.f + ly, z+lz),            //center where u are looking at
                             glm::vec3( 0.f, 5.f, 0.f));                 //up

    /* Request redrawing of window content */
    glutPostRedisplay();
}

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType)
{
    /* Create shader object */
    GLuint ShaderObj = glCreateShader(ShaderType);
    GLuint ShaderObj2 = glCreateShader(ShaderType);

    if (ShaderObj == 0)
    {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    /* Associate shader source code string with shader object */
    glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

    GLint success = 0;
    GLint success2 = 1;
    GLchar InfoLog[1024];

    /* Compile shader source code */
    glCompileShader(ShaderObj);
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    glCompileShader(ShaderObj2);
    glGetShaderiv(ShaderObj2, GL_COMPILE_STATUS, &success2);

    if (!success)
    {
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    /* Associate shader with shader program */
    glAttachShader(ShaderProgram, ShaderObj);
}

void CreateShaderProgram()
{
    /* Allocate shader object */
    ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Load shader code from file */
    VertexShaderString = LoadShader("shaders/vertexshader.vs");
    FragmentShaderString = LoadShader("shaders/fragmentshader.fs");

    /* Separately add vertex and fragment shader to program */
    AddShader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);

    /* Check results of linking step */
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

    if (Success == 0)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (!Success)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Put linked shader program into drawing pipeline */
    glUseProgram(ShaderProgram);
}


void Initialize() {
    Mesh mesh1, mesh2, mesh3, mesh4, mesh5;
    if(!mesh1.loadObj("models/teapot.obj")){
        printf("Could not load file. Exiting.\n");
        exit(0);
    }

   if(!mesh2.loadObj("models/teapot.obj")){
        printf("Could not load file 1. Exiting.\n");
        exit(0);
    }

    if(!mesh3.loadObj("models/suzanne.obj")){
        printf("Could not load file 2. Exiting.\n");
        exit(0);
    }

    if(!mesh4.loadObj("models/suzanne.obj")){
        printf("Could not load file. Exiting.\n");
        exit(0);
    }

    if(!mesh5.loadObj("models/suzanne.obj")){
        printf("Could not load file. Exiting.\n");
        exit(0);
    }

    Instance* inst1 = new Instance(mesh1);
    Instance* inst2 = new Instance(mesh2);
    Instance* inst3 = new Instance(mesh3);
    Instance* inst4 = new Instance(mesh4);
    Instance* inst5 = new Instance(mesh5);

    inst1->setPosition(glm::vec3(0,0,0));
    inst2->setPosition(glm::vec3(-10, -5, 0));
    inst3->setPosition(glm::vec3(-4, -7, 0));
    inst4->setPosition(glm::vec3(4, -7, 0));
    inst5->setPosition(glm::vec3(5,-3,0));
    //inst1.rotate()
    instances.push_back(inst1);
    instances.push_back(inst2);
    instances.push_back(inst3);
    instances.push_back(inst4);
    instances.push_back(inst5);
    inst1->addChild(inst2);
    inst1->addChild(inst5);
    inst2->addChild(inst3);
    inst2->addChild(inst4);



    /* Set background (clear) color to blue */
    glClearColor(0.0, 0.0, 0.4, 0.0);
    glDisable(GL_CULL_FACE);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    /* Setup shaders and shader program */
    CreateShaderProgram();

    /* Initialize matrices */
    ProjectionMatrix = glm::mat4(1.f);
    ViewMatrix = glm::mat4(1.f);


    /* Set projection transform */
    float fovy = 60.0*M_PI/180.0;
    float aspect = 1.0;
    float nearPlane = 0.1;
    float farPlane = 1000.0;
    ProjectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);

    ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.f), glm::vec3(0.f,0.f,10.f)));
    //ViewMatrix = glm::mat4(1.f);

}


int main(int argc, char** argv ) {
    /* Initialize GLUT; set double buffered window and RGBA color model */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 400);
    glutCreateWindow("CG Proseminar - Mobile");

    /* Initialize GL extension wrangler */
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    /* Setup scene and rendering parameters */
    Initialize();


    /* Specify callback functions;enter GLUT event processing loop,
     * handing control over to GLUT */
    glutIdleFunc(OnIdle);
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Keyboard2);
    //glutMouseFunc(Mouse);

    glEnable(GL_DEPTH_TEST);
    glutMainLoop();

    /* ISO C requires main to return int */
    for(auto inst:instances){
        delete inst;
    }
    return 0;
}
