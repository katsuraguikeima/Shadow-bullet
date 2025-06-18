#version 330 core
in vec3 texCoords;
out vec4 FragColor;

uniform sampler2D diffuseMap;   // Textura difusa
uniform sampler2D normalMap;    // Mapa de normales

void main()
{
    // Obtén las normales de la textura
    vec3 normal = texture(normalMap, texCoords.xy).rgb;
    normal = normalize(normal * 2.0 - 1.0);  // Convierte de [0,1] a [-1,1]

    // Aplica iluminación (ejemplo de iluminación difusa simple)
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 color = texture(diffuseMap, texCoords.xy).rgb;
    FragColor = vec4(color * diff, 1.0);
}
