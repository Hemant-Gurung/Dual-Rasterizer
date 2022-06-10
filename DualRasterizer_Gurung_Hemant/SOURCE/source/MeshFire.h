#pragma once
#include "EffectFire.h"
#include <vector>
#include "Mesh.h"

class MeshFire
{
public:
	MeshFire(ID3D11Device* pDevice, std::vector<VS_INPUT> vertices, std::vector<int> index);
	~MeshFire();
	void Render(ID3D11DeviceContext* pDeviceContext, Elite::Camera* pCamera, ID3D11ShaderResourceView* pDiffuseMapResource);
	void SetRotate();
	void update(float elapsedSec);
private:
	EffectFire* m_pEffectFire;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexxBuffer;
	int m_AmoutIndices{};
	float m_rotation;
	bool m_Rotate;
};
