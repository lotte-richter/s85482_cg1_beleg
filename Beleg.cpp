#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <glew.h>
#include <freeglut.h>
#include <FreeImage.h>
#include <vector>
using namespace glm;

constexpr int CHECKERBOARD_SIZE = 8;  // 8x8 Felder
constexpr float FIELD_SIZE = 0.25f;
constexpr int CIRCLE_SEGMENTS = 32;   // Detailstufe für Zylinder/Kegel

GLuint loadShaders(const char* vertexFilePath,
    const char* fragmentFilePath,
    const char* geometryFilePath,
    const char* tesscontrolFilePath,
    const char* tessevaluationFilePath,
    const char* computeFilePath);
GLint height = 100, width = 100;
enum VAO_IDs { VAOCheckerboard, VAOBoard, VAOPieces, VAOCone, NumVAOs };
enum Buffer_IDs { ArrayBufferCheckerboard, ArrayBufferBoard, ArrayBufferPieces, ArrayBufferCone, NumBuffers };
enum Texture_IDs { TextureCheckerboard, TextureBoard, TextureWhite, TextureBlack, NumTextures };
enum ObjectIDs { BOARD_ID, CHECKERBOARD_ID, WHITE_PIECE_ID, BLACK_PIECE_ID };
enum Attrib_IDs { vPosition, in_tex_coord, vNormal };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint Textures[NumTextures];
GLuint program;
GLfloat angle, anglestep;
GLint mposx, mposy;
GLfloat PI = 3.14159;
GLfloat alpha = 0.2, beta = 0.8, dist = 5, DELTA = 0.5;
bool  isTiefenPuffer = true, isCullFace = false;

// Animation
bool isAnimating = false;
float animationProgress = 0.0f;
const float animationDuration = 2.0f; // seconds
int pawnX = 1; // Startposition x
int pawnY = 4; // Startposition y
bool movingForward = true;
vec3 startPos, targetPos, currentPos;

// ==== custom functions ===

