cbuffer cbPerFrame
{
    matrix gWorld;
    matrix gView;
    matrix gProj;
    matrix gUvWorld;
    float4 gTextureDiffuse;
    uint gTextureID;
};

Texture2DArray gTexture;

SamplerState samAnisotropic
{
    Filter = MIN_MAG_MIP_LINEAR;
    //MaxAnisotropy = 4;

    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

struct VertexIn
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};

struct InstancedData
{
    row_major matrix World : WORLD;
    row_major matrix UvWorld : UVWORLD;
    float4 Diffuse : DIFFUSE;
    uint TextureID : TEXTUREID;
};

struct VertexOut
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
    float4 Diffuse : DIFFUSE;
    uint TextureID : TEXTUREID;
};
	
VertexOut VS_Instanced(VertexIn vin, InstancedData inst)
{
    VertexOut vout;
	
	// Transform to world space space.
    vout.Pos = mul(float4(vin.Pos, 1.0f), inst.World);
    vout.Pos = mul(vout.Pos, gView);
    vout.Pos = mul(vout.Pos, gProj);
	
    vout.Color = vin.Color;
    
    vout.Diffuse = inst.Diffuse;
    vout.TextureID = inst.TextureID;
    
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), inst.UvWorld).xy;
   
    return vout;
}


VertexOut VS(VertexIn vin)
{
    VertexOut vout;
	
	// Transform to world space space.
    vout.Pos = mul(float4(vin.Pos, 1.0f), gWorld);
    vout.Pos = mul(vout.Pos, gView);
    vout.Pos = mul(vout.Pos, gProj);
	
    vout.Color = vin.Color;
    vout.Diffuse = gTextureDiffuse;
    vout.TextureID = gTextureID;
    
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gUvWorld).xy;
   
    return vout;
}

float4 PS_Texture(VertexOut pin) : SV_Target
{
    return gTexture.Sample(samAnisotropic, float3(pin.Tex, (float)pin.TextureID)) * pin.Diffuse;
    //return gTexture.Sample(samAnisotropic, pin.Tex) * pin.Diffuse;
}

float4 PS_Color(VertexOut pin) : SV_Target
{
    return pin.Color * pin.Diffuse;
}

technique11 TechTextureInstanced
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Instanced()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }
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
