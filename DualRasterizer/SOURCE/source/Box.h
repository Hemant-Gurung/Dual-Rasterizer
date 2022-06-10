#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include <vector>
using namespace Elite;

class Box
{
public:

	Box(FPoint2 leftbottom,float width,float height);
	~Box() = default;

	Box(const Box&) = delete;
	Box(Box&&) noexcept = delete;
	Box& operator=(const Box&) = delete;
	Box& operator=(Box&&) noexcept = delete;


	//bool Hit(const Ray& ray, HitRecord& hitrecord);
	bool PointInBox(std::vector<FPoint4> trianglePoints, const FVector2& widthHeight);
	FPoint2 GetLeftBottom();
	FPoint2 GetRightBottom();

	void Rotate(Elite::FPoint3 center);
	//void VertexTransformationFunction(std::vector<FPoint4> m_originalVertices);
	FPoint2 m_topLeft{};
	FPoint2 m_RighBottom{};
private:
	

	float m_width;
	float m_height;

};