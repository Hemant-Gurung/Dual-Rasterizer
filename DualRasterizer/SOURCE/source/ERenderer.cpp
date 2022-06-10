#include "pch.h"
//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include <fstream>
//Project includes
#include "ERenderer.h"


Elite::Renderer::Renderer(SDL_Window * pWindow)
	: m_pWindow{ pWindow }
	, m_Width{}
	, m_Height{}
	, m_IsInitialized{ false }
	,m_ShowTail{true}
	,rasterizerCounter{}
	,m_RotateSoftWareMesh{false}
{
	int width, height = 0;
	m_pWindow = pWindow;
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
	m_LightDirection = { 0.577f,-0.577f,-0.577f,0.f };
	// initialize toggleRasterizer
	m_toggleRasterizer = false;
	//camera
	m_pCamera = new Camera();
	// initialize DirectX
	InitializeDirectX();
	// parse vertices of triangles
	AddObjects();
	//initialize depth buffer
	m_depthBuffer.resize(static_cast<uint64_t>(m_Width)* static_cast<uint64_t> (m_Height));

	//Initialize DirectX pipeline
	//...
	m_IsInitialized = true;
	std::cout << "DirectX is ready\n";
	std::cout << "initializing mesh..\n";
	m_pMesh = new Mesh(m_pDevice, m_VerticesObj, m_Indices);
	std::cout << "initializing mesh..Complete\n";
	m_pMeshFire = new MeshFire(m_pDevice, m_FireVertices, m_FireIndices);
	
}

Elite::Renderer::~Renderer()
{
	
	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();

		m_pDXGIFactory->Release();
		m_pDevice->Release();
		m_pDeviceContext->Release();
		m_pSwapChain->Release();
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilView->Release();
		m_pRenderTargetBuffer->Release();
		m_pRenderTargetView->Release();

		delete m_pCamera;
		m_pCamera = nullptr;
	
		delete m_pMeshFire;
		m_pMeshFire = nullptr;
		delete m_pMesh;
		m_pMesh = nullptr;
	}

	
	for (auto i = 0; i < m_pTriangles.size(); i++)
	{
		delete m_pTriangles[i];
		m_pTriangles[i] = nullptr;
	}

}


void Elite::Renderer::RenderDirectX11()
{
	
	if (!m_IsInitialized) 
		return;
	//Clear buffer
	RGBColor clearColor = RGBColor(0.3f, 0.3f, 0.3f);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// call projection matrix
	m_pCamera->Update(m_ElapsedSec, static_cast<float>(m_Width),static_cast<float>(m_Height));
	//call render for mesh
	m_pMesh->Render(
		m_pDeviceContext,
		m_pCamera,
		m_pDiffuseMap.get()->GetResourceView(),
		m_NormalTexture.get()->GetResourceView(),
		m_pSpecularMap.get()->GetResourceView(),
		m_pGlossinessMap.get()->GetResourceView()
	);
	if (m_ShowTail)
	{
		m_pMeshFire->Render(
			m_pDeviceContext,
			m_pCamera,
			m_pDiffuseMapFire.get()->GetResourceView()
		);
	}
	// present the render
	m_pSwapChain->Present(0, 0);
}

