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
	program = loadShaders("Programme/Dreiecke.vs", "Programme/Dreiecke.fs", "", "", "", "");
	glUseProgram(program);
	glGenBuffers(NumBuffers, Buffers); //Bereitstellen von namen für Buffer Objects
	glGenVertexArrays(NumVAOs, VAOs);//Bereitstellen von namen für Vertex Array Objects

	GLfloat vertices[NumVertices][2] = {
	 {-0.90,-0.90},{0.85,-0.90},{-0.90,0.85},{0.90,-0.85},{0.90,0.90},{-0.85,0.90} };
	glBindVertexArray(VAOs[Triangles]);//Erzeugen und Aktivieren eines Vertex Array Objects
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);//Erzeugen und Aktivieren eines Buffer Objects
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//Zuweisung von Server-Speicher zur Datenspeicherung
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);//Spezifizieren der Shader Variable für das Buffer Object
	glEnableVertexAttribArray(vPosition);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_RGBA);
	glutInitContextVersion(4, 5);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	//GLUT_COMPATIBILITY_PROFILE
	glutInitWindowSize(512, 512);
	glutCreateWindow(argv[0]);
	glewExperimental = GL_TRUE;
	if (glewInit()) printf("Error");
	init();


	GLuint Location;
	Location = glGetUniformLocation(program, "ModelViewProjection");
	mat4 Projection = mat4(1.0);
	mat4 View = mat4(1.0);//Kameraposition
	mat4 Model = mat4(1.0);//Transformation
	mat4 ModelViewProjection;
	Projection = ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.0f, 20.0f);
	//Projection=perspective(0.8f,4.0f/3.0f,0.1f,100.0f); //Perspektivische Projektion
	View = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0)); //nach-oben Sicht d. Kamera
	ModelViewProjection = Projection * View * Model; //Multiplikation von rechts -> links
	glUniformMatrix4fv(Location, 1, GL_FALSE, &ModelViewProjection[0][0]); //Übergabe an Shader
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);//Zeichnung
	Model = rotate(Model, 1.0f, vec3(0.0, 0.0, 1.0)); //Model=alte Matrix*neue Matrix(vec3) -> Matrix gedreht
	ModelViewProjection = Projection * View * Model;
	glUniformMatrix4fv(Location, 1, GL_FALSE, &ModelViewProjection[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}