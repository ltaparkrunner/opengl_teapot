#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 porcelainColor = vec3(0.92, 0.94, 0.96); 
    vec3 ambient = 0.25 * porcelainColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
//    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * porcelainColor * 0.7;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 128.0);
    vec3 specular = vec3(1.0, 1.0, 1.0) * spec * 1.2; 

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
