#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include <vector>
#include "Mesh.h"

using namespace Elite;


struct Vertex
{
	FPoint4 Position = {};
	FPoint4 WorldPosition = {};
	RGBColor color = {};
	FVector2 UV = {}; 
	FVector3 Normal{};
	FVector3 Tangent{};
};


class Triangle
{
public:
	Triangle(const  Vertex& tp1, const  Vertex& tp2, const  Vertex& tp3);
	~Triangle() = default;

	Triangle(const Triangle&) = delete;
	Triangle(Triangle&&) noexcept = delete;
	Triangle& operator=(const Triangle&) = delete;
	Triangle& operator=(Triangle&&) noexcept = delete;


	//bool Hit(const Ray& ray, HitRecord& hitrecord);
	bool PointInTriangle(const FPoint2& P, float& m_depthBuffer, RGBColor& finalcolor,Vertex& vertex, FMatrix4 worldMatrix);
	void Rotate(const float speed =3.f, const Elite::FVector3 axis = { 0,1,0 });
	std::vector<FPoint4> GetVertices();

	void VertexTransformationFunction(std::vector<FPoint4> m_originalVertices,std::vector<FPoint4>& m_transformedVert,const Elite::FMatrix4& onb, const Elite::FMatrix4& projectionMat, uint32_t width,uint32_t height,float fov);
	float GetWeight0() const;
	float GetWeight1() const;
	float GetWeight2() const;

	FPoint4 GetWorldPosition();
	
	FVector2 GetUV();
	FVector2 GetUv1();
	FVector2 GetUv2();
	FVector2 GetUv3();

	//void switchMode();
	void update(float elapsed);
	void SetRotate();
	// object
	//void RotateObject(Elite::FPoint3 center);
	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	PrimitiveTopology m_PrimitiveTopology{ PrimitiveTopology::TriangleList };
	FVector2 m_uv1;
	FVector2 m_uv2;
	FVector2 m_uv3;

	FPoint3 m_Center;

	FVector3 m_Normal1;
	FVector3 m_Normal2;
	FVector3 m_Normal3;

	FVector3 m_Tangent1;
	FVector3 m_Tangent2;
	FVector3 m_Tangent3;

	FVector3 m_InterpolatedNormal{};
	FVector3 m_InterpolatedTangent{};

	// toggle rasterizers
	bool g_toggleRasterizer{ false };

protected:
	FPoint4 m_tp1;
	FPoint4 m_tp2;
	FPoint4 m_tp3;

	/*RGBColor m_Color0;
	RGBColor m_Color1;
	RGBColor m_Color2;*/

	FVector3 m_Normal;
	RGBColor m_Color;
	
	float m_weight1;
	float m_weight2;
	float m_weight3;

	bool m_IsInsideTriangle;
	float m_rotation;

	
	FVector2 m_FinalUV;

	//Fpoin m_Vert[3];
	std::vector<FPoint4> m_Vertices;
	
	enum class faces
	{
		backface,
		frontface,
		both,
	};
	faces m_face{ faces::frontface };
	

	Vertex v1;
	Vertex v2;
	Vertex v3;
	
	Triangle* m_pTriangle;
	bool m_Rotate;
	
};

