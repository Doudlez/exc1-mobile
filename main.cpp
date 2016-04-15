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

extern "C" {
    #include "source/LoadShader.h"
    #include "source/OBJParser.h"
}

/* Define handles to two vertex buffer objects */
GLuint VBO1, VBO2;

/* Define handles to two index buffer objects */
GLuint IBO1, IBO2;


/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

glm::mat4 ProjectionMatrix; /* Perspective projection matrix */
glm::mat4 ViewMatrix;       /* Camera view matrix */
glm::mat4 ModelMatrix;

/* Transformation matrices for model positioning */
glm::mat4 TranslateOrigin;
glm::mat4 TranslateDown;
glm::mat4 RotateX;
glm::mat4 RotateZ;
glm::mat4 InitialTransform;

/* Arrays for holding vertex data of the two models */
GLfloat *vertex_buffer_data1, *vertex_buffer_data2;

/* Arrays for holding indices of the two models */
GLushort *index_buffer_data1,  *index_buffer_data2;

/* Structures for loading of OBJ data */
obj_scene_data data1, data2;

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

    glEnableVertexAttribArray(0);

    /* Bind buffer with vertex data of currently active object */
   /* if (model == Model1)
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    else if (model == Model2)
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);*/

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // TODO: Move to SetupDataBuffer?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
    /* Bind buffer with index data of currently active object */
    /*if (model == Model1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
    else if (model == Model2)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);*/

    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

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
    glUniformMatrix4fv(RotationUniform, 1, GL_FALSE, glm::value_ptr(ModelMatrix));

    /* Set state to only draw wireframe (no lighting used, yet) */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    //glDrawElements(GL_TRIANGLES, data1.face_count, GL_UNSIGNED_SHORT, 0);

    /* Disable attributes */
    glDisableVertexAttribArray(0);

    /* Swap between front and back buffer */
    glutSwapBuffers();
}

void OnIdle()
{
    float angle = fmod((glutGet(GLUT_ELAPSED_TIME) / 1000.0), 360.0);

    /* Time dependent rotation matrix */
    glm::mat4 RotationMatrixAnim =
            glm::rotate(glm::mat4(1.0f),              /* Output matrix */
                        angle,                        /* Rotation angle */
                        glm::vec3(0.0f, 1.0f, 0.0f)); /* Rotation axis*/

    /* Apply model rotation; finally move cube down */
    //ModelMatrix = TranslateDown * RotationMatrixAnim * InitialTransform;
    //glm::mat4 Translation = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0 , 0));
    //glm::mat4 ViewRotation = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f));
    //ViewMatrix = glm::inverse(Translation*ViewRotation);

    /* Set up single transformation matrix for complete transformation
       from model to screen space */
    //PVMMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

    //ModelMatrix = glm::rotate(ModelMatrix, angle, glm::vec3(0,0,0));

    /*Set camera position */
    ViewMatrix = glm::lookAt(glm::vec3(x, 1.0f, z),
                             glm::vec3(x+lx, 1.f + ly, z+lz),
                             glm::vec3( 0.f, 5.f, 0.f));

    /* Apply model rotation; finally move cube down */
    ModelMatrix = RotationMatrixAnim;

    /* Request redrawing of window content */
    glutPostRedisplay();
}

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType)
{
    /* Create shader object */
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0)
    {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    /* Associate shader source code string with shader object */
    glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

    GLint success = 0;
    GLchar InfoLog[1024];

    /* Compile shader source code */
    glCompileShader(ShaderObj);
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

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


void SetupDataBuffers() {
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, data1.vertex_count*3*sizeof(GLfloat), vertex_buffer_data1, GL_STATIC_DRAW);

    //glGenBuffers(1, &VBO2);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    //glBufferData(GL_ARRAY_BUFFER, data2.vertex_count*3*sizeof(GLfloat), vertex_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data1.face_count*3*sizeof(GLushort), index_buffer_data1, GL_STATIC_DRAW);

    //glGenBuffers(1, &IBO2);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, data2.face_count*3*sizeof(GLushort), index_buffer_data2, GL_STATIC_DRAW);
}

