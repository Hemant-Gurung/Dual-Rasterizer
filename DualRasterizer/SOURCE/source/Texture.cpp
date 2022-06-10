#include "pch.h"
#include <SDL_image.h>
#include "Texture.h"
#include <iostream>

Texture::Texture(ID3D11Device* pDevice, const char* path)
	:m_path{path}
{
	
	   m_pSurface = IMG_Load(path);
		if (m_pSurface == nullptr)
		{
			std::cout << "Error Reading File\n";
			return;
		}

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = m_pSurface->w;
		desc.Height = m_pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = m_pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);

		DXGI_FORMAT format{};
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pResourceView);

}

Texture::~Texture()
{
	SDL_FreeSurface(m_pSurface);
	m_pTexture->Release();
	m_pResourceView->Release();
	//delete m_Image;

}

SDL_Surface& Texture::GetSdlSurface()
{
	// TODO: insert return statement here
	return *m_pSurface;
}

Elite::RGBColor Texture::Sample(const Elite::FVector2& uv)
{
	
	uint32_t uvX =(uv.x)*(m_pSurface->w);
	uint32_t uvY = (uv.y) * (m_pSurface->h);
	
	
	uint32_t *pPixels = (uint32_t*)m_pSurface->pixels ;
	uint32_t pixelData = uvX+(uvY* m_pSurface->clip_rect.h);

	uint8_t r ;
	uint8_t g ;
	uint8_t b ;
	if (uvY <= m_pSurface->h && uvX <= m_pSurface->w)
	{
		SDL_GetRGB(pPixels[uvX + (uvY * m_pSurface->h)], m_pSurface->format, &r, &g, &b);
	}
	return Elite::RGBColor(r/255.f,g / 255.f,b / 255.f);
	//=====================================================>
}
