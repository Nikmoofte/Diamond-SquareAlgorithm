#version 330

out vec4 FragColor;

in float height;

void main()
{ 
    int coef = 50;
    FragColor = vec4(height/coef + 0.2f, height/coef + 0.2f, height/coef + 0.2f, 1.0f);
}
