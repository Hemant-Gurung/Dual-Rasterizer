/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include "Triangle.h"
#include <vector>
#include "Texture.h"
#include "ECamera.h"
#include "MeshFire.h"
#include "Box.h"
#include "Mesh.h"
struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		bool SaveBackbufferToImage() const;

		void RenderDirectX11();
		void RenderSoftware();
		void Update(float elapsedSec);
		// toggle rasterizer
		bool m_toggleRasterizer;
	private:
		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;

		bool m_IsInitialized;

		void AddObjects();
		//Box* m_pBox;
		std::unique_ptr<Box> m_pBox;
		bool SetSoftwareRenderer{ false };

		//software Rasterizer variables
		std::vector<Triangle*> m_pTriangles;
		std::vector<float> m_depthBuffer;
		std::vector<FPoint4>m_vertices;
		std::vector<FPoint4>m_Transformedvertices;
		std::vector<FPoint3> m_VertexBuffer;
		std::vector<int> m_IndexBufferList;
		std::vector<Vertex> m_VertexBufferStruct;
		FVector3 m_ViewDirection{};
		FVector4 m_LightDirection;
		RGBColor m_SpecularRef{};
		RGBColor m_pGlossiness{};

		void MakeTriangle();
		void CreateTangent();
		const RGBColor& PixelShading(const Vertex& transformedVertex, const FVector3& viewDirection);
		void Rotate();
		//ob parse
		std::vector<VS_INPUT> m_VerticesObj{};
		std::vector<Vertex> m_VerticesObjSoftware{};
		std::vector<int> m_Indices{};
		bool ParseOBJ(const std::string& filename, std::vector<VS_INPUT>& vertices, std::vector<int>& indices);
		// Texture

		std::unique_ptr<Texture> m_pDiffuseMap;
		std::unique_ptr<Texture> m_NormalTexture;
		std::unique_ptr<Texture> m_pGlossinessMap;
		std::unique_ptr<Texture> m_pSpecularMap;
		std::unique_ptr<Texture> m_pDiffuseMapFire;

		SDL_Surface m_pSurfaceTexture;
		//Texture* m_pDiffuseMap;
		//Texture* m_NormalTexture;
		//Texture* m_pGlossinessMap = nullptr;
		//Texture* m_pSpecularMap = nullptr;
		//Texture* m_pDiffuseMapFire;

		float m_ObservedArea{};
		FVector3 m_Tangent{};
		RGBColor m_Finalcolor{ 0,0,0 };
		RGBColor m_DiffuseMapColor{};
		FVector3 m_Binormal{};
		
		//DirectX11 Rasterizer variables ----------------------->
		HRESULT InitializeDirectX();
		//Device and DeviceContext initialization
		ID3D11Device* m_pDevice;   //The device interface represents a virtual adapter; it is used to create resources.

		//The ID3D11DeviceContext interface represents a device context which generates rendering commands.
		ID3D11DeviceContext* m_pDeviceContext;  //Returns the address of a pointer to an ID3D11DeviceContext object that represents the device context
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;

		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;

		// rendet target buffer and view
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		Mesh* m_pMesh;
		MeshFire* m_pMeshFire;

		// Fire variables initializer
		std::vector<VS_INPUT> m_FireVertices;
		std::vector<int> m_FireIndices;
		
		//Camera
		Camera* m_pCamera;
		float m_Fov;
		bool m_ShowTail{};
		bool m_RotateSoftWareMesh;
		float m_ElapsedSec;
		// keyboard inputs
		void GetKeyboardInputs(float elaspedSec);
		float m_elapsedSecCounter;
		float rasterizerCounter;
		float m_rotation;
	};
}

#endif