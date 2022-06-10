#include "pch.h"
#include "Effect.h"
#include<sstream>
#include <d3dcompiler.h>
#include "d3dx11effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& path)
	/*:m_LightDirection{0.577f,-0.577f,0.577f}*/
{
	m_pEffect = LoadEffect(pDevice, path);
	if (!m_pEffect->IsValid())
	{
		std::wcout<< L"error reading hlsl file\n";
		return;
	}
	
	m_pTechniquePoint = m_pEffect->GetTechniqueByName("DefaultTechniquePoint");
	if (!m_pTechniquePoint->IsValid())
	{
		std::wcout << L"Technique Point not valid\n";
	}
	
	m_pTechniqueLinear = m_pEffect->GetTechniqueByName("DefaultTechniqueLinear");
	if (!m_pTechniqueLinear->IsValid())
	{
		std::wcout << L"Technique Linear not valid\n";
	}

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_EffectMatrixVariable is not valid\n";
	}
	// world matrix

	m_pWorldMatrix = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrix->IsValid())
	{
		std::wcout << L"m_pWorldMatrix is not valid\n";
	}
	// view inverse

	m_pViewInverseMatrix = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pViewInverseMatrix->IsValid())
	{
		std::wcout << L"m_pViewInverseMatrix is not valid\n";
	}

	// diffuse Map
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::cout << L"Variable gDiffuseMap not found\n";
	}
	// specular Map
	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
	{
		std::cout << L"Variable m_pSpecularMapVariable not found\n";
	}
	
	//gloss Map
	m_pGlossMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossMapVariable->IsValid())
	{
		std::cout << L"Variable m_pGlossMapVariable not found\n";
	}
	
	//normal Map
	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
	{
		std::cout << L"Variable m_pNormalMapVariable not found\n";
	}

	m_LightDirection = m_pEffect->GetVariableByName("gLightDirection")->AsVector();
	if (!m_LightDirection->IsValid())
	{
		std::cout << L"Variable m_LightDirection not found\n";
	}
}

Effect::~Effect()
{
	m_pEffect->Release();
	
}

  ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result = S_OK;
	
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderflags = 0;
#if defined(DEBUG)|| defined(_DEBUG)
	shaderflags |= D3DCOMPILE_DEBUG;
	shaderflags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 
	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderflags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);
	
	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];


			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader :Failed to CreateEffectFromFile!\nPath:" << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechniquePoint()
{
	return m_pTechniquePoint;
}

ID3DX11Effect* Effect::GetEffect()
{
	return m_pEffect;
}

void Effect::SetMatrix(Elite::FMatrix4 worldViewprojectionMatrix)
{
	//m_pMatWorldViewProjVariable->SetMatrix(&worldViewprojectionMatrix.data[0][0]);
	//m_pMatWorldViewProjVariable->SetMatrix(&worldViewprojectionMatrix.data[1][1]);
}

void Effect::SetDiffuseMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pDiffuseMapVariable->IsValid())
	{
		m_pDiffuseMapVariable->SetResource(pResourceView);
	}
}

void Effect::SetNormalMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pNormalMapVariable->IsValid())
	{
		m_pNormalMapVariable->SetResource(pResourceView);
	}
}

void Effect::SetGlossMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pGlossMapVariable->IsValid())
	{
		m_pGlossMapVariable->SetResource(pResourceView);
	}
}

void Effect::SetSpecularMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pSpecularMapVariable->IsValid())
	{
		m_pSpecularMapVariable->SetResource(pResourceView);
	}
}

ID3DX11EffectMatrixVariable* Effect::GetMatrix()
{
	return m_pMatWorldViewProjVariable;
}
