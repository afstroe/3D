#include <gtc/type_ptr.hpp>

#include "camera.h"

glm::mat4 Camera::transform()
{
	glm::mat4 projectionMatrix;
	switch (_mode)
	{		
		case Mode::PERSPECTIVE:
			projectionMatrix = glm::perspective(_perspectiveData.verticalAngle, _perspectiveData.aspectRatio, _perspectiveData.nearPlane, _perspectiveData.farPlane);
		break;
		
		case Mode::ORTHO:
		default:
			projectionMatrix = glm::ortho(_parallelData.left, _parallelData.right, _parallelData.bottom, _parallelData.top, _parallelData.zNear, _parallelData.zFar);
	}	
	
	glm::mat4 viewMatrix = mvMatrix();

	projectionMatrix *= viewMatrix;

	return projectionMatrix;
}

glm::mat4 Camera::mvMatrix()
{
	glm::mat4 viewMatrix = attitudeMatrix();
	viewMatrix *= glm::translate(glm::mat4(1), _position);

	return viewMatrix;
}

glm::mat4 Camera::attitudeMatrix()
{
	glm::mat4 attitudeMatrix = glm::rotate(glm::mat4(1), glm::radians(roll()), glm::vec3(0, 0, 1));
	attitudeMatrix = glm::rotate(attitudeMatrix, glm::radians(pitch()), glm::vec3(1, 0, 0));
	attitudeMatrix = glm::rotate(attitudeMatrix, glm::radians(heading()), glm::vec3(0, 1, 0));

	return attitudeMatrix;
}
