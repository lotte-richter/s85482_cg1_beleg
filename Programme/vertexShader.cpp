#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
//#include <vector>
#include <glew.h>


GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath) {
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);//Erzeugung eines Shader-Objectes
	GLuint program;
	GLint compiled, linked;
	const GLchar *shaderSrc = { " #version 450 compatibility"
		"void main()" "{gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;}" };

	glShaderSource(shader, 1, &shaderSrc, NULL);//Zuweisung von Quelltext zu Shader-Objekt
	glCompileShader(shader);//Übersetzung von Quelltext in Shader-Objekt
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);//Shader-Objekt Informationen
	if (!compiled)
	{
		GLint length;
		GLchar* log;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);//Shader-Objekt Informationen
		log = (GLchar*)malloc(length);
		glGetShaderInfoLog(shader, length, &length, log);//Übersetzungsinformationen zu Shader-Objekt
		fprintf(stderr, "%s", log);
	}
	program = glCreateProgram();//Erzeugung eines Shader-Programms
	glAttachShader(program, shader);//Zuweisung Shader-Objekt zu Shader-Programm
	glLinkProgram(program);//Verbindung eines Shader-Programms
	glGetProgramiv(program, GL_LINK_STATUS, &linked);//Shader-Programm Informationen
	if (linked)
		glUseProgram(program);//Verwendung eines Shader-Programms
	else fprintf(stderr, "Linker Error");

	//Shader-Laden:
	std::string shaderCode;
	std::ifstream shaderStream(fragmentFilePath, std::ios::in);
	if (shaderStream.is_open())
	{
		std::string line = "";
		while (std::getline(shaderStream, line))
			shaderCode += "\n" + line;
		shaderStream.close();
	}
	else printf("Open source failed\n");
}

