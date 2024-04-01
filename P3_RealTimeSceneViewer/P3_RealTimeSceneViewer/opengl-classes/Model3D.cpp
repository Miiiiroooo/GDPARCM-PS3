#include "Model3D.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Utils.h"


Model3D::Model3D(ModelReference* objRef, Texture* texture) : objRef(objRef), texture(texture)
{
	transform.position = glm::vec3(0, 0, 0);
	transform.Rotate(glm::vec3(0, 0, 0));
	transform.scale = glm::vec3(1, 1, 1);
	color = glm::vec3(1.f, 1.f, 1.f);
}

glm::mat4 Model3D::GetTransformationMatrix()
{
	glm::mat4 translation = glm::translate(Utils::IDENTITY_MAT4, transform.position);
	glm::mat4 rotation = glm::toMat4(transform.GetRotation());
	glm::mat4 scale = glm::scale(Utils::IDENTITY_MAT4, transform.scale); 

	glm::mat4 transformation_matrix = translation * rotation * scale;

	return transformation_matrix;
}