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

private:
	glm::vec3 position{2.f, 20.f, 3.f};
	glm::vec3 getDirection() const;
	float pitch{-45.f};
	float yaw{45.f};
};

#endif // SRC_OPENGL_CAMERA_H_
