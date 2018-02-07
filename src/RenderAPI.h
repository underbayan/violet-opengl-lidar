//
//  created by laina on 2017/8/29.
//


#ifndef GL_RENDERAPI_H
#define GL_RENDERAPI_H

#include <GL/glew.h> // to load OpenGL extensions at runtime
#include <GLFW/glfw3.h> // to set up the OpenGL context and manage window lifecycle and inputs

#include <stdio.h>
#include <iostream>
#include <memory>
#include "helpers/ProgramUtilities.h"

#include "LidarRender.h"

#define INITIAL_SIZE_WIDTH 800
#define INITIAL_SIZE_HEIGHT 600
namespace RENDER_API {
    void destoryGlfw();

    void initGlfw();

    void generatePoints( vector <AVF_DATA::MSOP_WR> &mws);

    void generatePoints( vector<char> buffer);

    void generatePoints(string fileName, uint32_t startPackets = 0, uint32_t numPacks = 4);

    void addPoints(const vector <AVF_DATA::MSOP_WR> &mws);

    void addPoints(string fileName, uint32_t startPackets = 0, uint32_t numPacks = 4);

    void addPoints( vector<char> buffer);

    void loadImg(const string filename);

    void updateGLBuffer() ;
    void commandLine(unsigned int key);
    void clearScreen();
}

#endif //GL_RENDERAPI_H
