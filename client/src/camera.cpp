#include "camera.h"




Camera::Camera(Vector3f pos /*= Vector3f(0.f, 0.f, 0.f)*/, Vector3f up /*= Vector3f(0.f, 1.0f, 0.f)*/, float yaw /*= YawDef*/, float pitch /*= PitchDef*/)
	:Front(Vector3f(0.f,0.f,-1.f)),
	MovementSpeed(SpeedDef),
	MouseSensitivity(SensitivityDef),
	Zoom(ZoomDef)
{
	Position = pos;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

Camera::Camera(float px, float py, float pz, float upX, float upY, float upZ, float yaw, float pitch):
	Front(Vector3f(0.f,0.f,-1.f)),
	MovementSpeed(SpeedDef),
	MouseSensitivity(SensitivityDef),
	Zoom(ZoomDef)
{
	Position = Vector3f(px, py, pz);
	WorldUp = Vector3f(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}



Camera::~Camera()
{

}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(CameraMovement dir, float dt)
{
	float velocity = MovementSpeed * dt;
	switch (dir)
	{
	case CameraMovement::FORWARD:
		Position += Front * velocity;
		break;
	case CameraMovement::BACKWARD:
		Position -= Front * velocity;
		break;
	case CameraMovement::LEFT:
		Position -= Right * velocity;
		break;
	case CameraMovement::RIGHT:
		Position += Right * velocity;
		break;
	default:
		break;
	}
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool contrainPitch /*= true*/)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (contrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.f;
		if (Pitch < -89.f)
			Pitch = -89.f;
	}
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	Vector3f front;
	front.x = std::cos(glm::radians(Yaw) * std::cos(glm::radians(Pitch)));
	front.y = std::sin(glm::radians(Pitch));
	front.z = std::sin(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}
