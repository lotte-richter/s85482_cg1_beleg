#version 450 core
layout(location = 0) in vec4 vPosition;      // Vertex-Position
layout(location = 1) in vec2 in_tex_coord;   // Texturkoordinaten

out vec2 texCoord;                       // Weiter an Fragment-Shader
out vec4 Color;
uniform mat4 ModelViewProjection;            // Transformationsmatrix

void main() {
    gl_Position = ModelViewProjection * vPosition; // Standard-Transformation
    Color = vec4(1.0);                      // Standardfarbe, falls benötigt
    texCoord = in_tex_coord;             // Texturkoordinaten durchreichen
}