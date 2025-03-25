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
enum VAO_IDs { VAOSquare, VAOCircle, NumVAOs };
enum Buffer_IDs { ArrayBufferSquare, ArrayBufferCircle, NumBuffers };
enum Attrib_IDs { vPosition, vColor };
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint program;
GLfloat angle,anglestep;
GLfloat shift;
GLfloat kposx = 0.0;
GLfloat kposy = 0.0;
GLint mposx, mposy;
GLfloat PI = 3.14159;
GLfloat alpha = 0.2, beta = 0.8, dist = 5, DELTA = 0.5;

// ==== custom functions ===
void generateSquare(float edgeLen = 0.90) {
    GLfloat vertices[][2] = {
        {0.00, 0.00}, // start point
        {0.00, edgeLen}, {edgeLen, edgeLen}, {edgeLen, 0.00} // outer points
    };
    glBindVertexArray(VAOs[VAOSquare]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferSquare]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(vPosition);
}
void drawSquare(int colorR = 1, int colorG = 1, int colorB = 1) {
    glBindVertexArray(VAOs[VAOSquare]);
    glVertexAttrib3f(vColor, colorR, colorG, colorB);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void generateCircle(float radius = 0.90/*, int circPoints = 20*/) {
    const int circPoints = 20; // otherwise error on 2D-array declaration
    const float sectionAngle = 3.1415926536 * 2 / circPoints;
    GLfloat vertices[circPoints + 2][2] = { {0.00, 0.00} };

    for (int i = 0; i < circPoints; i++) {
        vertices[i + 1][0] = cos(sectionAngle * i) * radius;
        vertices[i + 1][1] = sin(sectionAngle * i) * radius;
    }

    GLfloat vertices[6][2] ={ 
        {0.0,0.0},{0.5,0.0},{0.2,0.2},
        {-0.5,0.0},{-0.2,0.0},{-0.7,0.5} };
    glDrawArrays(GL_TRIANGLES, 0, 6);

    vertices[circPoints + 1][0] = vertices[1][0];
    vertices[circPoints + 1][1] = vertices[1][1];

    glBindVertexArray(VAOs[VAOCircle]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferCircle]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(vPosition);
}
void drawCircle(int colorR = 1, int colorG = 1, int colorB = 0/*, int circPoints = 20*/) {
    int circPoints = 20;

    glBindVertexArray(VAOs[VAOCircle]);
    glVertexAttrib3f(vColor, colorR, colorG, colorB);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, circPoints + 2);
}


// ==== base program ===
void init() {
    printf("\n%s", (char*)glGetString(GL_RENDERER));
    printf("\n%s", (char*)glGetString(GL_VERSION));
    printf("\n%s\n", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    program = loadShaders("Programme/Dreiecke.vs", "Programme/Dreiecke.fs", "", "", "", "");
    glUseProgram(program);
    glGenBuffers(NumBuffers, Buffers);
    glGenVertexArrays(NumVAOs, VAOs);

    generateCircle();
    //generateSquare();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawCircle();

    GLfloat viewpoint[3];
    viewpoint[0] = dist * sin(beta) * sin(alpha);
    viewpoint[1] = dist * cos(beta);
    viewpoint[2] = dist * sin(beta) * cos(alpha);
    glViewport(0, 0, width, height);

    angle -= (0.01+anglestep);
    GLuint Location;
    Location = glGetUniformLocation(program, "ModelViewProjection");
    //mat4 Projection = mat4(1.0);
    mat4 Projection = perspective(1.0f, 1.0f, 0.1f, 100.f);
    //mat4 View = mat4(1.0);
    mat4 View = lookAt(vec3(viewpoint[0], viewpoint[1], viewpoint[2]),vec3(0.5, 0.5, 0.5), vec3(0.0, cos(beta - PI * 0.5), 0.0));
    //mat4 Model = mat4(1.0);
    mat4 Model=mat4(1.0f);
    //mat4 ModelViewProjection;
    mat4 ModelViewProjection = Projection * View * Model;
    //if (angle < -1) {
    //    shift += 0.01;
    //    Model = translate(Model, vec3(shift, 0.0, 0.0));
    //}
    //Model = rotate(Model, angle, vec3(0.0, 0.0, 1.0));
    ModelViewProjection = Projection * View * Model;
    glUniformMatrix4fv(Location, 1, GL_FALSE, &ModelViewProjection[0][0]);
    //drawSquare();

    glutSwapBuffers();
}

void keyboard_rotate(unsigned char theKey, int mouseX, int mouseY) {
    GLint x = mouseX;
    GLint y = height - mouseY;
    GLfloat px, py;
    switch (theKey) {
    case 'p':
        anglestep += 0.01;
        break;
    case 'o':
        anglestep -= 0.01;
        break;
    case 'e': exit(-1);
    }
}

void keyboard(unsigned char theKey, int mouseX, int mouseY) {
    GLint x = mouseX;
    GLint y = height - mouseY;
    switch (theKey) {
    case 'v': dist -= DELTA; display(); break;
    case 'z': dist += DELTA; display(); break;
    case 'e': exit(-1);
    }
}

void special(int specKey, int mouseX, int mouseY) {
    GLint x = mouseX;
    GLint y = height - mouseY;
    switch (specKey) {
    case GLUT_KEY_LEFT: alpha -= DELTA; display(); break;
    case GLUT_KEY_RIGHT: alpha += DELTA; display(); break;
    case GLUT_KEY_UP: beta -= DELTA; display(); break;
    case GLUT_KEY_DOWN: beta += DELTA; display(); break;
    }
}

void mouse_rotate(int theButton, int State, int mouseX, int mouseY) {
    if ((theButton == GLUT_LEFT_BUTTON) && (State == GLUT_DOWN)) {
        GLint mposx = mouseX; GLint mposy = height - mouseY;
        printf("%d,%d\n", mouseX, mouseY);
    }
}

void mouse(int theButton, int State, int mouseX, int mouseY) {
    GLint x = mouseX; GLint y = height - mouseY;
}

void motion(int mouseX, int mouseY) {
    alpha -= (mouseX - (width / 2.0)) / 10000.0;
    beta -= (mouseY - (height / 2.0)) / 10000.0;
    display();
}

void reshape(int w, int h) {
    width = w;
    height = h;
}

void idle(void) {
    //increment();
    display();
    Sleep(15);
}

void timer(int value) { //Nutzung anstatt idle Funktion
    //increment();
    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
} 

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(512, 512);
    glutInitContextVersion(4, 5);  // (4,2) (3,3);
    glutInitContextProfile(GLUT_CORE_PROFILE); //GLUT_COMPATIBILITY_PROFILE
    glutCreateWindow(argv[0]);
    glewExperimental = GL_TRUE;
    if (glewInit())  printf("Error");
    init();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    //glutKeyboardFunc(keyboard_rotate);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
}