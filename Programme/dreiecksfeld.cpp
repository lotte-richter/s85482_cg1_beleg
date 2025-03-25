//dreiecksfeld.cpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <glew.h>
#include <freeglut.h>
#include <FreeImage.h>
#include <windows.h>

GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath,
	const char* geometryFilePath, const char* tessconstrolFilePath,
	const char* tessevaluationFilePath, const char* computeFilePath);
int width, height;
const int Spalten = 10;
const int Zeilen = 10;
enum VAO_IDs{Triangles,NumVAOs};
enum Attrib_IDs{vPosition=0};
enum Buffer_IDs{ArrayBuffer,NumBuffers};
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint program;
GLfloat vertices[Spalten][Zeilen][3][2];
GLfloat posx = 0.0, posy = 0.0;
GLint mposx, mposy;

void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	program = loadShaders("Programme/Dreiecksfeld.vs", "Programme/Dreiecke.fs", "", "", "", "");
	glUseProgram(program);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(vPosition);
}

void display(void)
{
	if (width < height) glViewport(0, height, width, width);
	else glViewport((width - height) / 2, 0, height, height);
	glViewport(100, 100, 300, 300);
	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	int i, j;
	for (i = 0; i < Spalten; i++){
		for (j = 0; j < Zeilen; j++) {
			vertices[i][j][0][0] = i * 0.1 - 0.5 - 0.04;
			vertices[i][j][0][1] = j * 0.1 - 0.5 - 0.04;
			vertices[i][j][1][0] = i * 0.1 - 0.5 - 0.04;
			vertices[i][j][1][1] = j * 0.1 - 0.5 - 0.04;
		}
	}
}

void reshape(int w, int h) {
	width = w;
	height = h;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv); //Initialisierung von HLUT mit Argumentübergabe
	glutInitDisplayMode(GLUT_RGBA); //Initialisieren von Anzeigepuffern der Grafikkarte
	glutInitWindowSize(512, 512);
	glutInitContextVersion(4, 5);  // (4,2) (3,3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	//GLUT_COMPATIBILITY_PROFILE
	glutCreateWindow(argv[0]); //Erzeugen eines OpenGL-fähigen Fensters
	glewExperimental = GL_TRUE;
	if (glewInit()) printf("Error");
	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMainLoop();
}