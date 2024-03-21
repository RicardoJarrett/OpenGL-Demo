#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "loader.h"

using namespace std;

bool WIREFRAME = true;
bool CUBE_TEST = false;

float rotation = 0.0f;
glm::mat4 trans = glm::mat4(1.0f);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		rotation += 12.0f;
		trans = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		rotation -= 12.0f;
		trans = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	}
}

float x_rot = 0.0f;
float y_rot = 0.0f;
float z_rot = 0.0f;

float x_spd = 0.1f;
float y_spd = 0.05f;
float z_spd = 0.03f;

void do_rotate() {
	x_rot += x_spd;
	y_rot += y_spd;
	z_rot += z_spd;
	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(x_rot), glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(y_rot), glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(z_rot), glm::vec3(0.0f, 0.0f, 1.0f));
}

int init() {
	if (!glfwInit()) {
		printf("Unable to initialse GLFW.");
		return 0;
	}
	else {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}
	return 1;
}

int create_window(GLFWwindow** win) {
	*win = glfwCreateWindow(640, 480, "Test", NULL, NULL);	//windowed
	//GLFWwindow* window = glfwCreateWindow(640, 480, "Test", glfwGetPrimaryMonitor(), NULL);	//fullscreen
	if (!*win) {
		printf("Unable to create Window.");
		return 0;
	}
	else {
		glfwMakeContextCurrent(*win);
		printf("Window created succesfully.");
		glfwSetKeyCallback(*win, key_callback);

		glewExperimental = GL_TRUE;
		glewInit();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
	return 1;
}

void create_buffer(int slot, GLuint* bufferID, float* data, int count, int length) {
	glGenBuffers(1, bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, *bufferID);
	glBufferData(GL_ARRAY_BUFFER, count * length * sizeof(float), data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, *bufferID);
	glEnableVertexAttribArray(slot);
	glVertexAttribPointer(slot, length / 3, GL_FLOAT, GL_FALSE, sizeof(float) * (length / 3), NULL);
}

void create_object(GLuint* vertexBuffer, GLuint* texBuffer, GLuint* vertexArrayObject, float* points, float* texCoords, int count) {
	glGenVertexArrays(1, vertexArrayObject);
	glBindVertexArray(*vertexArrayObject);
	
	create_buffer(0, vertexBuffer, points, count, 9);
	create_buffer(1, texBuffer, texCoords, count, 6);
	/*
	glGenBuffers(1, vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, count * 9 * sizeof(float), points, GL_STATIC_DRAW);

	glGenBuffers(1, texBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *texBuffer);
	glBufferData(GL_ARRAY_BUFFER, count * 6 * sizeof(float), texCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, *texBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
	*/
}

void create_shaders(GLuint* vs, GLuint* fs, GLuint* shader_programme, const char* vertex_shader, const char* fragment_shader) {
	*vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(*vs, 1, &vertex_shader, NULL);
	glCompileShader(*vs);

	*fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(*fs, 1, &fragment_shader, NULL);
	glCompileShader(*fs);

	*shader_programme = glCreateProgram();
	glAttachShader(*shader_programme, *fs);
	glAttachShader(*shader_programme, *vs);
	glBindAttribLocation(*shader_programme, 0, "vp");
	glBindAttribLocation(*shader_programme, 1, "texCoords");
	glLinkProgram(*shader_programme);

	GLint testVal;
	glGetShaderiv(*vs, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE)
	{
		char infolog[1024];
		glGetShaderInfoLog(*vs, 1024, NULL, infolog);
		printf("The vertex shader failed to compile with the error:\n");
		printf(infolog);
	}
	glGetShaderiv(*fs, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE)
	{
		char infolog[1024];
		glGetShaderInfoLog(*fs, 1024, NULL, infolog);
		printf("The fragment shader failed to compile with the error:\n");
		printf(infolog);
	}
}

int main() {
	if (init()){
		GLFWwindow* window;
		if(create_window(&window)){

			GLuint tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);

			int width, height, channels;
			string path = "..\\cube_col.png";
			if (!CUBE_TEST) {
				path = "..\\monkey.png";
			}

			unsigned char* image =
				stbi_load(path.c_str(), &width, &height, &channels, 0);
			cout << width << " " << height << " " << channels << "\n";

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			if (image) {
				cout << "Image created\n";
				stbi_image_free(image);

			}
			else {
				cout << "Error creating image.\n";
			}

			loader newObj;
			if (CUBE_TEST) {
				newObj.load_obj("..\\cube.obj");
			}
			else {
				newObj.load_obj("..\\monkey.obj");
			}

			GLuint vertexBuffer;
			GLuint texBuffer;
			GLuint vertexArrayObject = 0;

			create_object(&vertexBuffer, &texBuffer, &vertexArrayObject, newObj.triArray, newObj.texArray, newObj.triCount);

			const char* vertex_shader =
				"#version 330 core\n"
				"in vec3 vp;"
				"in vec2 texCoords;"
				"out vec2 v_texCoords;"
				"uniform mat4 trans;"
				"void main(){"
				"  v_texCoords = texCoords;"
				"  gl_Position = trans * vec4(vp, 1.0);"
				"}";

			const char* fragment_shader =
				"#version 330 core\n"
				"in vec2 v_texCoords;"
				"uniform sampler2D tex;"
				"out vec4 frag_colour;"
				"void main(){"
				"  frag_colour = texture(tex, v_texCoords);"
				"}";

			GLuint vs = glCreateShader(GL_VERTEX_SHADER);
			GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
			GLuint shader_programme = glCreateProgram();
			
			create_shaders(&vs, &fs, &shader_programme, vertex_shader, fragment_shader);
			glUseProgram(shader_programme);
			GLint uniTrans = glGetUniformLocation(shader_programme, "trans");
			glUniform1i(glGetUniformLocation(shader_programme, "tex"), 0);

			if (WIREFRAME) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			while (!glfwWindowShouldClose(window)) {
				do_rotate();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glBindVertexArray(vertexArrayObject);
				glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
				glDrawArrays(GL_TRIANGLES, 0, newObj.triCount * 9);

				glfwSwapBuffers(window);
				glfwPollEvents();
			}
		}
		glfwDestroyWindow(window);
	}
	glfwTerminate();
	return 0;
}