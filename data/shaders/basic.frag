#version 330 core
struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    //ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, TexCoord));

    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir =normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse, TexCoord));

    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular,TexCoord));

    //attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    float alpha = texture(material.texture_diffuse, TexCoord).a;
    if(alpha < 0.1)
        discard;
    FragColor = vec4(result, alpha);
    //vec4 texColor = texture(material.texture_diffuse, TexCoord);
    //if(texColor.a < 0.1)
    //    discard;
    //FragColor = texColor;
    //FragColor = vec4(diffuse,1.0);
    //FragColor.rgb = Normal;
    //FragColor.a = 1.0f;
}