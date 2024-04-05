#include "ModelObject.h"

#include "PerspectiveCameraObject.h"
#include "LightObject.h"

#include"stb_image.h"

ModelObject::ModelObject()
{
    
}

ModelObject::ModelObject(string name)
{
    this->name = name;
}

string ModelObject::GetModelName()
{
    return this->name;
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

    loadedModel = true;
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
    TranslationMatrix = identity_matrix4;
    TranslationMatrix = glm::translate(TranslationMatrix, pos);

    Transform = TranslationMatrix * RotationMatrix * ScaleMatrix;
}

void ModelObject::SetScale(glm::vec3 scale)
{
    ScaleMatrix = identity_matrix4;
    ScaleMatrix = glm::scale(ScaleMatrix, scale);

    Transform = TranslationMatrix * RotationMatrix * ScaleMatrix;
}

void ModelObject::InsertPartialData(int index, std::vector<float> partialData)
{
    partialVertexDataMap[index] = partialData;
}

void ModelObject::LoadModelData()
{
    for (auto pair : partialVertexDataMap)
    {
        fullVertexData.insert(fullVertexData.end(), pair.second.begin(), pair.second.end());
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

    loadedModel = true;

    partialVertexDataMap.clear();
}

void ModelObject::InsertPartialTextureData(float width, float height, int index, unsigned bytesPerPixel, unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
    if (pixels == NULL)
    {
        pixels = new GLubyte[width * height * bytesPerPixel];
        this->width = width;
        this->height = height;
    }

    pixels[index] = r;
    pixels[index + 1] = g;
    pixels[index + 2] = b;
    pixels[index + 3] = a;

}

void ModelObject::LoadTextureData(GLint imageFormat)
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (window == NULL)
    {
        return;
    }

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, (tex_bytes != NULL ? tex_bytes : pixels));
    glGenerateMipmap(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    if (tex_bytes != NULL)
    {
        stbi_image_free(tex_bytes);
    }
}
