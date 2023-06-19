#version 150
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube TextureSampler;

void main()
{    
    FragColor = texture(TextureSampler, TexCoords);
}