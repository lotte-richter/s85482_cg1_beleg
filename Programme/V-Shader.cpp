#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glm.hpp>
//#include <gtx/transform.hpp>
#include <glew.h>
//#include <freeglut.h>
//#include <FreeImage.h>

void main()
{
	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); //Color
	glEnableVertexAttribArray(2); //Velocity

	glm:: mat4 ModelViewProjection = glm::mat4(1.0f);
	GLuint locModViePro = glGetUniformLocation(program, "ModelViewProjection");
	glUniformMatrix4fv(locModViePro, 1, GL_FALSE, &ModelViewProjection[0][0]);
	GLuint locLigCol = glGetUniformLocation(program, "LichtColor");
	glUniform4fv(locLigCol, 1, &LightColor[0]);

	// glm::vec3 LightColor=glm::vec3(1.0f,1.0f,0.2f);
	// GLuint logLigCol = glGetUniformLocation(program,"LightColor");
	//glUniform4f (locLigCol,LightCo.or[0],LightColor[1],LightColor[2],1.0);

	GLfloat LightIntensity = 0.7f;
	GLuint logLigInt = glGetUniformLocation(program, "LightIntensity");
	glUniform1f(logLigInt, LightIntensity);

	GLfloat Time = 0.0f;
	GLuint locTim = glGetUniformLocation(program, "t");
	glUniform1f(locTim, Time);
}