#include <stdio.h>
#include <iostream>
#include <vector>
#include <lodepng/lodepng.h>
#include <glm/gtc/matrix_transform.hpp>
#include "helpers/ProgramUtilities.h"
#include "Strip.h"

Strip::Strip() {
    perPoints = 4;
    headOffset = 0.0f;
}

Strip::~Strip() {
    stripPoints.clear();
    stripIndices.clear();
}

void Strip::init() {

    _programId = createGLProgram("./shaders/strip.vert", "./shaders/strip.frag");

    stripPoints.clear();
    for (int i = 0; i < 16; i++) {
        int channelNum = i + 1;
        double yaw = (channelNum > 8 ? (16 - channelNum) * 2 + 1.0 : (channelNum - 9) * 2 + 1.0) / 180.0f * PI_F;
        float radius = 1.0f / cos(yaw / 2.0f);
        for (int j = 0; j < 360; j += 1) {
            stripPoints.push_back(glm::vec3((float) j / 180.0f * PI_F, yaw, radius));
            if (i < 15) {
                if (i >= 8) {
                    stripIndices.push_back((i + 1) * 360 + j);

                    stripIndices.push_back(i * 360 + ((j + 1) % 360));

                    stripIndices.push_back(i * 360 + j);
                    stripIndices.push_back((i + 1) * 360 + j);

                    stripIndices.push_back((i + 1) * 360 + ((j + 1) % 360));

                    stripIndices.push_back(i * 360 + ((j + 1) % 360));
                }
                else {
                    stripIndices.push_back(i * 360 + j);
                    stripIndices.push_back(i * 360 + ((j + 1) % 360));
                    stripIndices.push_back((i + 1) * 360 + j);
                    stripIndices.push_back(i * 360 + ((j + 1) % 360));
                    stripIndices.push_back((i + 1) * 360 + ((j + 1) % 360));
                    stripIndices.push_back((i + 1) * 360 + j);
                }

            }
        }
    }
    updateBuffer();
}

void Strip::loadImg(string filename) {
    _textureStrip = loadTexture(filename, _programId, 0, "textureStrip", true);
};

void Strip::updateBuffer() {
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Upload the data to the Array buffer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * stripPoints.size() * 3, &(stripPoints[0]), GL_STATIC_DRAW);

    // Generate a vertex array (useful when we add other attributes to the geometry).
    _vao = 0;
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    // The first attribute will be the vertices positions.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // We load the indices data
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * stripIndices.size(), &(stripIndices[0]),
                 GL_STATIC_DRAW);
    glBindVertexArray(0);
    checkGLError();
}

void Strip::updatePointsWithLidar(vector <AVF_DATA::MSOP_WR> wrs) {
    vector <glm::vec3> result = LIDAR_READER::msopWrs2v(wrs);

    unsigned int mm = stripPoints.size();
    unsigned int lengthWrs = result.size();
    vector <glm::vec3> flag(stripPoints.size() / perPoints, glm::vec3(255, 255, 255));
    float distance;
    for (int i = 0; i < lengthWrs; i++) {
        int index = (static_cast<int>(result[i].x * PI_TO_ANGLE) % 360 + (i % 16) * 360) % mm;
        flag[index / perPoints].x = min(flag[index / perPoints].x, result[i].z);
        flag[index / perPoints].y++;
    }
    int perLineRange = 360 / perPoints;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < perLineRange; j++) {
            float leftDis = 0, rightDis = 0, cenDis = 0;
            int index = i * perLineRange + j;
            if (j < 3) {
                rightDis = abs(flag[index + 4].x - flag[index + 3].x) +
                           abs(flag[index + 3].x - flag[index + 2].x) +
                           abs(flag[index + 2].x - flag[index + 1].x);
                cenDis = abs(flag[index].x - flag[index + 1].x) * 3;
            }
            else if (j > perLineRange - 4) {
                leftDis = abs(flag[index - 4].x - flag[index - 3].x) +
                          abs(flag[index - 3].x - flag[index - 2].x) +
                          abs(flag[index - 2].x - flag[index - 1].x);
                cenDis = abs(flag[index].x - flag[index - 1].x) * 3;
            }
            else {
                leftDis = abs(flag[index - 4].x - flag[index - 3].x) +
                          abs(flag[index - 3].x - flag[index - 2].x) +
                          abs(flag[index - 2].x - flag[index - 1].x);
                rightDis = abs(flag[index + 4].x - flag[index + 3].x) +
                           abs(flag[index + 3].x - flag[index + 2].x) +
                           abs(flag[index + 2].x - flag[index + 1].x);
                cenDis = (abs(flag[index].x - flag[index + 1].x) +
                          abs(flag[index].x - flag[index - 1].x)) * 3.0f / 2.0f;
            }
//            cout << cenDis << "   " << leftDis << "   " << "  " << rightDis << endl;
            if (cenDis > leftDis && cenDis > rightDis) {
                if (j < 3)
                    flag[index].x = flag[index + 1].x;
                else if (j > perLineRange - 4)
                    flag[index].x = flag[index - 1].x;
                else
                    flag[index].x = (flag[index - 1].x + flag[index + 1].x) / 2.0f;
            }
            for (int i = 0; i < perPoints; i++) {
                stripPoints[index*perPoints + i].z = flag[index].x;
            }
        }
    }
    updateBuffer();
}

void Strip::draw(float elapsed, const glm::mat4 &view, const glm::mat4 &projection, size_t pingpong) {
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(0.1f), glm::vec3(0.0f, -0.0f, -0.0f)), glm::vec3(1.0f));
    // Combine the three matrices.
    glm::mat4 MV = view * model;
    glm::mat4 MVP = projection * MV;
    // Compute the normal matrix
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(MV)));
    // Compute the inverse view matrix
    glm::mat4 invView = glm::inverse(view);

    glUseProgram(_programId);

    // Select the right sub-uniform buffer to use for the light.
    glUniformBlockBinding(_programId, _lightUniformId, pingpong);
    // Upload the MVP matrix.
    GLuint mvpID = glGetUniformLocation(_programId, "mvp");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);
    // Upload the MV matrix.
    GLuint mvID = glGetUniformLocation(_programId, "mv");
    glUniformMatrix4fv(mvID, 1, GL_FALSE, &MV[0][0]);
    // Upload the normal matrix.
    GLuint normalMatrixID = glGetUniformLocation(_programId, "normalMatrix");
    glUniformMatrix3fv(normalMatrixID, 1, GL_FALSE, &normalMatrix[0][0]);
    // Upload the inverse view matrix.
    GLuint invVID = glGetUniformLocation(_programId, "inverseV");
    glUniformMatrix4fv(invVID, 1, GL_FALSE, &invView[0][0]);

    // Bind the textures.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureStrip);


    // Upload the light MVP matrix.
    GLuint lmvpID = glGetUniformLocation(_programId, "lightMVP");
    glUniformMatrix4fv(lmvpID, 1, GL_FALSE, &_lightMVP[0][0]);

    GLuint inIsPolarVID = glGetUniformLocation(_programId, "isPolar");
    glUniform1f(inIsPolarVID, 1);

    GLuint headOffsetVID = glGetUniformLocation(_programId, "headOffset");
    glUniform1f(headOffsetVID, headOffset);

    // Select the geometry.
    glBindVertexArray(_vao);
    // Draw!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glDrawElements(GL_TRIANGLES, stripIndices.size(), GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void Strip::clean() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteTextures(1, &_textureStrip);
}


