#pragma once

#include "define_types.h"


enum struct CameraMovement : int
{	
	FORWARD = 0,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YawDef = -90.f;
const float PitchDef = 0.f;
const float SpeedDef = 2.5f;
const float SensitivityDef = 0.1f;
const float ZoomDef = 45.f;

class Camera 
{
public:
	using Vector3f = glm::vec3;
	

	Camera(Vector3f pos = Vector3f(0.f, 0.f, 0.f), Vector3f up = Vector3f(0.f, 1.0f, 0.f), float yaw = YawDef, float pitch = PitchDef);

	Camera(float px, float py, float pz, float upX, float upY, float upZ, float yaw, float pitch);

	~Camera();

	Vector3f Position;
	Vector3f Front;
	Vector3f Up;
	Vector3f Right;
	Vector3f WorldUp;

	float Yaw;
	float Pitch;
	float Zoom;

	float MovementSpeed;
	float MouseSensitivity;

	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(CameraMovement dir, float dt);
	void ProcessMouseMovement(float xoffset, float yoffset, bool contrainPitch = true);
	void ProcessMouseScroll(float yoffset);

private:
	void updateCameraVectors();
};

