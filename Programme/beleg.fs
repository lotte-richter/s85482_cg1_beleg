#version 450 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texCheckerboard;
uniform sampler2D texWood;
uniform sampler2D texMetal;

uniform int objectId; // 0: Board, 1: Checkerboard

// Lichtparameter
uniform vec3 lightPos;
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 viewPos;

out vec4 fColor;

void main(void) {
    // Gemeinsame Lichtberechnung
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float primaryDiffuseStrength = 1.0f;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * primaryDiffuseStrength;

    // Specular (nur für Metall)
    float specularStrength = 0.5;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 primaryLightResult = diffuse + specular;

    // External light source ??

    vec3 lightResult = ambient + primaryLightResult; // + externalLightResult
    
    vec4 texColor;

    if(objectId == 0){
        texColor = texture(texWood, texCoord);
    } else if(objectId == 1){
        texColor = texture(texCheckerboard, texCoord);
    } else if(objectId == 2){
        texColor = texture(texMetal, texCoord);
    }

    // fColor = texColor;
    fColor = texColor * vec4(lightResult, 1.0f);
}