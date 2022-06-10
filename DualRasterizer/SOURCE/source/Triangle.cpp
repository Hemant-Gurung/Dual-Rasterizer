#include "pch.h"
#include "Triangle.h"
#include <iostream>

Triangle::Triangle(const Vertex& tp1, const Vertex& tp2, const Vertex& tp3)
	:m_tp1{ tp1.Position }
	, m_tp2{ tp2.Position }
	, m_tp3{ tp3.Position }
	, m_uv1{ tp1.UV }
	, m_uv2{ tp2.UV }
	, m_uv3{ tp3.UV }
	, m_Normal1{ tp1.Normal }
	, m_Normal2{ tp2.Normal }
	, m_Normal3{ tp3.Normal }
	,m_Tangent1{tp1.Tangent}
	,m_Tangent2{tp2.Tangent }
	,m_Tangent3{tp3.Tangent }
	, m_IsInsideTriangle{ false }
	, m_weight1{}
	, m_weight2{}
	, m_weight3{}
	,m_rotation{}
	,m_Rotate{false}
{
	{
		m_Vertices.push_back(m_tp1);
		m_Vertices.push_back(m_tp2);
		m_Vertices.push_back(m_tp3);
	}

}

bool Triangle::PointInTriangle(const FPoint2& P, float& m_depthBuffer, RGBColor& finalcolor,  Vertex& vertex,FMatrix4 worldMatrix)
{
	//m_Normal =   GetNormalized(Cross(FVector3(m_tp1 - m_tp3), FVector3(m_tp1 - m_tp2)));
	m_Center = { (m_tp1.x + m_tp2.x + m_tp3.x) / 3,(m_tp1.y + m_tp2.y + m_tp3.y) / 3,(m_tp1.z + m_tp2.z + m_tp3.z) / 3 };
	
	// vertices[0].pos.xy
	Elite::FVector2 ca = m_tp1.xy - m_tp3.xy;
	Elite::FVector2 cb = m_tp1.xy - m_tp2.xy;

	// total area
	float area = Cross(ca, cb);
	
	// calculate weights
	m_weight1 = ((Cross(P - m_tp2.xy, m_tp3.xy - m_tp2.xy))) / (area);
	m_weight2 = ((Cross(P - m_tp3.xy, m_tp1.xy - m_tp3.xy))) / (area);
	m_weight3 = ((Cross(P - m_tp1.xy, m_tp2.xy - m_tp1.xy))) / (area);

	if (m_weight1 >= 0 && m_weight2 >= 0 && m_weight3 >= 0)
	{
		float currentDepth = 1 / ((1 / m_tp1.z) * m_weight1 +
			(1 / m_tp2.z) * m_weight2 +
			(1 / m_tp3.z) * m_weight3);

		if (currentDepth >= 0.f && currentDepth <= 1.f)
		{
			if (currentDepth < m_depthBuffer)
			{
				// w interpolated 
				float wInterpolated = 1 / ((1 / m_tp1.w) * m_weight1 +
					(1 / m_tp2.w) * m_weight2 +
					(1 / m_tp3.w) * m_weight3);

				m_depthBuffer = currentDepth;

				//vertex position
				vertex.Position = { P.x,P.y,m_depthBuffer,wInterpolated };

				//world position
				vertex.WorldPosition = worldMatrix * vertex.Position;
				// calculate final uv
				m_FinalUV = ((m_uv1 / m_tp1.w) * m_weight1+
							(m_uv2 / m_tp2.w) * m_weight2 +
							(m_uv3 / m_tp3.w) * m_weight3 ) * wInterpolated;

				//vertex normal
				vertex.UV  = m_FinalUV;
				// calculate final color
				/*finalcolor = ((m_Color0 / m_tp1.w) * m_weight1 +
							 (m_Color1 / m_tp2.w) * m_weight2 +
					         (m_Color2 / m_tp3.w) * m_weight3) * wInterpolated;*/

				
				m_InterpolatedNormal = ((m_Normal1 ) * m_weight1 +
										(m_Normal2 ) * m_weight2 +
										(m_Normal3 ) * m_weight3)/3 ;

				m_Tangent1 = FVector3(m_Tangent1.xy,-m_Tangent1.z);
				m_Tangent2 = FVector3(m_Tangent2.xy, -m_Tangent2.z);
				m_Tangent3 = FVector3(m_Tangent3.xy, -m_Tangent3.z);
				m_InterpolatedTangent = ((m_Tangent1)*m_weight1 +
										 (m_Tangent2)*m_weight2 +
										 (m_Tangent3)*m_weight3);
				//add interpolated tangent to the vertex
				vertex.Tangent = m_InterpolatedTangent;

				Elite::Normalize(m_InterpolatedNormal);
				//vertex normal
				vertex.Normal = m_InterpolatedNormal;
						
				return true;
			}
		}
	}
	return false;
}

