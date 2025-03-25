#version 450 core  // 420, 330 core , compatibility
layout(location = 1) in vec4 vColor;
uniform mat4 ModelViewProjection;
in vec4 vPosition;
out vec4 Color;
void main(){ 
 gl_Position = ModelViewProjection*vPosition;
 Color = vColor;
  }