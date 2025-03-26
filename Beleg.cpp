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
enum VAO_IDs { VAOCheckerboard, VAOBoard, NumVAOs };
enum Buffer_IDs { ArrayBufferCheckerboard, ArrayBufferBoard, NumBuffers };
enum Texture_IDs { TextureCheckerboard, TextureBoard, NumTextures };
enum Object_IDs { BOARD_ID, CHECKERBOARD_ID, NUM_OBJECTS };
enum Attrib_IDs { vPosition, in_tex_coord };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint Textures[NumTextures];
GLuint program;
GLfloat angle, anglestep;
GLfloat shift;
GLfloat kposx = 0.0;
GLfloat kposy = 0.0;
GLint mposx, mposy;
GLfloat PI = 3.14159;
GLfloat alpha = 0.2, beta = 0.8, dist = 5, DELTA = 0.5;
bool isBlending = true, isTiefenPuffer = true, isStencil = false, isCullFace = false;



// ==== custom functions ===

void generateCheckerboard() {
    // 8x8 Schachbrettmuster (RGBA)
    static const GLubyte checkerboard[8 * 8 * 4] = {        
        0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF,
        0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF,
        
        0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF,
        0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF,
        
        0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF,
        0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF,
        
        0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF,
        0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF,
        
        0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF,
        0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF,
        
        0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF,
        0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF,
        
        0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF,
        0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF,
        
        0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF,
        0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x8B,0x45,0x13,0xFF, 0xFF,0xFF,0xFF,0xFF
    };

    // Textur erstellen und binden
    glBindTexture(GL_TEXTURE_2D, Textures[TextureCheckerboard]);
    glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 8, 8); // 8x8 Textur mit RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Vertex-Daten für das Schachbrett (2D-Ebene)
    static const GLfloat checkerboard_data[] = {
        // Oberfläche (leicht über dem braunen Quader)
        -1.0,  0.01, -1.0, 1.0,  0,0,
         1.0,  0.01, -1.0, 1.0,  1,0,
         1.0,  0.01,  1.0, 1.0,  1,1,
        -1.0,  0.01,  1.0, 1.0,  0,1
    };

    glBindVertexArray(VAOs[VAOCheckerboard]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferCheckerboard]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(checkerboard_data), checkerboard_data, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(in_tex_coord, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(4 * sizeof(float)));
    glEnableVertexAttribArray(in_tex_coord);
}
void drawCheckerboard() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAOs[VAOCheckerboard]);

    // Textur binden
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Textures[TextureCheckerboard]);
    glUniform1i(glGetUniformLocation(program, "texCheckerboard"), 1);

    // Object ID setzen
    glUniform1i(glGetUniformLocation(program, "objectId"), CHECKERBOARD_ID);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void generateBoard() {
    unsigned char boardTexture[4] = { 0x8B, 0x45, 0x13, 0xFF }; // Braun in RGBA

    glBindTexture(GL_TEXTURE_2D, Textures[TextureBoard]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, boardTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Vertex-Daten für den braunen Quader (etwas größer als das Schachbrett)
    static const GLfloat board_data[] = {
        // Oberfläche
        -1.1,  0.0, -1.1, 1.0,  0,0,
         1.1,  0.0, -1.1, 1.0,  1,0,
         1.1,  0.0,  1.1, 1.0,  1,1,
        -1.1,  0.0,  1.1, 1.0,  0,1,

        // Unterseite
        -1.1, -0.1, -1.1, 1.0,  0,0,
         1.1, -0.1, -1.1, 1.0,  1,0,
         1.1, -0.1,  1.1, 1.0,  1,1,
        -1.1, -0.1,  1.1, 1.0,  0,1,

        // Seiten
        -1.1,  0.0, -1.1, 1.0,  0,0,
        -1.1, -0.1, -1.1, 1.0,  1,0,
         1.1, -0.1, -1.1, 1.0,  1,1,
         1.1,  0.0, -1.1, 1.0,  0,1,

        -1.1,  0.0,  1.1, 1.0,  0,0,
        -1.1, -0.1,  1.1, 1.0,  1,0,
         1.1, -0.1,  1.1, 1.0,  1,1,
         1.1,  0.0,  1.1, 1.0,  0,1,

        -1.1,  0.0, -1.1, 1.0,  0,0,
        -1.1, -0.1, -1.1, 1.0,  1,0,
        -1.1, -0.1,  1.1, 1.0,  1,1,
        -1.1,  0.0,  1.1, 1.0,  0,1,

         1.1,  0.0, -1.1, 1.0,  0,0,
         1.1, -0.1, -1.1, 1.0,  1,0,
         1.1, -0.1,  1.1, 1.0,  1,1,
         1.1,  0.0,  1.1, 1.0,  0,1
    };

    glBindVertexArray(VAOs[VAOBoard]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferBoard]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(board_data), board_data, GL_STATIC_DRAW);

    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(in_tex_coord, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(in_tex_coord);
}

void drawBoard() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAOs[VAOBoard]);

    // Textur binden
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Textures[TextureBoard]);
    glUniform1i(glGetUniformLocation(program, "texBoard"), 0);

    // Objekt ID setzen
    glUniform1i(glGetUniformLocation(program, "objectId"), BOARD_ID);

    // Quader zeichnen
    for (int i = 0; i < 24; i += 4) {
        glDrawArrays(GL_TRIANGLE_FAN, i, 4);
    }
}

// ---- Textures ----
void loadWoodTexture() {
    const char* path = "images/wood.jpg";

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path, 0);
    FIBITMAP* dib = FreeImage_Load(fif, path, 0);
    dib = FreeImage_ConvertTo32Bits(dib);

    int width = FreeImage_GetWidth(dib);
    int height = FreeImage_GetHeight(dib);
    void* data = FreeImage_GetBits(dib);

    glBindTexture(GL_TEXTURE_2D, Textures[TextureBoard]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

    // Textureinstellungen
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    FreeImage_Unload(dib);
}

void init() {
    program = loadShaders("Programme/beleg.vs", "Programme/beleg.fs", "", "", "", "");
    glUseProgram(program);
    glGenBuffers(NumBuffers, Buffers);
    glGenVertexArrays(NumVAOs, VAOs);
    glGenTextures(NumTextures, Textures);
    glClearStencil(0x0);
    //glEnable(GL_CULL_FACE);
    generateBoard();
    generateCheckerboard();

    // Sicherstellen dass Texturen unterschiedliche IDs haben
    printf("Board TexID: %d, Checkerboard TexID: %d\n",
        Textures[TextureBoard],
        Textures[TextureCheckerboard]);
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
    
    glUniformMatrix4fv(Location, 1, GL_FALSE, &ModelViewProjection[0][0]);
    drawBoard();
    drawCheckerboard();

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