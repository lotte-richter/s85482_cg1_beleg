#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <glew.h>
#include <freeglut.h>
#include <FreeImage.h>
using namespace glm;

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
const GLuint NumVertices = 6; //Anz zu zeichnenede Punkte
GLuint program;

void init(void)
{
	printf("\n%s", (char*)glGetString(GL_RENDERER));
	printf("\n%s", (char*)glGetString(GL_VERSION));
	printf("\n%s\n", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);//Erzeugen und Aktivieren eines Vertex Array Objects
	struct VertexData { GLubyte color[4]; GLfloat position[2]; };
	VertexData vertices[NumVertices] = {
		{{255,0,0,255},{-0.9,-0.9}},{{0,255,0,255},{0.85,-0.9}},
		{{0,0,255},{-0.9,0.85}},{{10,10,10,255},{0.9,-0.85}},
		{{100,100,100,255},{0.9,0.9}},{{255,255,255,255},{-0.85,0.9}}
	};
	glGenBuffers(NumBuffers, Buffers); //Bereitstellen von Namen für Buffer Objects
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]); // -||- eines Buffer Objects
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Zuweisung von Server-Speicher zur Datenspeicherung
	program = loadShaders("Programme/Dreiecke.vs", "Programme/Dreiecke.fs", "", "", "", "");
	glUseProgram(program);
	glVertexAttribPointer(vColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexData), (void*)(0));
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(vertices[0].color))); 
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vColor);
} //Aktivieren der Shader Variable

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT); //Löschen des Frame Buffers
	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

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

	glViewport(0, 0, 500, 500);
	GLuint Location;
	Location = glGetUniformLocation(program, "ModelViewProjection");
	mat4 Projection = mat4(1.0);
	mat4 View = mat4(1.0);
	mat4 Model = mat4(1.0);
	mat4 ModelViewProjection;
	Projection = ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.0f, 20.0f);
	//Projection = perspective(0.8f, 4.0f / 3.0f, 0.1f, 10.0f);
	View = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0));
	ModelViewProjection = Projection * View * Model;
	glUniformMatrix4fv(Location, 1, GL_FALSE, &ModelViewProjection[0][0]);
	//glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	Model = rotate(Model, 1.0f, vec3(0.0, 0.0, 1.0));
	ModelViewProjection = Projection * View * Model;
	glUniformMatrix4fv(Location, 1, GL_FALSE, &ModelViewProjection[0][0]);
	//glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMainLoop();
}
