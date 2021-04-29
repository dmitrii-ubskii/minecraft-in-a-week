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

private:
	glm::vec3 position{2.f, 0.f, 3.f};
	glm::vec3 getDirection() const;
	float pitch{0};
	float yaw{-135.f};
};

#endif // SRC_OPENGL_CAMERA_H_
