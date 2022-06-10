#include "pch.h"
#include "Box.h"

Box::Box(FPoint2 leftbottom, float width, float height)
	:m_width{width}
	,m_height{height}
	,m_RighBottom{}
	,m_topLeft{}
{

}

bool Box::PointInBox(std::vector<FPoint4> trianglePoints,const FVector2& widthHeight)
{
	//m_leftBottom = 
	float xMin = std::min(trianglePoints[0].x, std::min(trianglePoints[1].x, trianglePoints[2].x));
	float yMin = std::min(trianglePoints[0].y, std::min(trianglePoints[1].y, trianglePoints[2].y));
	float xMax = std::max(trianglePoints[0].x, std::max(trianglePoints[1].x, trianglePoints[2].x));
	float yMax = std::max(trianglePoints[0].y, std::max(trianglePoints[1].y, trianglePoints[2].y));

	//      0 >= xValues <= (width-1)  0 >= yValues <= (height-1)

	
	if (xMin < 0)xMin = 0;
	if (yMin < 0)yMin = 0;
	if (xMax > widthHeight.x) xMax = widthHeight.x - 1;
	if (yMax > widthHeight.y) yMax = widthHeight.y - 1;

	m_topLeft = { xMin,yMin };
	m_RighBottom = { xMax,yMax };
	return true;
}

FPoint2 Box::GetLeftBottom()
{
	return m_topLeft;
}

FPoint2 Box::GetRightBottom()
{
	return m_RighBottom;
}


void Box::Rotate(Elite::FPoint3 center)
{
}