void generateCheckerboard() {
    static const GLubyte checkerboard[CHECKERBOARD_SIZE * CHECKERBOARD_SIZE * 4] = {

        0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF,
        0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF,

        0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF,
        0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF,

        0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF,
        0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF,

        0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF,
        0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF,

        0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF,
        0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF,

        0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF,
        0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF,

        0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF,
        0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF,

        0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF,
        0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0xFF, 0xFF,0xFF,0xFF,0xFF
    };

    // Textur erstellen und binden
    glBindTexture(GL_TEXTURE_2D, Textures[TextureCheckerboard]);
    glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGBA8, CHECKERBOARD_SIZE, CHECKERBOARD_SIZE); // 8x8 Textur mit RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Vertex-Daten für das Schachbrett (2D-Ebene)
    static const GLfloat checkerboard_data[] = {
        // Position           // Normal      // TexCoord
        -1.0,  0.01, -1.0, 1.0,  0.0, 1.0, 0.0,  0,0,
         1.0,  0.01, -1.0, 1.0,  0.0, 1.0, 0.0,  1,0,
         1.0,  0.01,  1.0, 1.0,  0.0, 1.0, 0.0,  1,1,
        -1.0,  0.01,  1.0, 1.0,  0.0, 1.0, 0.0,  0,1
    };

    glBindVertexArray(VAOs[VAOCheckerboard]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferCheckerboard]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(checkerboard_data), checkerboard_data, GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(4 * sizeof(float)));
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(in_tex_coord, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(7 * sizeof(float)));
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
    glUniform1i(glGetUniformLocation(program, "objectId"), 1);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void generateBoard() {
    // Vertex-Daten für den braunen Quader (etwas größer als das Schachbrett)
    static const GLfloat board_data[] = {
        // Oberfläche (Normale nach oben)
        // Position              // Normal        // TexCoord
        -1.1,  0.0, -1.1, 1.0,  0.0, 1.0, 0.0,  0.0, 0.0,
         1.1,  0.0, -1.1, 1.0,  0.0, 1.0, 0.0,  1.0, 0.0,
         1.1,  0.0,  1.1, 1.0,  0.0, 1.0, 0.0,  1.0, 1.0,
        -1.1,  0.0,  1.1, 1.0,  0.0, 1.0, 0.0,  0.0, 1.0,

        // Unterseite (Normale nach unten)
        -1.1, -0.1, -1.1, 1.0,  0.0, -1.0, 0.0,  0.0, 0.0,
         1.1, -0.1, -1.1, 1.0,  0.0, -1.0, 0.0,  1.0, 0.0,
         1.1, -0.1,  1.1, 1.0,  0.0, -1.0, 0.0,  1.0, 1.0,
        -1.1, -0.1,  1.1, 1.0,  0.0, -1.0, 0.0,  0.0, 1.0,

        // Vorderseite (Normale nach vorne - negative Z-Richtung)
        -1.1,  0.0, -1.1, 1.0,  0.0, 0.0, -1.0,  0.0, 0.0,
        -1.1, -0.1, -1.1, 1.0,  0.0, 0.0, -1.0,  1.0, 0.0,
         1.1, -0.1, -1.1, 1.0,  0.0, 0.0, -1.0,  1.0, 1.0,
         1.1,  0.0, -1.1, 1.0,  0.0, 0.0, -1.0,  0.0, 1.0,

         // Rückseite (Normale nach hinten - positive Z-Richtung)
         -1.1,  0.0,  1.1, 1.0,  0.0, 0.0, 1.0,  0.0, 0.0,
         -1.1, -0.1,  1.1, 1.0,  0.0, 0.0, 1.0,  1.0, 0.0,
          1.1, -0.1,  1.1, 1.0,  0.0, 0.0, 1.0,  1.0, 1.0,
          1.1,  0.0,  1.1, 1.0,  0.0, 0.0, 1.0,  0.0, 1.0,

          // Linke Seite (Normale nach links - negative X-Richtung)
          -1.1,  0.0, -1.1, 1.0,  -1.0, 0.0, 0.0,  0.0, 0.0,
          -1.1, -0.1, -1.1, 1.0,  -1.0, 0.0, 0.0,  1.0, 0.0,
          -1.1, -0.1,  1.1, 1.0,  -1.0, 0.0, 0.0,  1.0, 1.0,
          -1.1,  0.0,  1.1, 1.0,  -1.0, 0.0, 0.0,  0.0, 1.0,

          // Rechte Seite (Normale nach rechts - positive X-Richtung)
           1.1,  0.0, -1.1, 1.0,  1.0, 0.0, 0.0,  0.0, 0.0,
           1.1, -0.1, -1.1, 1.0,  1.0, 0.0, 0.0,  1.0, 0.0,
           1.1, -0.1,  1.1, 1.0,  1.0, 0.0, 0.0,  1.0, 1.0,
           1.1,  0.0,  1.1, 1.0,  1.0, 0.0, 0.0,  0.0, 1.0
    };

    glBindVertexArray(VAOs[VAOBoard]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferBoard]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(board_data), board_data, GL_STATIC_DRAW);

    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(in_tex_coord, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(in_tex_coord);
}

void drawBoard() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAOs[VAOBoard]);

    // Textur binden
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Textures[TextureBoard]);
    glUniform1i(glGetUniformLocation(program, "texWood"), 0);

    // Objekt ID setzen
    glUniform1i(glGetUniformLocation(program, "objectId"), 0);

    // Quader zeichnen
    for (int i = 0; i < 24; i += 4) {
        glDrawArrays(GL_TRIANGLE_FAN, i, 4);
    }
}

