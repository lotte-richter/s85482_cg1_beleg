
#version 450 core  // 420, 330 core , compatibility
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
out vec4 Color;
vec4 vP=vPosition;
void main(){ 
 vP[0]-=0.5;
 gl_Position = vP;
 //gl_Position = vPosition;
 Color = vColor;
  }
