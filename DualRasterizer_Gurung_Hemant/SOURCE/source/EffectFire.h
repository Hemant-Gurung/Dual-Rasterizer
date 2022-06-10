#pragma once
#include "pch.h"
#include <string>
class EffectFire
{
public:
	EffectFire(ID3D11Device* pDevice, const std::wstring& path);
	~EffectFire();

	ID3DX11Effect* LoadEffectFire(ID3D11Device* pDevice, const std::wstring& assertFile);
	void SetDiffuseMap(ID3D11ShaderResourceView* pResourceView);
	ID3DX11Effect* GetEffectFire() { return m_pEffectFire; }
	ID3DX11EffectTechnique* GetTechnique() { return m_pTechnique; }
	ID3DX11EffectMatrixVariable* GetMatrix();
	
private:
	
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11Effect* m_pEffectFire;
};