void generateChessPiece() { // Bauer (einfacher Zylinder)    
    std::vector<GLfloat> vertices;

    const float height = 0.2f;
    const float radius = 0.05f;

    for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
        float angle = 2.0f * PI * i / CIRCLE_SEGMENTS;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vec3 normal = normalize(vec3(cos(angle), 0.0f, sin(angle))); // Explizite Normalisierung

        // Unten
        vertices.insert(vertices.end(), { x, 0.0f, z, 1.0f });
        vertices.insert(vertices.end(), { normal.x, normal.y, normal.z });
        vertices.insert(vertices.end(), { (float)i / CIRCLE_SEGMENTS, 0.0f });

        // Oben
        vertices.insert(vertices.end(), { x, height, z, 1.0f });
        vertices.insert(vertices.end(), { normal.x, normal.y, normal.z });
        vertices.insert(vertices.end(), { (float)i / CIRCLE_SEGMENTS, 1.0f });
    }


    // Oberer Deckel
    vertices.insert(vertices.end(), { 0.0f, height, 0.0f, 1.0f });
    vertices.insert(vertices.end(), { 0.0f, 1.0f, 0.0f });
    vertices.insert(vertices.end(), { 0.5f, 0.5f });

    for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
        float angle = 2.0f * PI * i / CIRCLE_SEGMENTS;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.insert(vertices.end(), { x, height, z, 1.0f });
        vertices.insert(vertices.end(), { 0.0f, 1.0f, 0.0f });
        vertices.insert(vertices.end(), { (cos(angle) + 1.0f) / 2.0f, (sin(angle) + 1.0f) / 2.0f });
    }

    glBindVertexArray(VAOs[VAOPieces]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferPieces]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(in_tex_coord, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(in_tex_coord);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(vNormal);
}

void drawChessPiece(int id) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAOs[VAOPieces]);


    if (id == BLACK_PIECE_ID) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, Textures[TextureBlack]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(program, "texBlack"), BLACK_PIECE_ID);
        glUniform1i(glGetUniformLocation(program, "objectId"), BLACK_PIECE_ID);
    } else{
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Textures[TextureWhite]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(program, "texWhite"), WHITE_PIECE_ID);
        glUniform1i(glGetUniformLocation(program, "objectId"), WHITE_PIECE_ID);
    }
    

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (CIRCLE_SEGMENTS + 1));

    // Oberer Deckel 
    glDrawArrays(GL_TRIANGLE_FAN, 2 * (CIRCLE_SEGMENTS + 1), CIRCLE_SEGMENTS + 2);
}

void generateConePiece() {
    const float height = 0.5f;
    const float radius = 0.05f;
    float startAngle = 0.0f;

    const float sectionAngle = PI * 2 / CIRCLE_SEGMENTS;

    // Vertex-Daten mit Position (4), Normalen (3) und Texturkoordinaten (2)
    std::vector<GLfloat> vertices;

    // Kreis Mittelpunkt
    vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f, 1.0f });
    vertices.insert(vertices.end(), { 0.0f, -1.0f, 0.0f }); // Normale
    vertices.insert(vertices.end(), { 0.5f, 0.5f }); // UV

    // Kreisumfang
    for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
        float angle = sectionAngle * i;
        vertices.insert(vertices.end(), { cos(angle) * radius, 0.0f, sin(angle) * radius, 1.0f });
        vertices.insert(vertices.end(), { 0.0f, -1.0f, 0.0f });
        vertices.insert(vertices.end(), { 0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle) }); // Korrekte UVs
    }

    //Mantel
    for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
        float angle = sectionAngle * i;
        vec3 normal = normalize(vec3(cos(angle), radius / height, sin(angle)));

        // Unten
        vertices.insert(vertices.end(), { cos(angle) * radius, 0.0f, sin(angle) * radius, 1.0f });
        vertices.insert(vertices.end(), { normal.x, normal.y, normal.z });
        vertices.insert(vertices.end(), { (float)i / CIRCLE_SEGMENTS, 0.0f });

        // Spitze
        vertices.insert(vertices.end(), { 0.0f, height, 0.0f, 1.0f });
        vertices.insert(vertices.end(), { normal.x, normal.y, normal.z });
        vertices.insert(vertices.end(), { (float)i / CIRCLE_SEGMENTS, 1.0f });
    }

    glBindVertexArray(VAOs[VAOCone]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBufferCone]);glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(in_tex_coord, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(in_tex_coord);
}

