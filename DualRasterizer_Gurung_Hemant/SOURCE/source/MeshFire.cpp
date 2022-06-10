#include "pch.h"

#include "MeshFire.h"
#include "EffectFire.h"

MeshFire::MeshFire(ID3D11Device* pDevice, std::vector<VS_INPUT> vertices, std::vector<int> indices)
	:m_Rotate{false}
	,m_rotation{}
{
	std::wstring pathFire = L"../../BUILD/Resources/PosCol3D_2.fx";
	m_pEffectFire = new EffectFire(pDevice, pathFire);

	HRESULT result = S_OK;
	static const uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "WPOS";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_pEffectFire->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout
	(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout
	);

	if (FAILED(result))
		return;


	//Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(VS_INPUT) * (uint32_t)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = vertices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;


	//create input buffer
	m_AmoutIndices = (uint32_t)indices.size();
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmoutIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexxBuffer);
	if (FAILED(result))
		return;
}

MeshFire::~MeshFire()
{
	delete m_pEffectFire;
	m_pVertexLayout->Release();
	m_pIndexxBuffer->Release();
	m_pVertexBuffer->Release();
}

void MeshFire::Render(ID3D11DeviceContext* pDeviceContext, Elite::Camera* pCamera,ID3D11ShaderResourceView* pDiffuseMapResource)
{

	// set DiffuseMapresources
	m_pEffectFire->SetDiffuseMap(pDiffuseMapResource);

	UINT stride = sizeof(VS_INPUT);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	const float dt = 0.001f;
	Elite::FMatrix4 rotationMatrix = Elite::MakeRotation(m_rotation, Elite::FVector3{ 0,1,0 });
	Elite::FMatrix4 worldViewProjMat{};
	
	worldViewProjMat = pCamera->GetProjectionMatrix() * pCamera->GetWorldToViewHardware() * rotationMatrix;
	
	// get matrix
	//auto worldViewProjMat = pCamera->GetProjectionMatrix() * pCamera->GetWorldToViewHardware();
	m_pEffectFire->GetMatrix()->SetMatrix(*worldViewProjMat.data);
	//set indexbuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexxBuffer, DXGI_FORMAT_R32_UINT, 0);

	//set input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	//Set the input layout
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	////Render triangle
	D3DX11_TECHNIQUE_DESC teschDesc;
	m_pEffectFire->GetTechnique()->GetDesc(&teschDesc);
	for (UINT p = 0; p < teschDesc.Passes; ++p)
	{
		m_pEffectFire->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmoutIndices, 0, 0);
	}

}

void MeshFire::SetRotate()
{
	if (m_Rotate == true)
	{
		m_Rotate = false;
	}
	else
	{
		m_Rotate = true;
	}
}

void MeshFire::update(float elapsedSec)
{
	if (m_Rotate)
	{
		m_rotation += elapsedSec;
	}
}
