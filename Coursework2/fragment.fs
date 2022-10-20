#version 330

out vec4 FragColor;

in float height;

void main()
{ 
    int coef = 1000;
    float total = max(height/coef + 0.2f, 0.05f);
    FragColor = vec4(total, total, total, 1.0f);
}
