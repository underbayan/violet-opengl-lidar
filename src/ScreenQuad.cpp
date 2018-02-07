#include <stdio.h>
#include <iostream>
#include <vector>

#include "helpers/ProgramUtilities.h"

#include "ScreenQuad.h"

ScreenQuad::ScreenQuad(){}

ScreenQuad::~ScreenQuad(){}

void ScreenQuad::init(GLuint textureId, const std::string & shaderRoot){
	
	// Load the shaders
	_programId = createGLProgram(shaderRoot + ".vert", shaderRoot + ".frag");

	// Load geometry.
	std::vector<float> quadVertices{ -1.0, -1.0,  0.0,
		1.0, -1.0,  0.0,
		-1.0,  1.0,  0.0,
		1.0,  1.0,  0.0
	};

	// Array to store the indices of the vertices to use.
	std::vector<unsigned int> quadIndices{0, 1, 2, 2, 1, 3};
	
	_count = quadIndices.size();
	// Create an array buffer to host the geometry data.
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Upload the data to the Array buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * quadVertices.size(), &(quadVertices[0]), GL_STATIC_DRAW);

	// Generate a vertex array (useful when we add other attributes to the geometry).
	_vao = 0;
	glGenVertexArrays (1, &_vao);
	glBindVertexArray(_vao);
	// The first attribute will be the vertices positions.
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	// We load the indices data
	glGenBuffers(1, &_ebo);
 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quadIndices.size(), &(quadIndices[0]), GL_STATIC_DRAW);

	glBindVertexArray(0);
	
	// Link the texture of the framebuffer for this program.
	_textureId = textureId;
	glBindTexture(GL_TEXTURE_2D, _textureId);
	GLuint texUniID = glGetUniformLocation(_programId, "screenTexture");
	glUniform1i(texUniID, 0);
	checkGLError();
	
	
}


void ScreenQuad::draw(glm::vec2 invScreenSize){
	
	// Select the program (and shaders).
	glUseProgram(_programId);
	
	// Inverse screen size uniform.
	GLuint screenId = glGetUniformLocation(_programId, "inverseScreenSize");
	glUniform2fv(screenId, 1, &(invScreenSize[0]));
	
	// Active screen texture.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	
	// Select the geometry.
	glBindVertexArray(_vao);
	// Draw!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glDrawElements(GL_TRIANGLES, _count, GL_UNSIGNED_INT, (void*)0);
	
	glBindVertexArray(0);
	glUseProgram(0);
}





void ScreenQuad::clean(){
	glDeleteVertexArrays(1, &_vao);
}


