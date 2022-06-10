#pragma once
#include <string>
#include "Emath.h"
#include "ERGBColor.h"
#include "SDL.h"
class Texture
{
public:

	Texture(ID3D11Device* pDevice,const char* path);
	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&&) noexcept = delete;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&&) noexcept = delete;

	
	
	SDL_Surface& GetSdlSurface();
	Elite::RGBColor Sample(const Elite::FVector2& uv);
	ID3D11ShaderResourceView* GetResourceView() { return m_pResourceView; }
private:
	
	SDL_Surface* m_pSurface;
	const char* m_path;
	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_pResourceView;
};