void drawConePiece(int id) {

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(VAOs[VAOCone]);

    if (id == BLACK_PIECE_ID) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, Textures[TextureBlack]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(program, "texBlack"), BLACK_PIECE_ID);
        glUniform1i(glGetUniformLocation(program, "objectId"), BLACK_PIECE_ID);
    }
    else {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Textures[TextureWhite]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glUniform1i(glGetUniformLocation(program, "texWhite"), WHITE_PIECE_ID);
        glUniform1i(glGetUniformLocation(program, "objectId"), WHITE_PIECE_ID);
    }

    glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_SEGMENTS + 2);
    glDrawArrays(GL_TRIANGLE_STRIP, CIRCLE_SEGMENTS + 2, 2 * (CIRCLE_SEGMENTS + 1));
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

void loadWhiteTexture() {
    const char* path = "images/white_metal.jpg";

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path, 0);
    FIBITMAP* dib = FreeImage_Load(fif, path, 0);
    dib = FreeImage_ConvertTo32Bits(dib);

    int width = FreeImage_GetWidth(dib);
    int height = FreeImage_GetHeight(dib);
    void* data = FreeImage_GetBits(dib);

    glBindTexture(GL_TEXTURE_2D, Textures[TextureWhite]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

    // Textureinstellungen
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    FreeImage_Unload(dib);
}

void loadBlackTexture() {
    const char* path = "images/black_metal.jpg";

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path, 0);
    FIBITMAP* dib = FreeImage_Load(fif, path, 0);
    dib = FreeImage_ConvertTo32Bits(dib);

    int width = FreeImage_GetWidth(dib);
    int height = FreeImage_GetHeight(dib);
    void* data = FreeImage_GetBits(dib);

    glBindTexture(GL_TEXTURE_2D, Textures[TextureBlack]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

    // Textureinstellungen
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    FreeImage_Unload(dib);
}

// ---- other functions ----

vec3 getChessboardPosition(int x, int y) {
    float startX = -((CHECKERBOARD_SIZE / 2.0f) * FIELD_SIZE) + (FIELD_SIZE / 2.0f);
    float startZ = startX;
    return vec3(startX + x * FIELD_SIZE, 0.0f, startZ + y * FIELD_SIZE);
}


void updateAnimation(float deltaTime) {
    if (!isAnimating) return;

    animationProgress += deltaTime / animationDuration;

    if (animationProgress >= 1.0f) {
        animationProgress = 0.0f;

        if (movingForward) {
            movingForward = false;
            startPos = targetPos;
            targetPos = getChessboardPosition(1, 4);// Zurück zur Startposition
        }
        else {
            isAnimating = false;
            pawnX = 1;
            pawnY = 0;
        }
    }

    // Lineare Interpolation zwischen Start- und Zielposition
    currentPos = startPos + (targetPos - startPos) * animationProgress;
}

void init() {
    program = loadShaders("Programme/beleg.vs", "Programme/beleg.fs", "", "", "", "");
    glUseProgram(program);
    glGenBuffers(NumBuffers, Buffers);
    glGenVertexArrays(NumVAOs, VAOs);
    glGenTextures(NumTextures, Textures);
    glClearStencil(0x0);

    generateBoard();
    generateCheckerboard();
    generateChessPiece();
    generateConePiece();

    // Load Textures
    loadWoodTexture();
    loadWhiteTexture();
    loadBlackTexture();
}

