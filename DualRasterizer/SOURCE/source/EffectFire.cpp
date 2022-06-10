#include "pch.h"
#include "EffectFire.h"
#include<sstream>

EffectFire::EffectFire(ID3D11Device* pDevice, const std::wstring& path)
{
	m_pEffectFire = LoadEffectFire(pDevice, path);
	if (!m_pEffectFire->IsValid())
	{
		std::wcout << L"error reading hlsl file\n";
		return;
	}

	m_pTechnique = m_pEffectFire->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique Point not valid\n";
	}

	// diffuse Map
	m_pDiffuseMapVariable = m_pEffectFire->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::cout << L"Variable gDiffuseMap not found\n";
	}

	m_pMatWorldViewProjVariable = m_pEffectFire->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_EffectMatrixVariable is not valid\n";
	}
}

EffectFire::~EffectFire()
{

	m_pDiffuseMapVariable->Release();
	
	m_pEffectFire->Release();
}

ID3DX11Effect* EffectFire::LoadEffectFire(ID3D11Device* pDevice, const std::wstring& assertFile)
{
	HRESULT result = S_OK;

	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderflags = 0;
#if defined(DEBUG)|| defined(_DEBUG)
	shaderflags |= D3DCOMPILE_DEBUG;
	shaderflags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 
	result = D3DX11CompileEffectFromFile(assertFile.c_str(),
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
			ss << "EffectLoader :Failed to CreateEffectFromFile!\nPath:" << assertFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

void EffectFire::SetDiffuseMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pDiffuseMapVariable->IsValid())
	{
		m_pDiffuseMapVariable->SetResource(pResourceView);
	}
}

ID3DX11EffectMatrixVariable* EffectFire::GetMatrix()
{
	return m_pMatWorldViewProjVariable;
}

