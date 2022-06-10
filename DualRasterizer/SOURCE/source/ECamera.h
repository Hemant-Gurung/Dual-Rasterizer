/*=============================================================================*/
// Copyright 2021 Elite Engine 2.0
// Authors: Thomas Goussaert
/*=============================================================================*/
// ECamera.h: Base Camera Implementation with movement
/*=============================================================================*/

#pragma once
#include "EMath.h"

namespace Elite
{
	class Camera
	{
	public:

		Camera(const FPoint3& position = { 0.f, 0.f, 50.f }, const FVector3& viewForward = { 0.f, 0.f, -1.f }, float fovAngle = 45.f);
		~Camera() = default;

		Camera(const Camera&) = delete;
		Camera(Camera&&) noexcept = delete;
		Camera& operator=(const Camera&) = delete;
		Camera& operator=(Camera&&) noexcept = delete;

		void Update(float elapsedSec,float Swidth,float Sheight);

		const FMatrix4& GetWorldToViewSoftware() const { return m_WorldToViewSoftware; }
		const FMatrix4& GetWorldToViewHardware() const { return m_WorldToViewHardware; }
		const FMatrix4& GetViewToWorldHardware() const { return m_ViewToWorldHardware; }
		const FMatrix4& GetViewToWorldSoftware() const { return m_ViewToWorldSoftware; }
		const FMatrix4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const FMatrix4& GetProjectionMatrixRightHS() const { return m_ProjectionMatrixRightHS; }
		
		const float GetFov() const { return m_Fov; }
		void CalculateProjectionMatrix();
		void CalculateProjectionMatrixDirectX();
		void SetDirectXRasterizationActive(bool isActive);

		void CalculateLookAt();
		FPoint3 m_Position{};
		FMatrix4 m_WorldToViewSoftware{};
		FMatrix4 m_WorldToViewHardware{};
	private:
		
	
		float m_Fov{};
		float m_Fplane, m_Nplane;
		float m_AspectRatio;


		const float m_KeyboardMoveSensitivity{ 10.f };
		const float m_KeyboardMoveMultiplier{ 50.f };
		const float m_MouseRotationSensitivity{ .1f };
		const float m_MouseMoveSensitivity{ 2.f };
		
		FPoint2 m_AbsoluteRotation{}; //Pitch(x) & Yaw(y) only
		FPoint3 m_RelativeTranslation{};

		
		const FVector3 m_ViewForward{};

		
		
		FMatrix4 m_ProjectionMatrix;
		FMatrix4 m_ProjectionMatrixRightHS;
		Elite::FMatrix4 m_WorldMatrix;

		bool m_isSoftwareOn{};
		FMatrix4 m_ViewToWorldHardware{};
		FMatrix4 m_ViewToWorldSoftware{};
	};
}
