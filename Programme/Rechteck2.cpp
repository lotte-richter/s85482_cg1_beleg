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
const GLuint NumVerticesRectangle = 4; //= 6; wenn man 6 Werte angibt in vertices (für Mittellinie)
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
	glDrawArrays(GL_LINE_LOOP, 0, NumVerticesRectangle); //Rechteck wird umrandet
	//glDrawArrays(GL_TRIANGLE_FAN, 0, NumVerticesRectangle); --> Das Rechteck wird ausgefüllt
}

void generateCircle()
{
	int PI = 3.14;
	int r = 1;
	GLfloat vertices[NumVerticesCircle][2];
	vertices[0][0] = 0;
	vertices[0][1] = 0;
	for (int i = 1; i <= 32; i++) {
		vertices[i][0] = r * sin(2 * PI * 1 / 30.0);
		vertices[i][1] = r * cos(2 * PI * 1 / 30.0);
	}
	glBindVertexArray(VAOs[VAOCircle]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferCircle]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); //Spezifizieren der Variable für das Buffer Object
	glEnableVertexAttribArray(vPosition);
}

void drawCircle() {
	glBindVertexArray(VAOs[VAOCircle]);
	glVertexAttrib3f(vColor, 1, 0, 0);
	glDrawArrays(GL_LINE_LOOP, 0, NumVerticesCircle);
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
	//GLfloat vertices[NumVerticesRectangle][2] = {
	// {-0.90,-0.90},{0.90,-0.90},{0.90,0.90},{-0.90,0.90} };
	//generateRectangle(vertices, sizeof(vertices));
	generateCircle();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT); //Löschen des Frame Buffers
	//drawRectangle();
	drawCircle();
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