cbuffer cbPerFrame
{
    matrix gWorld;
    matrix gView;
    matrix gProj;
};

struct VertexIn
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
    row_major matrix World : WORLD;
};

struct VertexOut
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};
	
VertexOut VS(VertexIn vin)
{
    VertexOut vout;
	
	// Transform to world space space.
    vout.Pos = mul(float4(vin.Pos, 1.0f), gWorld);
    vout.Pos = mul(vout.Pos, gView);
    vout.Pos = mul(vout.Pos, gProj);
	
	// Output vertex attributes for interpolation across triangle.
    vout.Color = vin.Color;// * vin.Diffuse;
    
   
    return vout;
}
	
VertexOut VS_Instanced(VertexIn vin)
{
    VertexOut vout;
	
	// Transform to world space space.
    vout.Pos = mul(float4(vin.Pos, 1.0f), vin.World);
    vout.Pos = mul(vout.Pos, gView);
    vout.Pos = mul(vout.Pos, gProj);
	
	// Output vertex attributes for interpolation across triangle.
    vout.Color = vin.Color; // * vin.Diffuse;
    
   
    return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}

technique11 Tech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

technique11 TechInstanced
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Instanced()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}