void Elite::Renderer::RenderSoftware()
{
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			m_depthBuffer[static_cast<uint64_t>(c) + static_cast<uint64_t>((r) * static_cast<uint64_t>(m_Width))] = FLT_MAX;
			m_pBackBufferPixels[static_cast<uint32_t>(c) + (static_cast<uint32_t>(r) * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(0.3f * 255.f),
				static_cast<uint8_t>(0.3f * 255.f),
				static_cast<uint8_t>(0.3f * 255.f));
		}
	}

	 
	SDL_LockSurface(m_pBackBuffer);
	RGBColor finalColor{ 0.3f,0.3f,0.3f };

	//Loop over all the pixels]
	m_Fov = m_pCamera->GetFov();

	//Elite::FMatrix4 InvOnb = m_pCamera->CameraLookAt();
	Elite::FMatrix4 InvOnb = m_pCamera->GetWorldToViewSoftware();

	for (Triangle* triangle : m_pTriangles)
	{

		//get vertices from triangle to update
		m_vertices = triangle->GetVertices();

		//perform vertex transformation
		triangle->VertexTransformationFunction(m_vertices, m_Transformedvertices, InvOnb,m_pCamera->GetProjectionMatrixRightHS(), m_Width, m_Height, m_Fov);
		
		// create bounding box for the triangle
		m_pBox.get()->PointInBox(m_Transformedvertices, FVector2{ (float)m_Width,(float)m_Height });

		// pixel loop using bounding box
		for (auto r = static_cast<uint32_t>(m_pBox->m_topLeft.y); static_cast<int>(r) < m_pBox->m_RighBottom.y && r < m_Height; ++r)
		{
			for (auto c = static_cast<uint32_t>(m_pBox->m_topLeft.x); static_cast<int>(c) < m_pBox->m_RighBottom.x && c < m_Width; ++c)
			{
				float cc = (float)c;
				float rr = (float)r;

				FPoint2 pixel = { cc,rr };
				Vertex transformedVertex;
				
				// check if point is in triangle
				if (triangle->PointInTriangle(pixel, m_depthBuffer[static_cast<uint64_t>(c) + (static_cast<uint64_t>(r) * m_Width)], finalColor, transformedVertex,m_pCamera->GetViewToWorldSoftware()))
				{
					// do pixel shading if point is in triangle
					finalColor = PixelShading(transformedVertex, Elite::GetNormalized(Elite::FPoint3(transformedVertex.WorldPosition.xyz) - m_pCamera->m_Position));
					finalColor.MaxToOne();
					m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<uint8_t>(finalColor.r * 255.f),
						static_cast<uint8_t>(finalColor.g * 255.f),
						static_cast<uint8_t>(finalColor.b * 255.f));
				}
			}
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}
void Elite::Renderer::Update(float elapsedSec)
{
	// Rotate triangle 
	for (Triangle* triangle : m_pTriangles)
	{
		// rotate mesh check
		if (m_RotateSoftWareMesh)
		{
			triangle->update(elapsedSec);
		}
	}
	//Rotate();
	m_ElapsedSec = elapsedSec;
	if (m_RotateSoftWareMesh)
	{
		m_rotation += elapsedSec;
	}
	m_pCamera->Update(elapsedSec, m_Width, m_Height);
	GetKeyboardInputs(elapsedSec);
	m_pMesh->update(elapsedSec);
	m_pMeshFire->update(elapsedSec);

}
const RGBColor& Elite::Renderer::PixelShading(const Vertex& transformedVertex, const FVector3& viewDirection)
{
	m_Finalcolor = { 0,0,0 };
	
	Elite::FMatrix4 rotationMatrix = Elite::MakeRotation(m_rotation, Elite::FVector3{ 0,1,0 });
	auto worldmatrix = m_pCamera->GetViewToWorldSoftware();
	auto lightDir = worldmatrix * rotationMatrix* m_LightDirection;
	//reflect calculation
	auto reflect = Elite::Reflect(lightDir.xyz, transformedVertex.Normal);

	float cosin = Elite::Dot(reflect, viewDirection);
	cosin = Elite::Clamp(cosin, 0.f, 1.f);
	float shininess = 25.f;

	//sample glossiness map
	m_pGlossiness = m_pGlossinessMap->Sample(transformedVertex.UV);
	
	//sample specular map
	m_SpecularRef = m_pSpecularMap->Sample(transformedVertex.UV);

	Elite::RGBColor phongSpecReflect = m_pGlossiness * (pow(cosin, shininess * m_SpecularRef.r));

	// binormal
	m_Binormal = Cross(transformedVertex.Tangent, transformedVertex.Normal);
	FMatrix3 tangentSpaceAxis = FMatrix3(transformedVertex.Tangent, m_Binormal, transformedVertex.Normal);

	// sample normal map
	RGBColor normalMap = m_NormalTexture->Sample(transformedVertex.UV);

	//matrix multiplication to take to tangent space
	m_Tangent = tangentSpaceAxis * FVector3{ normalMap.r,normalMap.g,normalMap.b };

	// observed area
	m_ObservedArea = Dot(-m_Tangent, lightDir.xyz);
	m_ObservedArea = std::max(0.f, m_ObservedArea);
	float intensity{ 2.f };
	{
		// sample diffusemap color from png texture
		m_DiffuseMapColor = m_pDiffuseMap.get()->Sample(transformedVertex.UV);
		m_Finalcolor = phongSpecReflect + m_DiffuseMapColor *intensity* m_ObservedArea;
	}
	
	return m_Finalcolor;

}