void Initialize()
{
    int i;
    int success;

    /* Load first OBJ model */
    std::string filename1 = "models/suzanne.obj";
    success = parse_obj_scene(&data1, const_cast<char*>(filename1.c_str()));

    if(!success)
        printf("Could not load file. Exiting.\n");

    /* Load second OBJ model */
    //char* filename2 = "models/suzanne.obj";
    //success = parse_obj_scene(&data2, filename2);


    /*  Copy mesh data from structs into appropriate arrays */
    int vert = data1.vertex_count;
    int indx = data1.face_count;

    vertex_buffer_data1 = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    index_buffer_data1 = (GLushort*) calloc (indx*3, sizeof(GLushort));

    /* Vertices */
    for(i=0; i<vert; i++) {
        vertex_buffer_data1[i*3] = (GLfloat)(*data1.vertex_list[i]).e[0];
        vertex_buffer_data1[i*3+1] = (GLfloat)(*data1.vertex_list[i]).e[1];
        vertex_buffer_data1[i*3+2] = (GLfloat)(*data1.vertex_list[i]).e[2];
    }

    /* Indices */
    for(i=0; i<indx; i++) {
        index_buffer_data1[i*3] = (GLushort)(*data1.face_list[i]).vertex_index[0];
        index_buffer_data1[i*3+1] = (GLushort)(*data1.face_list[i]).vertex_index[1];
        index_buffer_data1[i*3+2] = (GLushort)(*data1.face_list[i]).vertex_index[2];
    }

   /* vert = data2.vertex_count;
    indx = data2.face_count;

    vertex_buffer_data2 = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    index_buffer_data2 = (GLushort*) calloc (indx*3, sizeof(GLushort));


    for(i=0; i<vert; i++)
    {
        vertex_buffer_data2[i*3] = (GLfloat)(*data2.vertex_list[i]).e[0];
        vertex_buffer_data2[i*3+1] = (GLfloat)(*data2.vertex_list[i]).e[1];
        vertex_buffer_data2[i*3+2] = (GLfloat)(*data2.vertex_list[i]).e[2];
    }


    for(i=0; i<indx; i++)
    {
        index_buffer_data2[i*3] = (GLushort)(*data2.face_list[i]).vertex_index[0];
        index_buffer_data2[i*3+1] = (GLushort)(*data2.face_list[i]).vertex_index[1];
        index_buffer_data2[i*3+2] = (GLushort)(*data2.face_list[i]).vertex_index[2];
    }*/

    /* Set background (clear) color to blue */
    glClearColor(0.0, 0.0, 0.4, 0.0);
    glDisable(GL_CULL_FACE);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Setup vertex, color, and index buffer objects */
    SetupDataBuffers();

    /* Setup shaders and shader program */
    CreateShaderProgram();

    /* Initialize matrices */
    ProjectionMatrix = glm::mat4(1.f);
    ViewMatrix = glm::mat4(1.f);
    ModelMatrix = glm::mat4(1.f);


    /* Set projection transform */
    float fovy = 60.0*M_PI/180.0;
    float aspect = 1.0;
    float nearPlane = 0.1;
    float farPlane = 1000.0;
    ProjectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);

    //ViewMatrix = glm::lookAt(glm::vec3(0,0,100),    /* Eye vector */
      //                       glm::vec3(0,0,0),     /* Viewing center */
        //                     glm::vec3(0,1,0) );  /* Up vector */

    //TranslateDown = glm::translate(glm::mat4(1.0f),
                                   //glm::vec3(0.0f, -sqrtf(sqrtf(2.0) * 1.0), 0.0f));

    //InitialTransform = glm::translate(InitialTransform,
      //                                glm::vec3(1.0f, 1.0f, 1.0f));



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
    return 0;
}