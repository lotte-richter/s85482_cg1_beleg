#version 450 core
in vec4 Color;
in vec2 texCoord;

uniform sampler2D texCheckerboard;
uniform sampler2D texWood;

uniform int objectId; // 0: Board, 1: Checkerboard

out vec4 fColor;

void main(void) {
    vec4 texColor;

    if(objectId == 0){
        texColor = texture(texWood, texCoord);
    } else if(objectId == 1){
        texColor = texture(texCheckerboard, texCoord);
    }

    fColor = texColor;
}