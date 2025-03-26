#version 450 core
layout(location = 0) in vec4 vPosition;      // Vertex-Position
layout(location = 1) in vec2 in_tex_coord;   // Texturkoordinaten
layout(location = 2) in vec3 vNormal;

out vec2 texCoord;                       // Weiter an Fragment-Shader
out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vPosition; // Standard-Transformation
    fragPos = vec3(model * vPosition);

    texCoord = in_tex_coord;             // Texturkoordinaten durchreichen

    normal = mat3(transpose(inverse(model))) * vNormal;
}