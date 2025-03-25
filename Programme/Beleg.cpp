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
enum VAO_IDs { VAOCube, VAOSphere, VAOCone, NumVAOs };
enum Buffer_IDs { ArrayBufferCube, ArrayBufferSphere, ArrayBufferCone, NumBuffers };
enum Texture_IDs { TextureCube, TextureSphere, TextureCone, NumTextures };
enum Attrib_IDs { vPosition, in_tex_coord };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint Textures[NumTextures];
GLuint program;
GLfloat angle = 0.0f, anglestep = 0.1f;
//GLfloat shift;
//GLfloat kposx = 0.0;
//GLfloat kposy = 0.0;
//GLint mposx, mposy;
//GLfloat PI = 3.14159;
GLfloat alpha = 0.0f, beta = 0.0f, dist = 5.0f, DELTA = 0.1f;
bool isBlending = true, isDepthTest = true;



// ==== custom functions ===


void generateCube() {
    GLfloat vertices[] = {
        // Positions          // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f
    };

    glBindVertexArray(VAOs[VAOCube]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferCube]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(vPosition);
}

void generateSquare() {
    static const GLfloat tex_data[] = { 1,0,0,1, 0,1,0,1, 0,0,1,1, 1,1,0,1 };
    glBindVertexArray(VAOs[VAOSquare]);
    glBindTexture(GL_TEXTURE_2D, Textures[TextureSquare]);
    glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2, 2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_FLOAT, tex_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    static const GLfloat quad_data[] =
    { -1,-1,0,1, 1,-1,0,1, 1,1,0,1, -1,1,0,1, 0,0, 1,0, 1,1, 0,1 };
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferSquare]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(in_tex_coord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(16 * sizeof(float)));
    glEnableVertexAttribArray(in_tex_coord);
}
void drawSquare(int colorR = 1, int colorG = 1, int colorB = 1) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAOs[VAOSquare]);
    glBindTexture(GL_TEXTURE_2D, Textures[TextureSquare]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void generateCheckerboard() {
    static const GLubyte checkerboard[] = {
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00, 0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF };
    glBindTexture(GL_TEXTURE_2D, Textures[TextureCheckerboard]);
    glTexStorage2D(GL_TEXTURE_2D, 0, GL_R8, 8, 8);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, checkerboard);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    static const GLfloat quad_data[] =
    { -0.8,-0.8,0,0.8, 0.8,-0.8,0,0.8, 0.8,0.8,0,0.8, -0.8,0.8,0,0.8, 0,0, 1,0, 1,1, 0,1 };
    glBindVertexArray(VAOs[VAOCheckerboard]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferCheckerboard]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(in_tex_coord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(16 * sizeof(float)));
    glEnableVertexAttribArray(in_tex_coord);
}
void drawCheckerboard() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAOs[VAOCheckerboard]);
    glBindTexture(GL_TEXTURE_2D, Textures[TextureCheckerboard]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawCube() {
    glBindVertexArray(VAOs[VAOCube]);
    glVertexAttrib3f(in_tex_coord, 1, 1, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (int i = 0; i < 24; i += 4) {
        glDrawArrays(GL_TRIANGLE_FAN, i, 4);
    }
}
// ==== base program ===
void init() {
    program = loadShaders("Programme/Textur.vs", "Programme/Textur.fs", "", "", "", "");
    glUseProgram(program);
    glGenBuffers(NumBuffers, Buffers);
    glGenVertexArrays(NumVAOs, VAOs);
    glGenTextures(NumTextures, Textures);
    glClearStencil(0x0);
    //glEnable(GL_CULL_FACE);
    generateSquare();
    generateCheckerboard();
    generateCube();
}

void display() {
    if (isBlending)
        glEnable(GL_BLEND);
    else glDisable(GL_BLEND);

    if (isTiefenPuffer)
        glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);

    if (isStencil)
        glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);

    if (isCullFace)
        glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat viewpoint[3];
    viewpoint[0] = dist * sin(beta) * sin(alpha);
    viewpoint[1] = dist * cos(beta);
    viewpoint[2] = dist * sin(beta) * cos(alpha);
    glViewport(0, 0, width, height);

    angle -= (0.01 + anglestep);
    GLuint Location;
    Location = glGetUniformLocation(program, "ModelViewProjection");
    mat4 Projection = perspective(1.0f, 1.0f, 0.1f, 100.f);
    mat4 View = lookAt(vec3(viewpoint[0], viewpoint[1], viewpoint[2]), vec3(0.5, 0.5, 0.5), vec3(0.0, cos(beta - PI * 0.5), 0.0));
    mat4 Model = mat4(1.0f);
    mat4 ModelViewProjection = Projection * View * Model;
    ModelViewProjection = Projection * View * Model;
    glUniformMatrix4fv(Location, 1, GL_FALSE, &ModelViewProjection[0][0]);
    drawSquare();
    Model = translate(Model, vec3(1, 1, 1));
    ModelViewProjection = Projection * View * Model;
    glUniformMatrix4fv(Location, 1, GL_FALSE, &ModelViewProjection[0][0]);
    drawCheckerboard();
    drawCube();

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
    case 'v': dist -= DELTA; display(); break; //reinzoomen
    case 'z': dist += DELTA; display(); break; //rauszoomen
    case 'b': isBlending = !isBlending; break; //transparenz
    case 't': isTiefenPuffer = !isTiefenPuffer; break; //Tiefenpuffer anscahalten
    case 's': isStencil = !isStencil; break; //Stencil
    case 'c': isCullFace = !isCullFace; break; //Rückseitenentfernung
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
    //Zusätzlich tiefenpuffer
    glViewport(0, 0, width, height);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_ALWAYS, 0x1, 0x1);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL); //ergänzen beim Tiefenpuffer
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
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
}