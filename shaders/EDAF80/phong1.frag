#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;
in vec3 Bitangent;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

void main()
{
    // Retrieve the normal from the normal map and perform tangent space to world space conversion
    vec3 norm = texture(normalMap, TexCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0); // Convert from [0, 1] to [-1, 1]
    norm = normalize(Tangent * norm.x + Bitangent * norm.y + Normal * norm.z);

    // Calculate the lighting vectors
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Calculate the reflection vector
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient component
    vec3 ambient = 0.1 * texture(material.diffuse, TexCoords).rgb;

    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * texture(material.diffuse, TexCoords).rgb;

    // Specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.shininess * texture(material.specular, TexCoords).rgb;

    // Combine all components
    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(lighting, 1.0);
}
