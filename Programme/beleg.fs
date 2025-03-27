#version 450 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texCheckerboard;
uniform sampler2D texWood;
uniform sampler2D texWhite;
uniform sampler2D texBlack;

uniform int objectId; // 0: Board, 1: Checkerboard, 2: White Chess Pieces, 3: Black Chess Pieces

// Lichtparameter
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 spotlightPos;
uniform vec3 spotlightDir;
uniform float spotlightCutOff;
uniform float spotlightOuterCutOff;
uniform vec3 spotlightColor;

out vec4 fColor;


void main(void) {
    // Gemeinsame Lichtberechnung
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float primaryDiffuseStrength = 1.0f;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * primaryDiffuseStrength;

    // Specular (nur für Metall)
    float specularStrength = 2.1;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 primaryLightResult = diffuse + specular;

    // Spotlight-Berechnung    
    float spotlightIntensity = 2.0f;
    vec3 spotlightLightDir = normalize(spotlightDir);
    vec3 spotlightToFrag = normalize(fragPos - spotlightPos);
    float theta = dot(spotlightToFrag, spotlightLightDir);

    float epsilon = spotlightCutOff - spotlightOuterCutOff;
    float spotlightIntensityFactor = clamp((theta - spotlightOuterCutOff) / epsilon, 0.0, 2.0);

    // Spotlight Diffuse
    float spotlightDiff = max(dot(norm, -spotlightToFrag), 0.0);
    vec3 spotlightDiffuse = spotlightDiff * spotlightColor;

    // Spotlight Specular
    vec3 spotlightReflectDir = reflect(spotlightToFrag, norm);
    float spotlightSpec = pow(max(dot(viewDir, spotlightReflectDir), 0.0), 32.0);
    vec3 spotlightSpecular = specularStrength * spotlightSpec * spotlightColor;

    vec3 spotlightResult = (spotlightDiffuse + spotlightSpecular) * spotlightIntensityFactor * spotlightIntensity;

    // External light source ??

    vec3 lightResult = ambient + primaryLightResult + spotlightResult; // + externalLightResult
    
    vec4 texColor;

    if(objectId == 0){
        texColor = texture(texWood, texCoord);
    } else if(objectId == 1){
        texColor = texture(texCheckerboard, texCoord);
    } else if(objectId == 2){
        texColor = texture(texWhite, texCoord);
    } else if(objectId == 3){
        texColor = texture(texBlack, texCoord);
    }

    // fColor = texColor;
    fColor = texColor * vec4(lightResult, 1.0f);
}