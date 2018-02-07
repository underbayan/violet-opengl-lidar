//
//  created by laina on 2017/9/8.
//

#ifndef GL_STRIP_H
#define GL_STRIP_H

#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "common/common.h"
#include "./lidar/LidarReader.h"

class Strip {

public:
    unsigned  int perPoints;
    float headOffset;
    Strip();

    ~Strip();

    /// Init function
    void init();
    void loadImg(string filename);

    /// Draw function
    void draw(float elapsed, const glm::mat4 &view, const glm::mat4 &projection, size_t pingpong);
    /// Clean function
    void clean();
    void updateBuffer();
    std::vector<glm::vec3> stripPoints;
    std::vector<unsigned int> stripIndices;
    void updatePointsWithLidar(vector<AVF_DATA::MSOP_WR> wrs);
private:

    GLuint _programId;
    GLuint _vao;
    GLuint _ebo;
    GLuint _textureStrip;
    size_t _count;
    glm::mat4 _lightMVP;
    GLuint _lightUniformId;

};

#endif
