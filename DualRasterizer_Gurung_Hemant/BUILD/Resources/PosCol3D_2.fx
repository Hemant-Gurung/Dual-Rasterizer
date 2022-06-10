
float4x4  gWorldViewProj     : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

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

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.UV = input.UV;
	return output;
}
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

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
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

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border;// or Mirror or Clamp or Border or Wrap
	AddressV = Clamp;// or Mirror or Clamp or Border or Wrap
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = true;
};


float4 PS(VS_OUTPUT input) :SV_TARGET
{
	// viewDirection and phongSpecReflection
	return gDiffuseMap.Sample(samPoint, input.UV);
	
}

technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}