void display() {
    if (isTiefenPuffer)
        glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);

    if (isCullFace)
        glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Uniform Locations vorab holen
    GLuint modelLoc = glGetUniformLocation(program, "model");
    GLuint viewLoc = glGetUniformLocation(program, "view");
    GLuint projLoc = glGetUniformLocation(program, "projection");

    int numViewports = 4;
    int viewportWidth = width / 2;
    int viewportHeight = height / 2;

    vec3 viewpoints[4] = {
        vec3(dist * sin(beta) * sin(alpha), dist * cos(beta), dist * sin(beta) * cos(alpha)), // Frei drehbare Kamera
        vec3(1.0f, 4.0f, 1.0f),  // Draufsicht
        vec3(3.0f, 1.5f, 3.0f),  // Schräge Perspektive
        vec3(2.0f, -3.0f, 1.0f) // Ansicht von unten
    };

    mat4 projections[4] = {
        perspective(1.0f, 1.0f, 0.1f, 100.f),
        ortho(-1.5f, 1.5f, -1.5f, 1.5f, 0.1f, 100.f),
        perspective(1.2f, 1.0f, 0.1f, 100.f),
        perspective(1.2f, 1.0f, 0.1f, 100.f)
    };

    // Lichtparameter setzen
    vec3 lightPos = vec3(1.0f, 1.0f, 2.0f);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // Spotlight-Parameter
    vec3 spotlightPos = vec3(0.0f, -3.0f, 0.0f);  // �ber dem Brett
    vec3 spotlightDir = vec3(0.0f, 1.0f, 0.0f); // Nach unten gerichtet
    float spotlightCutOff = cos(radians(15.0f));
    float spotlightOuterCutOff = cos(radians(17.5f));
    vec3 spotlightColor = vec3(1.0f, 1.0f, 1.0f);

    glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &lightPos[0]);
    glUniform3fv(glGetUniformLocation(program, "lightColor"), 1, &lightColor[0]);

    glUniform3fv(glGetUniformLocation(program, "spotlightPos"), 1, &spotlightPos[0]);
    glUniform3fv(glGetUniformLocation(program, "spotlightDir"), 1, &spotlightDir[0]);
    glUniform1f(glGetUniformLocation(program, "spotlightCutOff"), spotlightCutOff);
    glUniform1f(glGetUniformLocation(program, "spotlightOuterCutOff"), spotlightOuterCutOff);
    glUniform3fv(glGetUniformLocation(program, "spotlightColor"), 1, &spotlightColor[0]);

    for (int i = 0; i < numViewports; i++) {
        int x = (i % 2) * viewportWidth;
        int y = (i / 2) * viewportHeight;
        glViewport(x, y, viewportWidth, viewportHeight);
        /*glViewport(0, 0, width, height);*/

        mat4 View = lookAt(viewpoints[i], vec3(0.5, 0.5, 0.5), vec3(0.0, 1.0, 0.0));
        mat4 Projection = projections[i];
        mat4 Model = mat4(1.0f);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Projection[0][0]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &Model[0][0]);
        glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, &viewpoints[i][0]);

        drawBoard();
        drawCheckerboard();

        // Weiße Figuren
        for (int i = 0; i < 8; i++) {
            if (i == 4 && isAnimating) {
                Model = mat4(1.0f);
                Model = translate(Model, currentPos);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &Model[0][0]);
                drawChessPiece(WHITE_PIECE_ID);
            }
            else {
                Model = mat4(1.0f);
                Model = translate(Model, getChessboardPosition(1, i));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &Model[0][0]);
                drawChessPiece(WHITE_PIECE_ID);
            }

            Model = mat4(1.0f);
            Model = translate(Model, getChessboardPosition(0, i));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &Model[0][0]);
            drawConePiece(WHITE_PIECE_ID);
        }

        // Schwarze Figuren
        for (int i = 0; i < 8; i++) {
            Model = mat4(1.0f);
            Model = translate(Model, getChessboardPosition(6, i));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &Model[0][0]);
            drawChessPiece(BLACK_PIECE_ID);

            Model = mat4(1.0f);
            Model = translate(Model, getChessboardPosition(7, i));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &Model[0][0]);
            drawConePiece(BLACK_PIECE_ID);
        }
    }    

    glutSwapBuffers();
}

void keyboard(unsigned char theKey, int mouseX, int mouseY) {
    GLint x = mouseX;
    GLint y = height - mouseY;
    switch (theKey) {
    case 'v': dist -= DELTA; display(); break; //reinzoomen
    case 'z': dist += DELTA; display(); break; //rauszoomen
    case 't': isTiefenPuffer = !isTiefenPuffer; break; //Tiefenpuffer anscahalten
    case 'c': isCullFace = !isCullFace; break; //Rückseitenentfernung
    case 'a': if (!isAnimating) { // Animation starten
        isAnimating = true;
        movingForward = true;
        animationProgress = 0.0f;
        startPos = getChessboardPosition(1, 4);
        targetPos = getChessboardPosition(3, 4);
        currentPos = startPos;
    }
            break;
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
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f; // in sekunden
    lastTime = currentTime;

    updateAnimation(deltaTime);
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