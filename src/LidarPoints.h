#ifndef GL_LIDARPOINTS_H
#define GL_LIDARPOINTS_H

#include <iostream>
#include <lodepng/lodepng.h>
#include <numeric>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include "helpers/ProgramUtilities.h"
#include "helpers/MeshUtilities.h"
#include "./lidar/LidarReader.h"

using namespace std;

class LidarPoints {
public:
    LidarPoints();

    ~LidarPoints();

    void initProgram();

    void initWithMSOP_WRS(const vector <AVF_DATA::MSOP_WR> &mws);

    void appendWithMSOP_WRS(const vector <AVF_DATA::MSOP_WR> &mws);

    void draw(float elapsed, const glm::mat4 &view, const glm::mat4 &projection, const size_t pingpong);

    void simplifyPoints();

    void updateGLBuffer();

    void loadImg(string filename);

    void clean();


private:

    mesh_t _mesh;
    GLuint _programId;
    GLuint _vao;
    GLuint _ebo;
    GLuint _texColor;
    GLuint _texNormal;
    GLuint _texEffects;
    GLuint _lightUniformId;
    GLuint _shadowMapId;
    size_t _count;
    GLuint _texCubeMap;
    glm::mat4 _lightMVP;
    vector<int> _flag;

};


#endif //GL_LIDARPOINTS_H
