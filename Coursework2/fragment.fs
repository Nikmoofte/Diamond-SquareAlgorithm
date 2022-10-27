#version 330

out vec4 FragColor;

in float height;

uniform float fMaxHeight;

void main()
{ 
    float coef = 205 ;
    if (height > 205 * 0.8)
    {
        float col = height/(coef + 205 * 0.5);
        FragColor = vec4(col, col, col, 1.0f);
    }
    else if(height > 0)
    {
        float col = height/coef;
        FragColor = vec4(0.58 * col, 0.7 * col, 0.41 * col, 1.0f);
    }
    else
    {
        float col = height/coef * 0.8;
        FragColor = vec4(0, 0, -col + 0.1, 1.0f);
    }
}
