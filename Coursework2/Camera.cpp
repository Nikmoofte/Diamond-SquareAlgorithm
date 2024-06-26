#include "Camera.h"
#include "glad/glad.h"
#include <chrono>
#include <windows.h>


Camera::Camera(const glm::vec3& vPos, uint16_t uScreenWidth, uint16_t uScreenHeight)
{
	CameraPos = vPos;
	this->uScreenHeight = uScreenHeight;
	this->uScreenWidth = uScreenWidth;
	proj = glm::perspective(glm::radians(90.f), static_cast<float>(uScreenWidth) / uScreenHeight, 0.01f, 10000.0f);

	view = lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
}

void Camera::MouseControl()
{
	using namespace glm;

	POINT CursorPos;
	GetCursorPos(&CursorPos);

	if (bFirst || (CursorPos.x > uScreenWidth - 100) || (CursorPos.x < 100))
	{
		SetCursorPos(uScreenWidth / 2, CursorPos.y);
		uLastX = uScreenWidth / 2;
		uLastY = CursorPos.y;
		bFirst = false;
	}

	int iDeltaX = CursorPos.x - uLastX;
	int iDeltaY = uLastY - CursorPos.y;
	uLastX = CursorPos.x;
	uLastY = CursorPos.y;


	float fSensivity = 0.005f;
	fYaw += iDeltaX * fSensivity;
	fPitch += iDeltaY * fSensivity;

	if (fPitch > pi<float>() / 4)
		fPitch = pi<float>() / 4;
	if (fPitch < -pi<float>() / 4)
		fPitch = -pi<float>() / 4;

	if (fYaw > 2 * pi<float>())
		fYaw -= 2 * pi<float>();
	if (fYaw < 2 * pi<float>())
		fYaw += 2 * pi<float>();

	vec3 direction;
	direction.x = cos(fYaw) * cos(fPitch);
	direction.y = sin(fPitch);
	direction.z = sin(fYaw) * cos(fPitch);

	CameraFront = normalize(direction);
	view = lookAt(CameraPos, CameraPos + CameraFront, CameraUp);

}

const glm::vec3& Camera::GetPos()
{
	// TODO: �������� ����� �������� return
	return glm::vec3(0);
}

glm::mat4& Camera::GetProjMat()
{
	return proj;
}

glm::mat4& Camera::GetViewMat()
{
	return view;
}

void Camera::KeyboardControl(std::chrono::duration<float> tFrameTime)
{

	float fSpeed = 100.f;

	if (GetKeyState(VK_SHIFT) < 0)
		fSpeed = 1000.0f;

	float fMovSpeed = fSpeed * tFrameTime.count();

	if (GetKeyState(VK_SPACE) < 0)
	{
		CameraPos += fMovSpeed * CameraUp;
	}
	if (GetKeyState(VK_CONTROL) < 0)
	{
		CameraPos -= fMovSpeed * CameraUp;
	}
	
	if (GetKeyState('W') < 0)
	{
		CameraPos += fMovSpeed * CameraFront;
	}
	if (GetKeyState('S') < 0)
	{
		CameraPos -= fMovSpeed * CameraFront;
	}

	if (GetKeyState('D') < 0)
	{
		glm::vec3 right = normalize(cross(CameraFront, CameraUp));

		CameraPos += fMovSpeed * right;
	}
	if (GetKeyState('A') < 0)
	{
		glm::vec3 right = normalize(cross(CameraFront, CameraUp));
		CameraPos -= fMovSpeed * right;
	}
}
