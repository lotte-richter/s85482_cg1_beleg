#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <glew.h>
#include <freeglut.h>
#include <FreeImage.h>

GLuint loadShaders(const char* vertexFilePath,
	const char* fragmentFilePath,
	const char* geometryFilePath,
	const char* tesscontrolFilePath,
	const char* tessevaluationFilePath,
	const char* computeFilePath);
GLint height = 100, width = 100;
enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition, vColor };
GLuint vao[NumVAOs];
GLuint ebo[NumBuffers];
GLuint vbo[NumBuffers];
const GLuint NumVertices = 6; //Anz zu zeichnenede Punkte

void init(void) {
	static const GLfloat cube_positions[] = {
		-1.0,-1.0,-1.0,1.0, -1.0,-1.0,1.0,1.0, -1.0,1.0,-1.0,1.0,
		-1.0,1.0,1.0,1.0, 1.0,-1.0,-1.0,1.0, 1.0,-1.0,1.0,1.0,
		1.0,1.0,-1.0,1.0, 1.0,1.0,1.0,1.0 };
	static const GLfloat cube_colors[] = {
		1.0,1.0,1.0,1.0, 1.0,1.0,0.0,1.0, 1.0,0.0,1.0,1.0,
		1.0,0.0,0.0,1.0, 0.0,1.0,1.0,1.0, 0.0,1.0,0.0,1.0,
		0.0,0.0,1.0,1.0, 0.5,0.5,0.5,1.0 };
	static const GLushort cube_indices[] = {
		0,1,2,3,6,7,4,5,0xFFFF,2,6,0,4,1,5,3,7 };

	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, vao);	glBindVertexArray(vao[0]);
	glGenBuffers(1, vbo);	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions) + sizeof(cube_colors), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_positions), cube_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_positions), sizeof(cube_colors), cube_colors);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(cube_positions));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);

#if USE_PRIMITIVE_RESTART
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, NULL);
#else
	glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, NULL);
	glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT,
		(const GLvoid*)(9 * sizeof(GLushort)));
#endif
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vao[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glFlush();
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