void Elite::Renderer::Rotate()
{
	const float dt = 0.01f;
	FMatrix3 rotationMax = Elite::MakeRotation(dt * 3.f, { 0,1,0 });
	
	for (auto& m : m_VerticesObjSoftware)
	{
		m.Position.xyz = rotationMax * m.WorldPosition.xyz;
	}
	for (auto& v : m_VerticesObj)
	{
		v.Position = rotationMax * v.worldPos.xyz;
	}
	
}
void Elite::Renderer::AddObjects()
{
	// make unique pointers for each maps
	m_pDiffuseMap = std::make_unique<Texture>(m_pDevice, "../../BUILD/Resources/vehicle_diffuse.png");
	m_NormalTexture = std::make_unique<Texture>(m_pDevice, "../../BUILD/Resources/vehicle_normal.png");
	m_pGlossinessMap = std::make_unique<Texture>(m_pDevice, "../../BUILD/Resources/vehicle_gloss.png");
	m_pSpecularMap = std::make_unique<Texture>(m_pDevice, "../../BUILD/Resources/vehicle_specular.png");
	m_pDiffuseMapFire = std::make_unique<Texture>(m_pDevice, "../../BUILD/Resources/fireFX_diffuse.png");
	
	// parse vertices and indices for vehicle
	ParseOBJ("../../BUILD/Resources/vehicle.obj", m_VerticesObj, m_Indices);

	//parse vertices and indices for fire
	ParseOBJ("../../BUILD/Resources/fireFX.obj", m_FireVertices, m_FireIndices);

	// create tangent
	CreateTangent();

	//make triangle from vertices and indices
	MakeTriangle();


	//add box 
	FPoint2 leftbottom = { 100,20 };
	float widthBox = (400.f);
	float heightBox = (400.f);
	m_pBox = std::make_unique<Box>(leftbottom, widthBox, heightBox);
}

void Elite::Renderer::MakeTriangle()
{
	// use vertices to create a triangle 
	Vertex ver1, ver2, ver3;

	Vertex p1, p2, p3;
		for (int i = 0; i < m_Indices.size();)
		{
			p1 = m_VerticesObjSoftware.at(m_Indices.at(i)); //0
			ver1 = p1;
			i++;

			p2 = m_VerticesObjSoftware.at(m_Indices.at(i)); //1
			ver2 = p2;
			i++;

			p3 = m_VerticesObjSoftware.at(m_Indices.at(i)); //2
			ver3 = p3;
			i++;

			m_pTriangles.push_back(new Triangle(ver1, ver2, ver3));
		
		}
}


bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Renderer::CreateTangent()
{

	for (uint32_t i = 0; i < m_Indices.size(); i += 3)
	{

		uint32_t index0 = m_Indices[i];
		uint32_t index1{};
		uint32_t index2{};

		if (i + 1.f < m_Indices.size())
		{
			index1 = m_Indices[static_cast<uint64_t>(i) + 1];
		}
		if (i + 2.f < m_Indices.size())
		{
			index2 = m_Indices[static_cast<uint64_t>(i) + 2];

		}
		const FPoint4& p0 = m_VerticesObj[index0].Position;
		const FPoint4& p1 = m_VerticesObj[index1].Position;
		const FPoint4& p2 = m_VerticesObj[index2].Position;

		const FPoint2& uv0 = m_VerticesObj[index0].UV;
		const FPoint2& uv1 = m_VerticesObj[index1].UV;
		const FPoint2& uv2 = m_VerticesObj[index2].UV;

		const FVector4 edge0 = p1 - p0;
		const FVector4 edge1 = p2 - p0;
		const FVector2 diffX = FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
		const FVector2 diffY = FVector2(uv1.y - uv0.y, uv2.y - uv0.y);

		float r = 1.f / Cross(diffX, diffY);

		FVector4 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		m_VerticesObj[index0].Tangent += {tangent.x, tangent.y, tangent.z};
		m_VerticesObj[index1].Tangent += {tangent.x, tangent.y, tangent.z};
		m_VerticesObj[index2].Tangent += {tangent.x, tangent.y, tangent.z};
	}

	for (auto& v : m_VerticesObj)
	{
		v.Tangent = Elite::GetNormalized(Elite::Reject(v.Tangent, v.Normal));
		v.Tangent.z *= -1;
	}

}

bool Renderer::ParseOBJ(const std::string& filename, std::vector<VS_INPUT>& vertices, std::vector<int>& indices)
{
	std::ifstream file(filename);
	if (!file)
		return false;

	std::vector<FPoint3> positions;
	std::vector<FVector3> normals;
	std::vector<FVector2> UVs;

	vertices.clear();
	indices.clear();

	std::string sCommand;
	// start a while iteration ending when the end of file is reached (ios::eof)
	while (!file.eof())
	{
		//read the first word of the string, use the >> operator (istream::operator>>) 
		file >> sCommand;
		//use conditional statements to process the different commands	
		if (sCommand == "#")
		{
			// Ignore Comment
		}
		else if (sCommand == "v")
		{
			//Vertex
			float x, y, z;
			file >> x >> y >> z;
			positions.push_back(FPoint3(x, y, z));
		}
		else if (sCommand == "vt")
		{
			// Vertex TexCoord
			float u, v;
			file >> u >> v;
			UVs.push_back(FVector2(u, 1 - v));
		}
		else if (sCommand == "vn")
		{
			// Vertex Normal
			float x, y, z;
			file >> x >> y >> z;
			normals.push_back(FVector3(x, y, z));
		}
		else if (sCommand == "f")
		{
			//if a face is read:
			//construct the 3 vertices, add them to the vertex array
			//add three indices to the index array
			//add the material index as attibute to the attribute array
			//
			// Faces or triangles
			VS_INPUT vertex{};
			size_t iPosition, iTexCoord, iNormal;
			for (size_t iFace = 0; iFace < 3; iFace++)
			{
				// OBJ format uses 1-based arrays
				file >> iPosition;
				vertex.Position = positions[iPosition - 1];
				
				//FMatrix4 worldmat = m_pCamera->GetViewToWorldHardware();
				//vertex.worldPos = worldmat *FPoint4(vertex.Position.x,vertex.Position.y,vertex.Position.z,1);
				if ('/' == file.peek())//is next in buffer ==  '/' ?
				{
					file.ignore();//read and ignore one element ('/')

					if ('/' != file.peek())
					{
						// Optional texture coordinate
						file >> iTexCoord;
						
						vertex.UV = Elite::FPoint2(UVs[iTexCoord - 1].x, UVs[iTexCoord - 1].y);
					}
					if ('/' == file.peek())
					{
						file.ignore();
						// Optional vertex normal
						file >> iNormal;
						vertex.Normal = Elite::FVector3(normals[iNormal - 1].x, normals[iNormal - 1].y, normals[iNormal - 1].z);
					}
				}

				vertices.push_back(vertex);
				indices.push_back(vertices.size() - 1);
			}
		}
		//read till end of line and ignore all remaining chars
		file.ignore(1000, '\n');
	}

	// rearrange the mesh
	Vertex vert;
	for (const VS_INPUT& vertex : vertices)
	{
		
		vert.Position = vertex.Position;
		vert.Normal = { vertex.Normal.x,vertex.Normal.y, vertex.Normal.z };
		vert.WorldPosition = vertex.worldPos;
		vert.Tangent = { vertex.Tangent.x, vertex.Tangent.y,vertex.Tangent.z};
		vert.UV = { vertex.UV.x,vertex.UV.y };
		m_VerticesObjSoftware.push_back(vert);
	}
	return true;
}
	
