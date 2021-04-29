#include "opengl/camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::move(float along, float side)
{
	auto direction = getDirection();
	auto sideDirection = glm::normalize(glm::cross(direction, glm::vec3{0.f, 1.f, 0.f}));
	position += along * direction + side * sideDirection;
}

void Camera::rotate(float deltaPitch, float deltaYaw)
{
	pitch = glm::clamp(pitch + deltaPitch, -89.f, 89.f);
	yaw += deltaYaw;
	while (yaw < 0) yaw += 360.f;
	while (yaw >= 360) yaw -= 360.f;
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + getDirection(), glm::vec3{0.f, 1.f, 0.f});
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return glm::perspective(glm::radians(45.f), 4.f/3.f, .1f, 100.f);
}

glm::vec3 Camera::getDirection() const
{
	return {
		glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
		glm::sin(glm::radians(pitch)),
		glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))
	};
}