void Triangle::Rotate(const float speed, const Elite::FVector3 axis)
{
}

std::vector<FPoint4> Triangle::GetVertices()
{
	return m_Vertices;
}



void Triangle::VertexTransformationFunction(std::vector<FPoint4> originalVertices, std::vector<FPoint4>& m_transformedVert, const Elite::FMatrix4& onb, const Elite::FMatrix4& projectionMat, uint32_t width, uint32_t height, float fov)
{
	float AspectRatio = (float)width / (float)height;
	float farP = 100.f;
	float nearP = 0.1f;

	float A{ -farP / (farP - nearP) };
	float B{ -(farP * nearP) / (farP - nearP) };

	//Rotation
	const float dt = 0.001f;
	Elite::FMatrix4 rotationMatrix = Elite::MakeRotation(m_rotation, Elite::FVector3{ 0,1,0 });
	
	FMatrix4 mat = projectionMat* onb* rotationMatrix;

	for (FPoint4& vertex : originalVertices)
	{
		// to view space
		FPoint4 viewPoint = mat * vertex;
		// projection space
		vertex.x = viewPoint.x / (viewPoint.w);
		vertex.y = viewPoint.y / (viewPoint.w);
		vertex.z = viewPoint.z / viewPoint.w;   // depth of triangle
		vertex.w = viewPoint.w;
	}

	m_transformedVert.resize(3);
	if (originalVertices[0].x > -1 && originalVertices[0].x < 1)
	{
		float ssVertex0X = ((originalVertices[0].x + 1) / 2) * width;
		float ssVertex0Y = ((1 - originalVertices[0].y) / 2) * height;
		m_tp1 = { ssVertex0X,ssVertex0Y,originalVertices[0].z,originalVertices[0].w };
		m_transformedVert[0] = (m_tp1);
	}

	if (originalVertices[1].x > -1 && originalVertices[1].x < 1)
	{
		float ssVertex1X = ((originalVertices[1].x + 1) / 2) * width;
		float ssVertex1Y = ((1 - originalVertices[1].y) / 2) * height;
		m_tp2 = { ssVertex1X,ssVertex1Y,originalVertices[1].z,originalVertices[1].w };
		m_transformedVert[1] = (m_tp2);
	}
	if (originalVertices[2].x > -1 && originalVertices[2].x < 1)
	{
		float ssVertex2X = ((originalVertices[2].x + 1) / 2) * width;
		float ssVertex2Y = ((1 - originalVertices[2].y) / 2) * height;
		m_tp3 = { ssVertex2X,ssVertex2Y,originalVertices[2].z,originalVertices[2].w };
		m_transformedVert[2] = (m_tp3);
	}
}



float Triangle::GetWeight0() const
{
	return m_weight1;
}

float Triangle::GetWeight1() const
{
	return m_weight2;
}

float Triangle::GetWeight2() const
{
	return m_weight3;
}

FPoint4 Triangle::GetWorldPosition()
{
	return FPoint4{};
}


FVector2 Triangle::GetUV()
{
	return m_FinalUV;
}

FVector2 Triangle::GetUv1()
{
	return m_uv1;
}

FVector2 Triangle::GetUv2()
{
	return m_uv2;
}

FVector2 Triangle::GetUv3()
{
	return m_uv3;
}

void Triangle::update(float elapsed)
{
	
	m_rotation += elapsed;
}

