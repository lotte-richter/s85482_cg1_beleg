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
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVertices = 9; //Anz zu zeichnenede Punkte
GLuint program;

void generateTriangle() {
	GLfloat vertices[NumVertices][2];
	for (int i = 0; i < NumVertices; i = i + 3) {
		vertices[i][0] = 0; //x
		vertices[i][1] = 0.25 + 0.1 * i; //y
		vertices[i+1][0] = -0.25;
		vertices[i+1][1] = 0.1 * i;
		vertices[i+2][0] = 0.25;
		vertices[i+2][1] = 0.1 * i;
	}
	glBindVertexArray(VAOs[Triangles]);//Erzeugen und Aktivieren eines Vertex Array Objects
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]); // -||- eines Buffer Objects
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Zuweisung von Server-Speicher zur Datenspeicherung
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); //Spezifizieren der Variable für das Buffer Object
	glEnableVertexAttribArray(vPosition);
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
	generateTriangle();
} 

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT); //Löschen des Frame Buffers
	glBindVertexArray(VAOs[Triangles]);
	for (int i = 0; i < NumVertices/3; i++) {
		glVertexAttrib3f(vColor, 1, 1, 0);
		glDrawArrays(GL_LINE_LOOP, i * 3, 3);
	}
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
