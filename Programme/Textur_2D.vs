#version 450 core  // 420, 330 core , compatibility
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 in_tex_coord;
out vec2 vs_tex_coord;
void main (void){
gl_Position=vposition;
vs_tex_coord=in_tex_coord; }