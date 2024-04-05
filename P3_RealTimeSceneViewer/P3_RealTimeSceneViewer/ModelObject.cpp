#include "ModelObject.h"

#include "PerspectiveCameraObject.h"
#include "LightObject.h"

#include"stb_image.h"

ModelObject::ModelObject()
{
    
}

void ModelObject::SetupModel(string path)
{
    Path = path;

    Success = tinyobj::LoadObj(&Attributes, &Shapes, &Material, &warning, &error, Path.c_str());

    for (int i = 0; i < Shapes[0].mesh.indices.size(); i++)
    {
        indices.push_back(Shapes[0].mesh.indices[i].vertex_index);
    }

    for (int i = 0; i < Shapes[0].mesh.indices.size(); i++)
    {
        tinyobj::index_t vData = Shapes[0].mesh.indices[i];

        fullVertexData.push_back(Attributes.vertices[(vData.vertex_index * 3)]);
        fullVertexData.push_back(Attributes.vertices[(vData.vertex_index * 3) + 1]);
        fullVertexData.push_back(Attributes.vertices[(vData.vertex_index * 3) + 2]);

        fullVertexData.push_back(Attributes.normals[(vData.normal_index * 3)]);
        fullVertexData.push_back(Attributes.normals[(vData.normal_index * 3) + 1]);
        fullVertexData.push_back(Attributes.normals[(vData.normal_index * 3) + 2]);

        fullVertexData.push_back(Attributes.texcoords[(vData.texcoord_index * 2)]);
        fullVertexData.push_back(Attributes.texcoords[(vData.texcoord_index * 2) + 1]);

    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * fullVertexData.size(), fullVertexData.data(), GL_DYNAMIC_DRAW);

    //Change 5 to 8. position XYZ, normal XYZ, UV = 8


    GLintptr normPtr = 3 * sizeof(GLfloat);
    GLintptr uvPtr = 6 * sizeof(GLfloat);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (void*)normPtr);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)uvPtr);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    Transform = glm::translate(identity_matrix4, glm::vec3(0, 0, 0)); // x,y,z
    //scale
    Transform = glm::scale(Transform, glm::vec3(1.f, 1.f, 1.f)); // x,y,z
}

void ModelObject::SetupTexture(string path, GLuint imageFormat)
{
    stbi_set_flip_vertically_on_load(true);
    tex_bytes = stbi_load(path.c_str(), &img_width, &img_height, &colorChannels, 0);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, img_width, img_height, 0, imageFormat, GL_UNSIGNED_BYTE, tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);
}

void ModelObject::Draw(GLuint ShaderProgram, PerspectiveCameraObject perspectiveCamera, LightObject light)
{
    light.ApplyLight(ShaderProgram);

    perspectiveCamera.ApplyCameraPosition(ShaderProgram);
    perspectiveCamera.ApplyProjection(ShaderProgram);
    perspectiveCamera.ApplyView(ShaderProgram);

    unsigned int TransformLoc = glGetUniformLocation(ShaderProgram, "transform");
    glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, glm::value_ptr(Transform));

    glActiveTexture(GL_TEXTURE0);
    GLuint tex0Address = glGetUniformLocation(ShaderProgram, "tex0");
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glUniform1i(tex0Address, 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);

}

void ModelObject::DeleteBuffers()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ModelObject::SetPosition(glm::vec3 pos)
{
    Transform = glm::translate(Transform, pos);
}

void ModelObject::SetScale(glm::vec3 scale)
{
    Transform = glm::scale(Transform, scale); 
}
