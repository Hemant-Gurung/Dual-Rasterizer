#pragma once
#include "pch.h"
#include <string>

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& path);
	~Effect();

	ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assertFile);
	ID3DX11EffectTechnique* GetTechniquePoint();
	ID3DX11EffectTechnique* GetTechniqueLinear() { return m_pTechniqueLinear; }
	ID3DX11Effect* GetEffect();
	void SetMatrix(Elite::FMatrix4 worldViewprojMatrix);
	void SetDiffuseMap(ID3D11ShaderResourceView* pResourceView);
	void SetNormalMap(ID3D11ShaderResourceView* pResourceView);
	void SetGlossMap(ID3D11ShaderResourceView* pResourceView);
	void SetSpecularMap(ID3D11ShaderResourceView* pResourceView);
	ID3DX11EffectMatrixVariable* GetMatrix();
	ID3DX11EffectMatrixVariable* GetViewMatrix() { return m_pViewInverseMatrix; }
	ID3DX11EffectMatrixVariable* GetWorldMatrix() { return m_pWorldMatrix; }
	ID3DX11EffectVectorVariable* GetLightDirection() { return m_LightDirection; }
private:
	ID3DX11Effect* m_pEffect;

	// not released
	ID3DX11EffectTechnique* m_pTechniquePoint;
	ID3DX11EffectTechnique* m_pTechniqueLinear;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectMatrixVariable* m_pViewInverseMatrix;
	ID3DX11EffectMatrixVariable* m_pWorldMatrix;

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	
	ID3DX11EffectVectorVariable* m_LightDirection;
};