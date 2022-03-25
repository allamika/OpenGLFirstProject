
#version 330 core
out vec4 FragColor;
  
uniform float redUniform;
uniform float greenUniform;
uniform float blueUniform;

void main()
{
    FragColor = vec4(redUniform, greenUniform, blueUniform, 1.0f);
}
