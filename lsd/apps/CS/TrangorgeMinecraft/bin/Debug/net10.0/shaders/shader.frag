#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in float BlockType;

void main()
{
    vec3 color;
    
    if (BlockType < 1.5)
        color = vec3(0.3, 0.7, 0.2);
    else if (BlockType < 2.5)
        color = vec3(0.5, 0.35, 0.2);
    else if (BlockType < 3.5)
        color = vec3(0.5, 0.5, 0.5);
    else if (BlockType < 4.5)
        color = vec3(0.6, 0.4, 0.2);
    else
        color = vec3(0.2, 0.5, 0.1);

    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(Normal, lightDir), 0.3);
    vec3 result = color * diff;

    FragColor = vec4(result, 1.0);
}