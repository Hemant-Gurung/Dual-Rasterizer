#pragma once
#include "pch.h"
#include "EMath.h"
#include "ERGBColor.h"
#include <vector>
#include "Effect.h"
#include <memory>
#include "ECamera.h"
//#include "EffectFire.h"
struct VS_INPUT
{
	Elite::FPoint3 Position;
	Elite::FPoint2 UV;
	Elite::FVector3 Normal;
	Elite::FVector3 Tangent;
	Elite::FPoint4 worldPos;
	
};

struct VS_OUTPUT
{
	Elite::FPoint4 Position;
	Elite::FPoint2 UV;
	Elite::FVector3 Normal;
	Elite::FVector3 Tangent;
	Elite::FPoint4 worldPos;
};

class Mesh
{

public:
	Mesh(ID3D11Device* pDevice,std::vector<VS_INPUT> vertices,std::vector<int> index);
	~Mesh();
	

	Effect* m_pEffect;
	
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexxBuffer;
	int m_AmoutIndices{};

	void Render(
		ID3D11DeviceContext* pDviceContext,
		Elite::Camera* pCamera,
		ID3D11ShaderResourceView* pDiffuseMapResource,
		ID3D11ShaderResourceView* pNormalMapResource,
		ID3D11ShaderResourceView* pSpecularMapResource,
		ID3D11ShaderResourceView* pGlossMapResource
	);
	void update(float elapsedSec);
	void SetRotate(); 
private:
	int m_pToggleSampleState{};
	float m_rotation{};
	bool m_Rotate{};
};
