#version 450 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texCheckerboard;
uniform sampler2D texWood;
uniform sampler2D texMetal;

uniform int objectId; // 0: Board, 1: Checkerboard

out vec4 fColor;

void main(void) {
    vec4 texColor;
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normalize(normal), lightDir), 0.0);
    vec3 lightning = vec3(0.2) + vec3(0.8) * diff; // Ambient + Diffuse

    if(objectId == 0){
        texColor = texture(texWood, texCoord);
    } else if(objectId == 1){
        texColor = texture(texCheckerboard, texCoord);
    } else if(objectId == 2){
        texColor = vec4(texture(texMetal, texCoord).rgb * lightning, 1.0);
    }

    fColor = texColor;
}