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
enum VAO_IDs { VAORectangle, VAOCircle, NumVAOs };
enum Buffer_IDs { ArrayBufferRectangle, ArrayBufferCircle, NumBuffers };
enum Attrib_IDs { vPosition, vColor };
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVerticesRectangle = 4;
const GLuint NumVerticesCircle = 33;
GLuint program;

void generateRectangle(GLfloat vertices[NumVerticesRectangle][2], GLuint Size)
{
	glBindVertexArray(VAOs[VAORectangle]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferRectangle]);
	glBufferData(GL_ARRAY_BUFFER, Size, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); //Spezifizieren der Variable für das Buffer Object
	glEnableVertexAttribArray(vPosition); //Aktivieren der Shader Variable
}

void drawRectangle()
{
	glBindVertexArray(VAOs[VAORectangle]);
	glVertexAttrib3f(vColor, 1, 0, 0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NumVerticesRectangle);
}

void init(void)
{
	printf("\n%s", (char*)glGetString(GL_RENDERER));
	printf("\n%s", (char*)glGetString(GL_VERSION));
	printf("\n%s\n", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	program = loadShaders("Programme/Dreiecke.vs", "Programme/Dreiecke.fs", "", "", "", "");
	glUseProgram(program);
	glGenBuffers(NumBuffers, Buffers); //Bereitstellen von Namen für Buffer Objects
	glGenVertexArrays(NumVAOs, VAOs);//Bereitstellen von Namen für Vertex Array Objects
	GLfloat vertices[NumVerticesRectangle][2] = {
	 {-0.90,-0.90},{0.90,-0.90},{0.90,0.90},{-0.90,0.90} };

	generateRectangle(vertices, sizeof(vertices));
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT); //Löschen des Frame Buffers
	drawRectangle();
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