
#include "camera.h"



glm::mat4 Camera::transform()
{
	glm::mat4 projMatrix = projectionMatrix();
	
	glm::mat4 viewMatrix = mvMatrix();

	projMatrix *= viewMatrix;

	return projMatrix;
}

glm::mat4 Camera::projectionMatrix()
{
	glm::mat4 projectionMatrix;
	switch (m_mode)
	{
	case Mode::PERSPECTIVE:
		projectionMatrix = glm::perspective(m_perspectiveData.verticalAngle, m_perspectiveData.aspectRatio, m_perspectiveData.nearPlane, m_perspectiveData.farPlane);
		break;

	case Mode::ORTHO:
	default:
		projectionMatrix = glm::ortho(m_parallelData.left, m_parallelData.right, m_parallelData.bottom, m_parallelData.top, m_parallelData.zNear, m_parallelData.zFar);
	}
	return projectionMatrix;
}

glm::mat4 Camera::mvMatrix()
{
	glm::mat4 viewMatrix = attitudeMatrix();

	viewMatrix *= glm::translate(glm::mat4(1), -m_position);

	return viewMatrix;
}

glm::mat4 Camera::attitudeMatrix()
{
	glm::mat4 attitudeMatrix = glm::rotate(glm::mat4(1), glm::radians(roll()), glm::vec3(0, 0, 1));
	attitudeMatrix = glm::rotate(attitudeMatrix, glm::radians(pitch()), glm::vec3(1, 0, 0));
	attitudeMatrix = glm::rotate(attitudeMatrix, glm::radians(heading()), glm::vec3(0, 1, 0));

	return attitudeMatrix;
}

glm::mat4 Camera::lookAt(const glm::vec3& at)
{
	glm::vec4 up = attitudeMatrix() * glm::vec4(0, 1, 0, 1);
	
	return glm::lookAt(m_position, at, up.xyz());
}
