cbuffer cbPerFrame
{
    matrix gWorld;
    matrix gView;
    matrix gProj;
    matrix gUvWorld;
    float4 gTextureDiffuse;
};

Texture2D gTexture;

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = WRAP;
    AddressV = WRAP;
};

struct VertexIn
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};
	
VertexOut VS(VertexIn vin)
{
    VertexOut vout;
	
	// Transform to world space space.
    vout.Pos = mul(float4(vin.Pos, 1.0f), gWorld);
    vout.Pos = mul(vout.Pos, gView);
    vout.Pos = mul(vout.Pos, gProj);
	
	// Output vertex attributes for interpolation across triangle.
    vout.Color = vin.Color;
    
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gUvWorld).xy;
   
    return vout;
}

float4 PS_Texture(VertexOut pin) : SV_Target
{
    return gTexture.Sample(samAnisotropic, pin.Tex) * gTextureDiffuse;
}

float4 PS_Color(VertexOut pin) : SV_Target
{
    return pin.Color * gTextureDiffuse;
}

technique11 TechTexture
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }
}

technique11 TechColor
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Color()));
    }
}