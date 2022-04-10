#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 CoordPara;

void main(){
    FragColor = vec4(CoordPara.x, 0.0, CoordPara.y, 1.0);
}
