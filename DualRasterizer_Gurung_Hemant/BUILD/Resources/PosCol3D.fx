//------------------------
//  Input/Output Structs
//------------------
float4x4  gWorldViewProj     : WorldViewProjection;
Texture2D gDiffuseMap        : DiffuseMap;
Texture2D gNormalMap         : NormalMap;
Texture2D gSpecularMap       : SpecularMap;
Texture2D gGlossinessMap     : GlossinessMap;

float3    gLightDirection    : LightDirection;
float4x4  gWorldMatrix       : WORLD;
float4x4  gViewInverseMatrix : VIEWINVERSE;

// extra
//float PI{ 3.1415f };
//float gLightIntensity{ 7.0f };
//float shininess{ 25.0f };

struct VS_INPUT
{
	float3 Position      :POSITION;
	float2 UV            :TEXCOORD;
	float3 Normal		 :NORMAL;
	float3 Tangent       :TANGENT;
	float4 WorldPosition :WPOS;
};

struct VS_OUTPUT
{
	float4 Position      :SV_Position;
	float2 UV            :TEXCOORD;
	float3 Normal		 :NORMAL;
	float3 Tangent       :TANGENT;
	float4 WorldPosition :WPOS;
};

RasterizerState gRasterizerState
{
	CullMode = front;
	FrontCounterClockwise = false;
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border;// or Mirror or Clamp or Border or Wrap
	AddressV = Clamp;// or Mirror or Clamp or Border or Wrap
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border;// or Mirror or Clamp or Border or Wrap
	AddressV = Clamp;// or Mirror or Clamp or Border or Wrap
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

float3 PhongShade(float3 viewDir, VS_OUTPUT input,float3 normal)
{
	float3 phongReflect = reflect(gLightDirection,input.Normal);

	float  cosin = saturate(dot(phongReflect, viewDir));

	float4 phongExp = gSpecularMap.Sample(samPoint, input.UV);
	float3 phongExpColor = phongExp.rgb;
	float4 phongReflectance = gGlossinessMap.Sample(samPoint, input.UV);
	float3 phongColor = phongReflectance.rgb;
	float3 phongSpecReflect = phongColor * (pow(cosin, phongExpColor.g * 25.f));
	return phongSpecReflect;
}

//Vertex Shader 
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output =  (VS_OUTPUT)0;
	output.Position  =  mul(float4(input.Position, 1.f), gWorldViewProj);
	output.Normal    =  mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent   =  mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.UV        =  input.UV;
	return output;
}

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;
	StencilEnable = false;

	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;

	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;

};

// blend state
BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

//Pixel Shader
float4 PSPoint(VS_OUTPUT input) :SV_TARGET
{
	// viewDirection and phongSpecReflection
	 float3 binormal = cross(input.Tangent,input.Normal);
	 float3x3 tangentsSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);
	 float4 normalmap = gNormalMap.Sample(samPoint, input.UV);
	 float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);
	 float3 phongSpecReflect = PhongShade(viewDirection, input,normalmap.rgb);

	 float3 tangent = mul(tangentsSpaceAxis, normalmap.rgb);
	 float  observedArea = saturate(dot(tangent,gLightDirection)); 
	 float intensity = 1.5f;
	float4 diffuseMap = gDiffuseMap.Sample(samPoint, input.UV);
	float3 diffuseMapColor = diffuseMap.rgb;
	 
	return float4(0.025f,0.025f,0.025f,0.025f)+float4(phongSpecReflect,1.f)+( float4(diffuseMapColor,1.f)*intensity* observedArea);
}
//
//float4 PSLinear(VS_OUTPUT input) :SV_TARGET
//{
//	 float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);
//	 float4 normalmap = gNormalMap.Sample(samLinear, input.UV);
//	 //normalmap = normalmap * 2 - float4(1.f, 1.f, 1.f, 1.f);
//	 float3 phongReflect = PhongShade(viewDirection,input,normalmap.rgb);
//	 float cosin = saturate(dot(phongReflect, viewDirection));
//
//	 float4 phongExp = gSpecularMap.Sample(samLinear, input.UV);
//	 float4 phongReflectance = gGlossinessMap.Sample(samLinear, input.UV);
//
//	 float4 phongSpecReflect = phongReflectance * (pow(cosin, phongExp.x * 25.f));
//	 float3 binormal = cross(input.Normal,input.Tangent);
//	 float3x3 tangentsSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);
//
//	
//	 float3 tangent = mul(tangentsSpaceAxis, normalmap.rgb);
//	 float observedArea = dot(-tangent, gLightDirection);
//
//	 //(specRef.r,specRef.g,specRef.b,1.0f)
//	float4 diffuseMap = gDiffuseMap.Sample(samLinear, input.UV);
//	//finalcolor+(specRef.r, specRef.g, specRef.b, 1.0f);
//  return phongSpecReflect + diffuseMap*observedArea;
//}

//Technique
technique11 DefaultTechniquePoint
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);  // set counterclock wise triangle
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSPoint()));
	}

	//pass P1
	//{
	//	SetRasterizerState(gRasterizerState);  // set counterclock wise triangle
	//	SetVertexShader(CompileShader(vs_5_0, VS()));
	//	SetGeometryShader(NULL);
	//	SetPixelShader(CompileShader(ps_5_0, PSLinear()));
	//}
}