// DIRECTX CLASSES
HRESULT Renderer::InitializeDirectX()
{
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;  //feature is a well defined set of GPU functionlality

	uint32_t createDeviceFlags = 0; // flag to enable layers... use this if the app calls the methods of dx3d_11 from a single thread

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// create a device that represents the display adapter
	// &m_pDevice ---->  Returns the address of a pointer to an ID3D11Device object that represents the device created
	HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);

	// check if the creation is successful
	if (FAILED(result))
		return result;

	// create DXGIFactory that can be used to generate other objects (also to create SWAP CHAIN)
	// it gives more performace and saves memory because it creates chain according to the GPU hardware.
	result = CreateDXGIFactory(_uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));  //Creates a DXGI 1.0 factory that you can use to generate other DXGI objects.
	if (FAILED(result))
		return result;

	// creation of swapchain descriptor
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	//Get the handle HWND from the SDL backbuffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	//create the swapchain and hook it into the handle of the SDL window
	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result))
		return result;

	//Create the Depth/Stencil Buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc{};   // buffer used to mask pixels in an image... it controls whether a pixel is drawn or not// using buffer you enable or disable the drawing to the RENDER TARGET
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// describe the resource view for our depth/stencil buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// create matching resource view
	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer); // ID3D11Texture2d or ID3D11Resource
	if (FAILED(result))
		return result;

	// create depth stencil view
	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
		return result;


	// create Render Target view 
	// get back buffer from swap chain 
	// create resource view for the buffer
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&m_pRenderTargetBuffer));
	if (FAILED(result))
		return result;
	// create render target view from the buffer
	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
	if (FAILED(result))
		return result;

	// Bind the view to the output Merget Stage
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// SETTING THE VIEW PORT
	D3D11_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(m_Width);
	viewPort.Height = static_cast<float>(m_Height);
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewPort);

	return result;
}

void Elite::Renderer::GetKeyboardInputs(float elapsedSec)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	m_elapsedSecCounter += elapsedSec * 100.f;
	if (m_elapsedSecCounter > 10)
	{
		m_elapsedSecCounter = 0;
		if (state[SDL_SCANCODE_E])
		{
			if (m_toggleRasterizer)
			{
				std::cout << "software Rasterizer Running...\n";
				m_toggleRasterizer = false;
				m_pCamera->SetDirectXRasterizationActive(true);
			}
			else if (!m_toggleRasterizer)
			{
				std::cout << "DirectX Rasterizer Running...\n";
				m_toggleRasterizer = true;
				m_pCamera->SetDirectXRasterizationActive(false);
			}
		}
		if (m_toggleRasterizer && state[SDL_SCANCODE_T])
		{
			if (m_ShowTail)
			{
				std::cout << "Tail not visible\n";
				std::cout << "Press T to show tail";
				m_ShowTail = false;
			}
			else if (!m_ShowTail)
			{
				m_ShowTail = true;
			}
		}

		//control rotation
		if (state[SDL_SCANCODE_R])
		{
			m_pMesh->SetRotate();
			m_pMeshFire->SetRotate();

			//toggle rotation 
			if (m_RotateSoftWareMesh)
				m_RotateSoftWareMesh = false;
			else
				m_RotateSoftWareMesh = true;
		}
		
	}
}
