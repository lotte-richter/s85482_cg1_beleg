#version 450 core  // 420, 330 core , compatibility
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
out vec4 Color;
uniform mat4 ModelViewProjection;
void main(){ 
 gl_Position=ModelViewProjection*vPosition;
 Color = vColor;
  }