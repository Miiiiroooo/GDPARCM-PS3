#version 330 core //Version

uniform sampler2D tex0;
in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float ambientStr;
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform vec3 directional;
uniform float specStr;
uniform float specPhong;

uniform float brightness;
uniform bool pointLight;

uniform sampler2D norm_tex;

out vec4 FragColor; //Returns a Color

void main()
{
    if(pointLight){

        float distance = length(lightPos - fragPos); 
        float intensity = brightness / (pow(distance, 2)); 

        vec3 normal = normalize(normCoord);

        vec3 lightDir = normalize(lightPos - fragPos);

        float diff = max(dot(normal,lightDir), 0.0);
        vec3 diffuse = diff * intensity * lightColor; 

        vec3 ambientCol = ambientColor * ambientStr;// * intensity; //multiply intensity

        vec3 viewDir = normalize(cameraPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, normal);

        float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
        vec3 specColor = spec * specStr * intensity * lightColor;  


        FragColor =  vec4((specColor + ambientCol + diffuse) ,1.0) * (texture(tex0, texCoord));
    }
    else{
        vec3 normal = normalize(normCoord);

        vec3 lightDir = directional;

        float diff = max(dot(normal,lightDir), 0.0);
        vec3 diffuse = diff * lightColor; 

        vec3 ambientCol = ambientColor * ambientStr;

        vec3 viewDir = directional;
        vec3 reflectDir = reflect(-lightDir, normal);

        float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
        vec3 specColor = spec * specStr * lightColor;  


        FragColor =  vec4((specColor + ambientCol + diffuse) ,1.0) * (texture(tex0, texCoord));
    
    }

    
}