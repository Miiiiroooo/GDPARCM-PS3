#version 330 core //Version Number

//the position variable has attribute position 0
//Accesses the vertex position and assigns it to aPos
layout(location = 0) in vec3 aPos; 

//Creates a Transform variable
//We'll assign the Transformation matrix here
uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

layout(location = 1) in vec3 vertexNormal;
out vec3 normCoord;
out vec3 fragPos;

layout(location = 2) in vec2 aTex;
out vec2 texCoord;

void main()
{
    texCoord = aTex;

    //gl_Position is predefined
    //This is the final position of the matrix when drawn
    gl_Position =   projection * view*
                    transform * //Multiply the matrix with the position
                    vec4(aPos, 1.0); // Turns our 3x1 matrix into a 4x1

    mat3 modelMatrix = mat3(transpose(inverse(transform)));
    normCoord = modelMatrix * vertexNormal;

    fragPos = vec3(transform * vec4(aPos,1.0));


}