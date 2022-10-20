#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

class Camera
{
public:
	Camera(const glm::vec3& vPos, uint16_t uScreenWidth, uint16_t uSreenWidth, unsigned uProgramID);
	void MouseControl();
	void KeyboardControl(std::chrono::duration<float> tFrameTime);
	void ChangeProgramID();
	void SetPos(const glm::vec3& vNewPos);
	const glm::vec3& GetPos();

private:
	unsigned uProgId;
	glm::vec3 CameraPos;
	glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj;

	uint16_t uScreenWidth;
	uint16_t uScreenHeight;

	bool bFirst = true;

	uint16_t uLastX;
	uint16_t uLastY;
	int16_t iDeltaX;
	int16_t iDeltaY;
	float fYaw = 0;
	float fPitch = 0;
};

