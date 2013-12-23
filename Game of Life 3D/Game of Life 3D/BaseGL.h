#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <SOIL.h>
#include <fstream>
#include <math.h>


class BaseGL {
public:
    virtual bool init() = 0;
    virtual void teardown() = 0;

    virtual void render() = 0;
    virtual void update() = 0;

    int width, height;
    virtual bool resized() = 0;
    virtual void keyboard(int key,int scancode,int action,int mods) = 0;
    
    
    GLuint loadShader(const char* fileNameVSH,const char* fileNameFSH,const char* fileNameGSH);
    void drawQuad(float x1, float y1, float x2, float y2);
    bool gluInvertMatrix(const float m[16], float invOut[16]);
    
    int main();
};