#pragma once
#include "EMath.h"
class Camera
{
public:

	enum class keys
	{
		none,
		left,
		right,
		up,
		down,
	};
	Camera(Elite::FPoint3 position, float FOV);
	~Camera() = default;

	Camera(const Camera&) = delete;
	Camera(Camera&&) noexcept = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&&) noexcept = delete;

	float m_Scale{};
	keys m_Key{ keys::none };
	Elite::FPoint3 m_Position;
	float m_Fov;
	
	
	Elite::FMatrix4 CameraLookAt();
	void GetRelativeMouseState();
	float m_MousePosX{}, m_MousePosY{};

	void SetFov(float value);
	

	
private:
	
	float m_Sensitivity;
	Elite::FVector3 m_Forward;
	Elite::FVector3 m_Right;
	Elite::FVector3 m_Up;
};