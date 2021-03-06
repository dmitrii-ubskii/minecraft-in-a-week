#ifndef SRC_OPENGL_CAMERA_H_
#define SRC_OPENGL_CAMERA_H_

#include <glm/glm.hpp>

class Camera
{
public:
	void rotate(float deltaPitch, float deltaYaw);
	void move(float along, float side);

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	glm::vec3 getPosition() const { return position; }
	glm::vec3 getDirection() const;

private:
	glm::vec3 position{2.f, 100.f, 3.f};
	float pitch{-45.f};
	float yaw{45.f};
};

#endif // SRC_OPENGL_CAMERA_H_
