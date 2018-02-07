#include "LidarPoints.h"

LidarPoints::LidarPoints() {

}

LidarPoints::~LidarPoints() {
    _mesh.clear();
    _flag.clear();
    glDeleteTextures(1, &_texColor);
    glDeleteVertexArrays(1, &_vao);
    glDeleteProgram(_programId);
}

void LidarPoints::initProgram() {
    _programId = createGLProgram("./shaders/lidar.vert", "./shaders/lidar.frag");

}

void LidarPoints::updateGLBuffer() {
    if (_mesh.positions.size() < 1)return;
    _vao = 0;
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    // The first attribute will be the vertices positions.
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _mesh.positions.size() * 3, &(_mesh.positions[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

//    glEnableVertexAttribArray(2);
//    glBindBuffer(GL_ARRAY_BUFFER, color_shader);
//    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 0, NULL);

    // We load the indices data
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _mesh.indices.size(), &(_mesh.indices[0]),
                 GL_STATIC_DRAW);
    glBindVertexArray(0);

    checkGLError();
}

void LidarPoints::loadImg(string filename) {
    _texColor = loadTexture(filename, _programId, 0, "textureColor", true);
}

void LidarPoints::initWithMSOP_WRS(const vector <AVF_DATA::MSOP_WR> &mws) {
    initProgram();
    _mesh.clear();
    _mesh = LIDAR_READER::msopWrapperVector2Mesh(mws);
};

void LidarPoints::appendWithMSOP_WRS(const vector <AVF_DATA::MSOP_WR> &mws) {
    LIDAR_READER::msopWrapperVectorAppend2Mesh(mws, _mesh);
};

void LidarPoints::draw(float elapsed, const glm::mat4 &view, const glm::mat4 &projection, size_t pingpong) {
    if (_mesh.positions.size() < 1)return;
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.0f, -0.0f)), glm::vec3(1.0f));
    // Combine the three matrices.
    glm::mat4 MV = view * model;
    glm::mat4 MVP = projection * MV;

    // Compute the normal matrix
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(MV)));
    // Compute the inverse view matrix
    glm::mat4 invView = glm::inverse(view);
    // Set the clear color to white.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Select the program (and shaders).
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


    // Upload the light MVP matrix.
    GLuint lmvpID = glGetUniformLocation(_programId, "lightMVP");
    glUniformMatrix4fv(lmvpID, 1, GL_FALSE, &_lightMVP[0][0]);
    // Bind the textures.
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, _texColor);
    // Bind the shadowMap texture.
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _shadowMapId);

    // Select the geometry.
    glBindVertexArray(_vao);
    // Draw!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
//    glDrawElements(GL_TRIANGLES, _mesh.indices.size(), GL_UNSIGNED_INT, (void *) 0);
    glDrawArrays(GL_POINTS, 0, _mesh.positions.size());
    glBindVertexArray(0);
    glUseProgram(0);
}


void LidarPoints::clean() {

}


