#include "camera.h"

glm::mat4 Camera::transform()
{
	switch (_mode)
	{
		glm::mat4 projectionMatrix;
		case Mode::PERSPECTIVE:
			projectionMatrix = glm::perspective(_perspectiveData.verticalAngle, _perspectiveData.aspectRatio, _perspectiveData.nearPlane, _perspectiveData.farPlane);
		break;
		
		case Mode::ORTHO:
		default:
			projectionMatrix = glm::ortho(_parallelData.left, _parallelData.right, _parallelData.bottom, _parallelData.top, _parallelData.zNear, _parallelData.zFar);
	}

	return glm::mat4();
}
