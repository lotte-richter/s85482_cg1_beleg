in vec4 color;
uniform vec4 LightColor;
uniform float LightIntensity;
out vec4 fCol;

void main(){
  float cr=min(max(color.r*LightColor.r*LightIntensity),0.0),1.0);
  float cg=min(max(color.g*LightColor.g*LightIntensity),0.0),1.0);
  float cb=min(max(color.b*LightColor.b*LightIntensity),0.0),1.0);
  float ca=color.a*0.9; 
  fCol=vec4(cr,cg,cb,ca);
}